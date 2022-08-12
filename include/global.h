#ifndef GLOBAL_H
#define GLOBAL_H
#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/common.h"

    using sptr = std::shared_ptr<spdlog::sinks::sink>;
    using lsinks = std::initializer_list<sptr>;

#endif
