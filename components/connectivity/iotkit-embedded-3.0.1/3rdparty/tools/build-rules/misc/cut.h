#ifndef __CUT_H__
#define __CUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <assert.h>

#define CUT_CASE_MAX_CNT        (1024)
#define CUT_MSG_MAX_LEN         (512)

#define cut_printf              printf
#define cut_snprintf            snprintf
#define cut_malloc              malloc
#define cut_free                free

#define CASE2                   CASEs

extern int cut_main(int argc, char **argv);
extern struct cut_runtime cut;

struct cut_case {
    const char *sname;
    const char *cname;
    void *data;
    void (*run)(void *, void *);
    void (*setup)(void *, void *);
    void (*teardown)(void *);
    int skip;
};

struct cut_runtime {
    jmp_buf jmpbuf;
    int     scnt_total;
    int     ccnt_total;
    int     ccnt_pass;
    int     ccnt_fail;
    int     ccnt_skip;
    struct  cut_case *clist[CUT_CASE_MAX_CNT];
    struct  cut_case *ccur;
    char    *cerrmsg[CUT_CASE_MAX_CNT];
};

#define CUT_CASE_RUNNER(sname, cname)  cut_##sname##_##cname##_run
#define CUT_CASE_NAME(sname, cname)    cut_##sname##_##cname
#define CUT_CASE_DATA(sname)           cut_##sname##_data
#define CUT_CASE_SETUP(sname)          cut_##sname##_setup
#define CUT_CASE_TEARDOWN(sname)       cut_##sname##_teardown

#define DATA(sname) \
    struct CUT_CASE_DATA(sname)

#define SETUP(sname) \
    static void CUT_CASE_SETUP(sname)(struct CUT_CASE_DATA(sname) *data, struct cut_case *case_data)

#define TEARDOWN(sname) \
    static void CUT_CASE_TEARDOWN(sname)(struct CUT_CASE_DATA(sname) *data)


/*
 * @brief: construct a test case structor and a test case runner
 * @sname: suite name
 * @cname: case name
 * e.g.
    CASE(mysuite, mycase1) {
        // do something here
        ASSERT_TRUE(1);
    }
 */
