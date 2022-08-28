#ifndef __LT_MACRO_H__
#define __LT_MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"
#include "log.h"

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#define LT_LIKELY(x) __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#define LT_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define LT_LIKELY(x) (x)
#define LT_UNLIKELY(x) (x)
#endif

static lsinks csink{g_sink,c_sink};
static auto c_logger = std::make_shared<spdlog::logger>("marco",csink);

/*
/// 断言宏封装
#define LT_ASSERT(x)                                                               \
    if (LT_UNLIKELY(!(x))) {                                                        \
     //   g_logger->error("ASSERTON:{} \n backtrace:\n{}"                              \ 
	//			,x, LT::BacktraceToString(100, 2, "    "));                           \
        assert(x);                                                                     \
    }

/// 断言宏封装
#define LT_ASSERT2(x, w)                                                            \
    if (LT_UNLIKELY(!(x))) {                                                        \
      //  g_logger->error("ASSERTON:{} \nbacktrace:\n{}",x, LT::BacktraceToString(100, 2, "    "));\
        assert(x);                                                                     \

        //c_logger->error("Assertion: {} \n backtrace: {}  \n",x,LT::BacktraceToString(100,2,""));              \
    }
*/

#define LT_ASSERT(x)                                                    \
    if (LT_UNLIKELY(!(x))) {                                              \
        c_logger->error("Assertion: {} \n backtrace: {}  \n",x,LT::BacktraceToString(100,2,""));              \
        assert(x);                                                            \
    }

/// 断言宏封装
#define LT_ASSERT2(x, w)                                                            \
    if (LT_UNLIKELY(!(x))) {                                                        \
        c_logger->error("Assertion: {} \n backtrace: {}  \n",x,LT::BacktraceToString(100,2,""));              \
        assert(x);                                                                  \
    }












#endif
