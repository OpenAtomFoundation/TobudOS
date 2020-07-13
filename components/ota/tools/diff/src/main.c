#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"

#include "unistd.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "crc8.h"
#include "ota_diff.h"
#include "lzma_compress.h"
#include "ota_image.h"

#define ERROR(msg)                                                                          \
    printf("ERROR: line number[%d], function[%s] msg[%s]\n", __LINE__, __FUNCTION__, msg);  \
    rc = -1;                                                                                \
    goto OUT;

typedef struct args_param_st {
    int             is_verbose;
    int             is_safe_ignored;
} args_param_t;

typedef struct diff_param_st {
    uint16_t        block_len;
    ota_img_vs_t    new_version;
    ota_img_vs_t    old_version;
} diff_param_t;

static void usage(void)
{
    fprintf(stderr,
            "Usage: diff [-h] [-v] [-s] -b block_len -n new_version -o old_version old_image new_image patch_image\n"
            "     -h  help\n"
            "     -v  more information\n"
            "     -s  ingnore the safe block\n"
            "     -b  set block size\n"
            "     -n  version of new_image. format: two number separated by dot, {version_major.version_minor}\n"
            "     -o  version of old_image. format: two number separated by dot, {version_major.version_minor}\n"
            );
    exit(-1);
}

static void panic(const char *format, ...)
{
    va_list args;
    static char buffer[128];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    fprintf(stderr, buffer);
    exit(-1);
}

static int str2int(char *p)
{
    long int v;
    v = strtoll(p, &p, 0);
    switch (*p) {
    case 0:
        if (v < -(1 << 30)) {
            v = -(1 << 30);
        }
        if (v > (1 << 30)) {
            v = (1 << 30);
        }
        break;
    case 'k':
    case 'K':
        if (v < -(1 << 20)) {
            v = -(1 << 20);
        }
        if (v > (1 << 20)) {
            v = (1 << 20);
        }
        v *= 1024;
        break;
    case 'm':
    case 'M':
        if (v < -(1 << 10)) {
            v = -(1 << 10);
        }
        if (v > (1 << 10)) {
            v = (1 << 10);
        }
        v *= 1024 * 1024;
        break;
    case 'g':
    case 'G':
        if (v < -1) {
            v = -1;
        }
        if (v > 1) {
            v = 1;
        }
        v *= 1024 * 1024 * 1024;
        break;
    }
    return v;
}

static void process_command_line(int argc, char **argv, diff_param_t *param, args_param_t *args_param)
{
    int c, v;
    uint32_t version_major, version_minor;

    param->block_len = 0;

    while ((c = getopt(argc, argv, "vhsb:n:o:")) != -1) {
        switch (c) {
        case 'h':
            usage();
            break;

        case 'v':
            args_param->is_verbose = 1;
            break;

        case 's':
            args_param->is_safe_ignored = 1;
            break;

        case 'b':
            v = str2int(optarg);
            if (v < 64 || v > (1 << 20) || (v & (v - 1))) {
                panic("invalid block_len %s\n", optarg);
            }
            param->block_len = v;
            break;

        case 'n':
            if (sscanf(optarg, "%u.%u", &version_major, &version_minor) < 2) {
                panic("invalid new_version format %s\n", optarg);
            }

            if (version_major > (uint8_t)-1 || version_minor > (uint8_t)-1) {
                panic("invalid version number: %d %d\n", version_major, version_minor);
            }

            param->new_version.major = version_major;
            param->new_version.minor = version_minor;
            break;

        case 'o':
            if (sscanf(optarg, "%u.%u", &version_major, &version_minor) < 2) {
                panic("invalid new_version format %s\n", optarg);
            }

            if (version_major > (uint8_t)-1 || version_minor > (uint8_t)-1) {
                panic("invalid version number: %d %d\n", version_major, version_minor);
            }

            param->old_version.major = version_major;
            param->old_version.minor = version_minor;
            break;

        default:
            usage();
            break;
        }
    }
}

