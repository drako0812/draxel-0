#include "base.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>

namespace pxly {

#if defined(__cpp_lib_source_location)
    void assert_(bool                                        condition,
                 std::optional<std::function<std::string()>> func,
                 const std::source_location                  location__) {
        if (!condition) {
            auto msg = func.value_or([]() { return "pxly::assert: Assertion failed!"; })();
            fmt::print(std::cerr,
                       "{} @\"{}\" #{}: {}\n",
                       location__.function_name(),
                       location__.file_name(),
                       location__.line(),
                       msg);
            std::exit(EXIT_FAILURE);
        }
    }
#else
    void assert_(bool condition, std::optional<std::function<std::string()>> func, const char * file__, int line__) {
        if (!condition) {
            auto msg = func.value_or([]() { return "pxly::assert: Assertion failed!"; })();
            fmt::print(std::cerr, "\"{}\" #{}: {}\n", file__, line__, msg);
            std::exit(EXIT_FAILURE);
        }
    }
#endif

} // namespace pxly
