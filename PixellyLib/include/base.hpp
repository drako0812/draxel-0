#pragma once

#include <functional>
#include <optional>
#include <string>
#include <version>

#if defined(__cpp_lib_source_location)
#    include <source_location>
#endif

#if defined(PXLY_SHARED)
#    if defined(WINDOWS)
#        if defined(PXLY_EXPORT)
#            define PXLY_API __declspec(dllexport)
#        else
#            define PXLY_API __declspec(dllimport)
#        endif
#    else
#        define PXLY_API
#    endif
#else
#    define PXLY_API
#endif

namespace pxly {

    using uint8 = std::uint8_t;
    using int16 = std::int16_t;

#if defined(__cpp_lib_source_location)
    void assert_(bool condition, std::optional<std::function<std::string()>> func = std::nullopt, const std::source_location location__ = std::source_location::current());
    #define pxly_assert(__condition__, __message__) pxly::assert_(__condition__, [&]() { return __message__; })
#else
    void assert_(bool condition, std::optional<std::function<std::string()>> func, const char * file__, int line__);
    #define pxly_assert(__condition__, __message__) pxly::assert_(__condition__, [&]() { return __message__; }, (__FILE__), (__LINE__))
#endif

    constexpr int GpuPixelWidth = 384;
    constexpr int GpuPixelHeight = 216;
    constexpr int GpuTextCellsWidth = 48;
    constexpr int GpuTextCellsHeight = 27;
    constexpr int GpuTextBufferCount = GpuTextCellsWidth * GpuTextCellsHeight;
    constexpr int GpuTextBufferBytes = GpuTextBufferCount * 2;
    constexpr int GpuPixelBufferCount = GpuPixelWidth * GpuPixelHeight;
    constexpr int GpuPixelSize = 4;
    constexpr int GpuPixelBufferBytes = ((GpuPixelBufferCount * GpuPixelSize) / 8);
    constexpr int GpuPixelsToBytes(int pixels) { return (pixels * GpuPixelSize) / 8; }
    constexpr int GpuTextureWidth = 8;
    constexpr int GpuTextureHeight = 8;
    constexpr int GpuTexturePages = 16;
    constexpr int GpuTexturesPerPage = 256;
    constexpr int GpuPaletteSize = 16;
    constexpr int GpuPaletteCount = 1;

} // namespace pxly
