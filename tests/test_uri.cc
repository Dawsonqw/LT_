#include "LT.h"
#include <iostream>

int main(int argc, char * argv[]) {
    auto uri = LT::Uri::Create("http://a:b@host.com:8080/p/a/t/h?query=string#hash");
    std::cout << uri->toString() << std::endl;
    return 0;
}
