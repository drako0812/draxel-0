#include "base.h"
#include "base_intern.hpp"
#include <cstdlib>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>

void pxly_assert(bool condition, std::optional<std::function<const std::string()>> func) {
    if (!condition) {
        auto msg = func.value_or("pxly_assert: Assertion failed!")();
        fmt::print(std::cerr, "{}\n", msg);
        std::exit(EXIT_FAILURE);
    }
}
