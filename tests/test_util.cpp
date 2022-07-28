#include "LT.h"

LT::Logger::ptr g_logger = LT_LOG_ROOT();

void test2() {
    std::cout << LT::BacktraceToString() << std::endl;
}
void test1() {
    test2();
}

void test_backtrace() {
    test1();
}

int main() {
    LT_LOG_INFO(g_logger) << LT::GetCurrentMS();
    LT_LOG_INFO(g_logger) << LT::GetCurrentUS();
    LT_LOG_INFO(g_logger) << LT::ToUpper("hello");
    LT_LOG_INFO(g_logger) << LT::ToLower("HELLO");
    LT_LOG_INFO(g_logger) << LT::Time2Str();
    LT_LOG_INFO(g_logger) << LT::Str2Time("1970-01-01 00:00:00"); // -28800

    std::vector<std::string> files;
    LT::FSUtil::ListAllFile(files, "./LT", ".cpp");
    for (auto &i : files) {
        LT_LOG_INFO(g_logger) << i;
    }

    // todo, more...

    test_backtrace();

    LT_ASSERT2(false, "assert");
    return 0;
}
