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
#include "ota_partition.h"

typedef struct partition_param_st {
    ota_updt_type_t     updt_type;

    ota_img_vs_t        version;
    ota_pts_t           pts;
} partition_param_t;

static int partitions_verify(ota_pt_t *pts, int n)
{
    int i = 0;

    for (i = 0; i < n; ++i) {
        if (pts[i].start == 0 || pts[i].start == (uint32_t)-1 ||
            pts[i].end == 0 || pts[i].end == (uint32_t)-1) {
            return -1;
        }

        if (pts[i].start >= pts[i].end) {
            return -1;
        }
    }

    return 0;
}

static void usage(void)
{
    fprintf(stderr,
            "Usage: ptbl [-h] -p option -a active_app_pt [-b backup_app_pt] -o ota_pt -k kv_pt -v version pt_tbl_image\n"
            "     -h  help\n"
            "     -p  option[ip or pp]\n"
            "         if option is ip, using in-position update stragety\n"
            "            in this situation, MUST no backup_app_pt is supplied\n"
            "         if option is pp, using ping-pong update stragety\n"
            "            in this situation, MUST ONE backup_app_pt is supplied\n"
            "     -a  active application partition, format: pt_start,pt_end\n"
            "     -b  backup application partition, format: pt_start,pt_end\n"
            "     -o  ota partition, format: pt_start,pt_end\n"
            "     -k  kv partition, format pt_start,pt_end\n"
            "     -v  version of the initial application, format: two number separated by dot\n"
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

static void process_command_line(int argc, char **argv, partition_param_t *param)
{
    int c;
    uint32_t start, end;
    uint32_t version_major, version_minor;

    while ((c = getopt(argc, argv, "hp:a:b:o:k:v:")) != -1) {
        switch (c) {
        case 'h':
            usage();
            break;

        case 'p':
            if (strncmp("ip", optarg, 2) == 0) {
                param->updt_type = OTA_UPDATE_IN_POSITION;
            } else if (strncmp("pp", optarg, 2) == 0) {
                param->updt_type = OTA_UPDATE_PING_PONG;
            } else {
                usage();
            }
            break;

        case 'a':
            if (sscanf(optarg, "%x,%x", &start, &end) < 2) {
                panic("invalid partition %s\n", optarg);
            }

            if (param->updt_type == OTA_UPDATE_IN_POSITION) {
                param->pts.ip.ip_pts.active_app.start   = start;
                param->pts.ip.ip_pts.active_app.end     = end;
            } else if (param->updt_type == OTA_UPDATE_PING_PONG) {
                param->pts.pp.pp_pts.active_app.start   = start;
                param->pts.pp.pp_pts.active_app.end     = end;
            } else {
                usage();
            }

            break;

        case 'b':
            if (sscanf(optarg, "%x,%x", &start, &end) < 2) {
                panic("invalid partition %s\n", optarg);
            }

            if (param->updt_type == OTA_UPDATE_PING_PONG) {
                param->pts.pp.pp_pts.backup_app.start   = start;
                param->pts.pp.pp_pts.backup_app.end     = end;
            } else {
                usage();
            }

            break;

        case 'o':
            if (sscanf(optarg, "%x,%x", &start, &end) < 2) {
                panic("invalid partition %s\n", optarg);
            }

            if (param->updt_type == OTA_UPDATE_IN_POSITION) {
                param->pts.ip.ip_pts.ota.start      = start;
                param->pts.ip.ip_pts.ota.end        = end;
            } else if (param->updt_type == OTA_UPDATE_PING_PONG) {
                param->pts.pp.pp_pts.ota.start      = start;
                param->pts.pp.pp_pts.ota.end        = end;
            } else {
                usage();
            }

            break;

        case 'k':
            if (sscanf(optarg, "%x,%x", &start, &end) < 2) {
                panic("invalid partition %s\n", optarg);
            }

            if (param->updt_type == OTA_UPDATE_IN_POSITION) {
                param->pts.ip.ip_pts.kv.start       = start;
                param->pts.ip.ip_pts.kv.end         = end;
            } else if (param->updt_type == OTA_UPDATE_PING_PONG) {
                param->pts.pp.pp_pts.kv.start       = start;
                param->pts.pp.pp_pts.kv.end         = end;
            } else {
                usage();
            }

            break;

        case 'v':
            if (sscanf(optarg, "%u.%u", &version_major, &version_minor) < 2) {
                panic("invalid version format %s\n", optarg);
            }

            if (version_major > (uint8_t)-1 || version_minor > (uint8_t)-1) {
                panic("invalid version number: %d %d\n", version_major, version_minor);
            }

            param->version.major = version_major;
            param->version.minor = version_minor;
            break;

        default:
            usage();
            break;
        }
    }
}

void write_partition(partition_param_t *param, FILE *fp)
{
    uint8_t crc = 0;
    ota_pt_hdr_t hdr;

    hdr.magic   = OTA_PARTITION_MAGIC;
    hdr.version = param->version;

    crc         = partition_hdr_crc(&hdr);

    if (param->updt_type == OTA_UPDATE_IN_POSITION) {
        if (partitions_verify(&param->pts.ip.pts[0], sizeof(param->pts.ip.pts) / sizeof(ota_pt_t)) != 0) {
            panic("invalid partitions\n");
        }

        crc = partitions_crc(crc, &param->pts.ip.pts[0], sizeof(param->pts.ip.pts) / sizeof(ota_pt_t));
    } else if (param->updt_type == OTA_UPDATE_PING_PONG) {
        if (partitions_verify(&param->pts.pp.pts[0], sizeof(param->pts.pp.pts) / sizeof(ota_pt_t)) != 0) {
            panic("invalid partitions\n");
        }

        crc = partitions_crc(crc, &param->pts.pp.pts[0], sizeof(param->pts.pp.pts) / sizeof(ota_pt_t));
    } else {
        panic("invalid partitions\n");
    }

    hdr.crc     = crc;

    if (fwrite(&hdr, 1, sizeof(ota_pt_hdr_t), fp) != sizeof(ota_pt_hdr_t)) {
        fclose(fp);
        panic("failed to write partition header\n");
    }

    if (param->updt_type == OTA_UPDATE_IN_POSITION) {
        if (fwrite(&param->pts.ip.pts[0], 1, sizeof(param->pts.ip.pts), fp) != sizeof(param->pts.ip.pts)) {
            fclose(fp);
            panic("failed to write partitions\n");
        }
    } else if (param->updt_type == OTA_UPDATE_PING_PONG) {
        if (fwrite(&param->pts.pp.pts[0], 1, sizeof(param->pts.pp.pts), fp) != sizeof(param->pts.pp.pts)) {
            fclose(fp);
            panic("failed to write partitions\n");
        }
    } else {
        panic("invalid partitions\n");
    }
}

int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    char *pt_tbl_path = NULL;
    partition_param_t param = { 0 };

    memset(&param, 0, sizeof(partition_param_t));

    process_command_line(argc, argv, &param);

    if (optind + 1 != argc) {
        usage();
    }

    pt_tbl_path = argv[optind];

    if ((fp = fopen(pt_tbl_path, "wb")) == NULL) {
        panic("failed to open: %s\n", pt_tbl_path);
    }

    write_partition(&param, fp);

    fclose(fp);

    return 0;
}

