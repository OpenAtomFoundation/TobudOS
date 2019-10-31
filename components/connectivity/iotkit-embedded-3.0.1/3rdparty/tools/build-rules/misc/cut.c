#include "cut.h"

struct cut_runtime cut;

static char suite_pattern[64];
static char case_pattern[64];

static void _filter(int argc, char **argv)
{
    int i = 0;
    struct cut_case *c = NULL;
    if (argc == 2 && 0 == strcmp(argv[1], "all")) {
        return;
    }

    for (i = 0; i < cut.ccnt_total; i++) {
        c = cut.clist[i];
        if ((argc == 2 && (0 != strcmp(c->sname, argv[1]))) ||
            (argc == 3 && (0 != strcmp(c->sname, argv[1]) || 0 != strcmp(c->cname, argv[2])))) {

            if (!(argc == 2 && strlen(suite_pattern) && strstr(c->sname, suite_pattern))) {
                cut.clist[i]->skip = 1;
                cut.ccnt_skip++;
            }
        }
    }
}

static void _usage(const char *me)
{
    cut_printf("Usage: %s [OPTION] S-FILTER [C-FILTER]\n\n" \
               "OPTION:\n" \
               "  --verbose:    verbose when exec cases\n" \
               "  --list:       list cases\n" \
               "  --count:      print case count\n" \
               "\n" \
               "S-FILTER:    suite name filter, e.g. '%s all' means run all suites\n" \
               "C-FILTER:    case name filter\n", me, me);
}

static int _verbose_opt = 0;
static int _parse_arg(int argc, char **argv)
{
    if (argc >= 2) {
        if (0 == strcmp(argv[1], "--list")) {
            int i = 0;
            int cnt = 0;
            for (i = 0; i < cut.ccnt_total; i++) {
                struct cut_case *c = cut.clist[i];
                if (argc == 2 ||
                    (argc == 3 && 0 == strcmp(argv[2], "all")) ||
                    (argc == 3 && 0 == strcmp(c->sname, argv[2])) ||
                    (argc == 3 && strlen(suite_pattern) && strstr(c->sname, suite_pattern)) ||
                    (argc == 4 && strlen(suite_pattern) && strlen(case_pattern) && strstr(c->sname, suite_pattern)
                     && strstr(c->cname, case_pattern)) ||
                    (argc == 4 && 0 == strcmp(c->sname, argv[2]) && 0 == strcmp(c->cname, argv[3]))) {
                    cut_printf("  [%02d] %s.%s\n", ++cnt, c->sname, c->cname);
                }
            }
            cut_printf("\n");
            cut_printf("In total %d case(s), matched %d case(s)\n", cut.ccnt_total, cnt);
            cut_printf("\n");
            return 0;
        }
        if (0 == strcmp(argv[1], "--count")) {
            cut_printf("total %d case(s).\n", cut.ccnt_total);
            return 0;
        }
        if (0 == strcmp(argv[1], "--help")) {
            _usage(argv[0]);
            return 0;
        }
    }

    return 1;
}

int cut_main(int argc, char **argv)
{
    int         i = 0, j = 0, cnt = 0;
    char        tmpbuf[128];
    char       *pos;

    if (argc >= 2) {

        int         idx = 1;
        char        *q = NULL;

        if (!strcmp(argv[1], "--list") || !strncmp(argv[1], "--verbose", strlen("--verbose"))) {
            idx += 1;
        }

        if (idx < argc) {

            if ((q = strchr(argv[idx], '%')) != NULL) {
                strncpy(suite_pattern, argv[idx], q - argv[idx]);
            }
            idx += 1;
            if (idx < argc) {
                if ((q = strchr(argv[idx], '%')) != NULL) {
                    strncpy(case_pattern, argv[idx], q - argv[idx]);
                }
            }
        }
    }

    if (0 == _parse_arg(argc, argv)) {
        return 0;
    }

    if (argc >= 2 && !strncmp(argv[1], "--verbose", strlen("--verbose"))) {
        _verbose_opt = 1;
        argc --;
        argv ++;
    }

    _filter(argc, argv);

    for (; i < cut.ccnt_total; i++) {
        pos = tmpbuf;

        cut.ccur = cut.clist[i];
        if (cut.ccur->skip) {
            continue;
        }

        memset(tmpbuf, 0, sizeof(tmpbuf));
        pos += cut_snprintf(pos,
                            sizeof(tmpbuf),
                            "TEST [%02d/%02d] %s.%s ",
                            ++cnt,
                            cut.ccnt_total - cut.ccnt_skip,
                            cut.ccur->sname,
                            cut.ccur->cname);
        for (j = 80 - strlen(tmpbuf); j >= 0; --j) {
            pos += sprintf(pos, "%s", ".");
        }
        if (_verbose_opt) {
            pos += sprintf(pos, " [%sEXEC%s]\n", COL_YEL, COL_DEF);
            cut_printf("%s", tmpbuf);
            pos -= 19;
        }
        TRY {
            if (cut.ccur->setup)
            {
                cut.ccur->setup(cut.ccur->data, cut.ccur);
            }
            cut.ccur->run(cut.ccur->data, (struct cut_case *)cut.ccur);
            if (cut.ccur->teardown)
            {
                cut.ccur->teardown(cut.ccur->data);
            }

            pos += sprintf(pos, " [%sSUCC%s]\n", COL_GRE, COL_DEF);
            cut_printf("%s", tmpbuf);

            cut.ccnt_pass++;
            continue;
        }
        EXCEPT {

            pos += sprintf(pos, " [%sFAIL%s]\n", COL_RED, COL_DEF);
            cut_printf("%s", tmpbuf);

            cut.ccnt_fail++;
            continue;
        }
    }

    cut_printf("\n");
    cut_printf("===========================================================================\n");
    if (cut.ccnt_fail > 0) {
        cut_printf("FAIL LIST:\n");
        for (i = 0; i < cut.ccnt_fail; i++) {
            cut_printf("  [%02d] %s\n", i + 1, cut.cerrmsg[i]);
            cut_free(cut.cerrmsg[i]);
        }
        cut_printf("---------------------------------------------------------------------------\n");
    }
    cut_printf("SUMMARY:\n" \
               "     TOTAL:    %d\n" \
               "   SKIPPED:    %d\n" \
               "   MATCHED:    %d\n" \
               "      PASS:    %d\n" \
               "    FAILED:    %d\n", cut.ccnt_total, cut.ccnt_skip,
               cut.ccnt_total - cut.ccnt_skip, cut.ccnt_pass, cut.ccnt_fail);
    cut_printf("===========================================================================\n");

    return cut.ccnt_fail;

}

