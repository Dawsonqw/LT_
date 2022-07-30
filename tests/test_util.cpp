#include "LT.h"

auto g_logger=std::make_shared<spdlog::logger>("gLog", g_sink);

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

    std::vector<std::string> files;
    for (auto &i : files) {
    }

    test_backtrace();

    return 0;
}
