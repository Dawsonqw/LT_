#ifndef __LT_MACRO_H__
#define __LT_MACRO_H__

#include <string.h>
#include <assert.h>
#include "log.h"
#include "util.h"

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#define LT_LIKELY(x) __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#define LT_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LT_LIKELY(x) (x)
#define LT_UNLIKELY(x) (x)
#endif

/// 断言宏封装
#define LT_ASSERT(x)                                                                \
    if (LT_UNLIKELY(!(x))) {                                                        \
        LT_LOG_ERROR(LT_LOG_ROOT()) << "ASSERTION: " #x                          \
                                          << "\nbacktrace:\n"                          \
                                          << LT::BacktraceToString(100, 2, "    "); \
        assert(x);                                                                     \
    }

/// 断言宏封装
#define LT_ASSERT2(x, w)                                                            \
    if (LT_UNLIKELY(!(x))) {                                                        \
        LT_LOG_ERROR(LT_LOG_ROOT()) << "ASSERTION: " #x                          \
                                          << "\n"                                      \
                                          << w                                         \
                                          << "\nbacktrace:\n"                          \
                                          << LT::BacktraceToString(100, 2, "    "); \
        assert(x);                                                                     \
    }

#endif