#define CASE(sname, cname) \
    static void CUT_CASE_RUNNER(sname, cname)(void *null, struct cut_case *case_data); \
    static struct cut_case CUT_CASE_NAME(sname, cname) = \
            { \
              #sname, #cname, NULL, (void(*)(void*,void*))CUT_CASE_RUNNER(sname, cname), NULL, NULL, 0}; \
    static void CUT_CASE_RUNNER(sname, cname)(void *null, struct cut_case *case_data)

/*
 * @brief: construct a test case structor and a test case runner
 *         with case_data/setup/teardown for each case.
 * @sname: suite name
 * @cname: case name
 * e.g.
    CASE_DATA(mysuite) {
        int errmsg;
        char *errcode;
    };

    CASE_SETUP(mysuite) {
        data->errcode = 0;
        data->errmsg = (char*)malloc(100);
    }

    CASE_TEARDOWN(mysuite) {
        if(data->errmsg) {
            free(data->errmsg);
            data->errmsg = NULL;
        }
    }

    CASEs(mysuite, mycase1) {
        data->errcode = 1;
        strcpy(data->errmsg, "timeout error");
        ASSERT_TRUE(1);
    }
 */
#define CASEs(sname, cname)                                                        \
    static struct CUT_CASE_DATA(sname) CUT_CASE_DATA(sname);                       \
    static void CUT_CASE_RUNNER(sname, cname)(struct CUT_CASE_DATA(sname) * data,struct cut_case *case_data); \
    static struct cut_case CUT_CASE_NAME(sname, cname) = \
            { \
              #sname, #cname, &CUT_CASE_DATA(sname), (void(*)(void*,void*))CUT_CASE_RUNNER(sname, cname),      \
              (void(*)(void*,void*))CUT_CASE_SETUP(sname), (void(*)(void*))CUT_CASE_TEARDOWN(sname), 0}; \
    \
    static void CUT_CASE_RUNNER(sname, cname)(struct CUT_CASE_DATA(sname) * data, struct cut_case *case_data)

/*
 * @brief: construct a test suite by adding test case(s)
 * @sname: suite name
 * e.g.
    SUITE(mysuite) = {
        ADD_CASE(mysuite, mycase1),
        ADD_CASE(mysuite, mycase2),
        ADD_CASE_NULL
    };
 */
#define SUITE(sname) struct cut_case *cut_suite_##sname[]

/*
 * @brief: add a test case into a test suite
 * @sname: suite name
 * @cname: case name
 */
#define ADD_CASE(sname, cname)   &CUT_CASE_NAME(sname, cname)
#define ADD_CASE_NULL            (struct cut_case*)(NULL)

/*
 * @brief: add a test suite into case list
 * @sname: suite name
 */
#define ADD_SUITE(sname)                                  \
    do {                                                  \
        int i = 0;                                        \
        extern struct cut_case *cut_suite_##sname[];      \
        struct cut_case *c = cut_suite_##sname[i];        \
        if (cut.ccnt_total >= CUT_CASE_MAX_CNT) {         \
            cut_printf("reaches maximum case count:%d\n", \
                       CUT_CASE_MAX_CNT);                 \
            break;                                        \
        }                                                 \
        while (c) {                                       \
            *(cut.clist + cut.ccnt_total++) = c;          \
            c = *(cut_suite_##sname + (++i));             \
        }                                                 \
    } while (0)


#define TRY                if (0 == setjmp(cut.jmpbuf))
#define EXCEPT             else
#define RAISE_EXCEPTION_WITH_MSG(msg)                          \
    do {                                                       \
        int ret = 0, i = cut.ccnt_fail;                        \
        cut.cerrmsg[i] = (char*)cut_malloc(CUT_MSG_MAX_LEN);   \
        assert(cut.cerrmsg[i] != NULL);                        \
        memset(cut.cerrmsg[i], 0, CUT_MSG_MAX_LEN);            \
        ret = cut_snprintf(cut.cerrmsg[i],                     \
                           CUT_MSG_MAX_LEN - 1,                \
                           "%s.%s in %s(%d) expected %s",      \
                           cut.ccur->sname, cut.ccur->cname,   \
                           __FILE__, __LINE__, msg);           \
        if (ret >= CUT_MSG_MAX_LEN)                            \
            cut_snprintf(cut.cerrmsg[i] + CUT_MSG_MAX_LEN - 4, \
                         4, "...");                            \
        longjmp(cut.jmpbuf, 1);                                \
    } while (0)

#define ASSERT_TRUE(cond)                       \
    do {                                        \
        if (!(cond))                            \
            RAISE_EXCEPTION_WITH_MSG("[True]"); \
    } while (0)

#define ASSERT_INT(expected, compare, actual)                                     \
    do {                                                                          \
        if (!((expected)compare(actual)))                                         \
            RAISE_EXCEPTION_WITH_MSG("[" #expected " " #compare " " #actual "]"); \
    } while (0)

#define ASSERT_STR(expected, compare, actual)                                     \
    do {                                                                          \
        if (!(strcmp((expected), (actual)) compare 0))                            \
            RAISE_EXCEPTION_WITH_MSG("[" #expected " " #compare " " #actual "]"); \
    } while (0)

#define ASSERT_IN(expected1, actual, expected2)                                       \
    do {                                                                              \
        if ((actual) < (expected1) || (actual) > (expected2))                         \
            RAISE_EXCEPTION_WITH_MSG("[" #expected1 " <= " #actual " <= " #expected2 "]"); \
    } while (0)

#define ASSERT_NSTR(expected, compare, actual, len)                                     \
    do {                                                                          \
        if (!(strncmp((expected), (actual), (len)) compare 0))                            \
            RAISE_EXCEPTION_WITH_MSG("[" #expected " " #compare " " #actual "]"); \
    } while (0)


#define ASSERT_FAIL()                               RAISE_EXCEPTION_WITH_MSG("[should not be here]")
#define ASSERT_FALSE(cond)                          ASSERT_TRUE(!(cond))
#define ASSERT_NULL(ptr)                            ASSERT_INT(ptr, ==, NULL)
#define ASSERT_NOT_NULL(ptr)                        ASSERT_INT(ptr, !=, NULL)
#define ASSERT_EQ(actual, expected)                 ASSERT_INT(actual, ==, expected)
#define ASSERT_NE(actual, expected)                 ASSERT_INT(actual, !=, expected)
#define ASSERT_GT(actual, expected)                 ASSERT_INT(actual,  >, expected)
#define ASSERT_GE(actual, expected)                 ASSERT_INT(actual, >=, expected)
#define ASSERT_LT(actual, expected)                 ASSERT_INT(actual,  <, expected)
#define ASSERT_LE(actual, expected)                 ASSERT_INT(actual, <=, expected)
#define ASSERT_STR_EQ(actual, expected)             ASSERT_STR(actual, ==, expected)
#define ASSERT_STR_NE(actual, expected)             ASSERT_STR(actual, !=, expected)
#define ASSERT_STR_GT(actual, expected)             ASSERT_STR(actual,  >, expected)
#define ASSERT_STR_LT(actual, expected)             ASSERT_STR(actual,  <, expected)
#define ASSERT_NSTR_EQ(actual, expected, len)       ASSERT_NSTR(actual, ==, expected, len)
#define ASSERT_NSTR_NE(actual, expected, len)       ASSERT_NSTR(actual, !=, expected, len)
#define ASSERT_NSTR_GT(actual, expected, len)       ASSERT_NSTR(actual,  >, expected, len)
#define ASSERT_NSTR_LT(actual, expected, len)       ASSERT_NSTR(actual,  <, expected, len)

/*
 * see http://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
 */
#define COL_DEF "\x1B[0m"
#define COL_RED "\x1B[1;31m"
#define COL_GRE "\x1B[1;32m"
#define COL_YEL "\x1B[1;33m"
#define COL_BLU "\x1B[1;34m"
#define COL_MAG "\x1B[1;35m"
#define COL_CYN "\x1B[1;36m"
#define COL_WHE "\x1B[1;37m"

#ifdef __cplusplus
}
#endif

#endif /* __CUT_H__ */