static uint8_t img_crc(uint8_t *img, size_t img_size, ota_img_hdr_t *img_hdr)
{
    uint8_t crc = 0;

    crc = crc8(crc, (uint8_t *)&img_hdr->magic, sizeof(uint16_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->new_version, sizeof(ota_img_vs_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->old_version, sizeof(ota_img_vs_t));

    crc = crc8(crc, (uint8_t *)&img_hdr->new_crc, sizeof(uint8_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->new_size, sizeof(uint32_t));

    crc = crc8(crc, (uint8_t *)&img_hdr->old_crc, sizeof(uint8_t));
    crc = crc8(crc, (uint8_t *)&img_hdr->old_size, sizeof(uint32_t));

    crc = crc8(crc, (uint8_t *)&img_hdr->patch_size, sizeof(uint32_t));
    crc = crc8(crc, img, img_size);

    return crc;
}

int make_patch(uint8_t *new, size_t new_size,
                    uint8_t *old, size_t old_size,
                    diff_param_t *diff_param,
                    args_param_t *args_param,
                    FILE *patch_file)
{
    int rc = 0;
    ota_img_hdr_t img_hdr;
    uint8_t *patch = NULL;
    size_t patch_size;

    if (ota_diff(old, old_size, new, new_size, diff_param->block_len, args_param->is_safe_ignored, args_param->is_verbose, &patch, &patch_size) != 0) {
        ERROR("ota_diff failed\n");
    }

    img_hdr.magic               = OTA_IMAGE_MAGIC;
    img_hdr.new_version         = diff_param->new_version;
    img_hdr.old_version         = diff_param->old_version;

    img_hdr.new_size            = new_size;
    img_hdr.new_crc             = crc8(0, new, new_size);

    img_hdr.old_size            = old_size;
    img_hdr.old_crc             = crc8(0, old, old_size);

    img_hdr.patch_size          = patch_size;
    img_hdr.patch_crc           = img_crc(patch, patch_size, &img_hdr);

    if (fwrite(&img_hdr, 1, sizeof(ota_img_hdr_t), patch_file) != sizeof(ota_img_hdr_t)) {
        ERROR("write header failed\n");
    }

    if (fwrite(patch, 1, patch_size, patch_file) != patch_size) {
        ERROR("write patch failed\n");
    }

OUT:
    if (patch) {
        free(patch);
    }

    return rc;
}

int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    size_t new_size, old_size;
    uint8_t *new = NULL, *old = NULL;
    char *new_path, *old_path, *patch_path;
    args_param_t args_param = { 0 };
    diff_param_t diff_param = { 0 };

    memset(&args_param, 0, sizeof(args_param_t));
    memset(&diff_param, 0, sizeof(diff_param_t));

    process_command_line(argc, argv, &diff_param, &args_param);

    if (optind + 3 != argc) {
        usage();
    }

    old_path    = argv[optind];
    new_path    = argv[optind + 1];
    patch_path  = argv[optind + 2];

    if (((fp = fopen(new_path, "rb")) == NULL) ||
                (fseek(fp, 0, SEEK_END) == -1) ||
                ((new_size = ftell(fp)) == -1) ||
                ((new = malloc(new_size + 1)) == NULL) ||
                (fseek(fp, 0, SEEK_SET) != 0) ||
                (fread(new, 1, new_size, fp) != new_size) ||
                (fclose(fp) == -1)) {
        printf("failed to open: %s\n", new_path);
        goto errout;
    }

    if (((fp = fopen(old_path, "rb")) == NULL) ||
                (fseek(fp, 0, SEEK_END) == -1) ||
                ((old_size = ftell(fp)) == -1) ||
                ((old = malloc(old_size + 1)) == NULL) ||
                (fseek(fp, 0, SEEK_SET) != 0) ||
                (fread(old, 1, old_size, fp) != old_size) ||
                (fclose(fp) == -1)) {
        printf("failed to open: %s\n", old_path);
        goto errout;
    }

    /* Create the patch file */
    if ((fp = fopen(patch_path, "wb")) == NULL) {
        printf("failed to open: %s\n", patch_path);
        goto errout;
    }

    if (make_patch(new, new_size,
                    old, old_size,
                    &diff_param,
                    &args_param,
                    fp) != 0) {
        printf("make patch failed\n");
        goto errout;
    }

errout:
    if (fp) {
        fclose(fp);
    }

    if (new) {
        free(new);
    }

    if (old) {
        free(old);
    }

    return 0;
}

