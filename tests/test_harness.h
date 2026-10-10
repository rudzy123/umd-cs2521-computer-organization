#ifndef CS2521_TEST_HARNESS_H
#define CS2521_TEST_HARNESS_H

#include <stdio.h>

/*
 * One inclusion per test program. EXPECT keeps going after a failure so the
 * run reports every broken check instead of stopping at the first.
 */

static int g_checks_failed = 0;

#define EXPECT(cond)                                                         \
    do {                                                                     \
        if (!(cond)) {                                                       \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++g_checks_failed;                                               \
        }                                                                    \
    } while (0)

#define EXPECT_LL(got, want)                                                        \
    do {                                                                            \
        long long expect_ll_got_ = (long long)(got);                               \
        long long expect_ll_want_ = (long long)(want);                             \
        if (expect_ll_got_ != expect_ll_want_) {                                   \
            fprintf(stderr, "FAIL %s:%d: got %lld want %lld\n", __FILE__, __LINE__, \
                    expect_ll_got_, expect_ll_want_);                               \
            ++g_checks_failed;                                                      \
        }                                                                           \
    } while (0)

#define EXPECT_ULL(got, want)                                                            \
    do {                                                                                 \
        unsigned long long expect_ull_got_ = (unsigned long long)(got);                 \
        unsigned long long expect_ull_want_ = (unsigned long long)(want);               \
        if (expect_ull_got_ != expect_ull_want_) {                                      \
            fprintf(stderr, "FAIL %s:%d: got %llu want %llu\n", __FILE__, __LINE__,     \
                    expect_ull_got_, expect_ull_want_);                                  \
            ++g_checks_failed;                                                           \
        }                                                                                \
    } while (0)

static int test_finish(void) {
    if (g_checks_failed != 0) {
        fprintf(stderr, "%d check(s) failed\n", g_checks_failed);
        return 1;
    }
    printf("ok\n");
    return 0;
}

#endif
