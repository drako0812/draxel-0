#pragma once

#include <boost/predef.h>
#include <cstdlib>
#include <functional>
#include <gcem.hpp>
#include <optional>
#include <string>
#include <version>

#if defined(__cpp_lib_source_location)
#    include <source_location>
#endif

namespace pxly {

    inline std::string get_home_data_path() {
        std::string base_home;
#if BOOST_OS_WINDOWS
#    if BOOST_PLAT_MINGW || BOOST_PLAT_MINGW32 || BOOST_PLAT_MINGW64
        base_home = std::getenv("HOME");
        base_home += "/.Pixelly";
#    elif BOOST_PLAT_WINDOWS_DESKTOP
        base_home = std::getenv("HOMEDRIVE");
        base_home += std::getenv("HOMEPATH");
        base_home += "\\AppData\\Local\\Pixelly";
#    else
#        error "This specific Windows platform is currently unsupported"
#    endif
#elif BOOST_OS_CYGWIN
        base_home = std::getenv("HOME");
        base_home += "/.Pixelly";
#elif BOOST_OS_LINUX
        base_home = std::getenv("HOME");
        base_home += "/.Pixelly";
#else
#    error "Platform not currently supported"
#endif
        return base_home;
    }

    using uint8  = std::uint8_t;
    using uint16 = std::uint16_t;
    using int8   = std::int8_t;
    using int16  = std::int16_t;

#if defined(__cpp_lib_source_location)
    void assert_(bool                                        condition,
                 std::optional<std::function<std::string()>> func       = std::nullopt,
                 const std::source_location                  location__ = std::source_location::current());
#    define pxly_assert(__condition__, __message__) pxly::assert_(__condition__, [&]() { return __message__; })
#else
    void assert_(bool condition, std::optional<std::function<std::string()>> func, const char * file__, int line__);
#    define pxly_assert(__condition__, __message__) \
        pxly::assert_(                              \
          __condition__, [&]() { return __message__; }, (__FILE__), (__LINE__))
#endif

    constexpr int GpuPixelWidth       = 384;
    constexpr int GpuPixelHeight      = 216;
    constexpr int GpuTextCellsWidth   = 48;
    constexpr int GpuTextCellsHeight  = 27;
    constexpr int GpuTextBufferCount  = GpuTextCellsWidth * GpuTextCellsHeight;
    constexpr int GpuTextBufferBytes  = GpuTextBufferCount * 2;
    constexpr int GpuPixelBufferCount = GpuPixelWidth * GpuPixelHeight;
    constexpr int GpuPixelSize        = 4;
    constexpr int GpuPixelBufferBytes = ((GpuPixelBufferCount * GpuPixelSize) / 8);
    constexpr int GpuPixelsToBytes(int pixels, int bits_per_pixel) { return (pixels * bits_per_pixel) / 8; }
    constexpr int GpuTextureWidth    = 8;
    constexpr int GpuTextureHeight   = 8;
    constexpr int GpuTexturePages    = 16;
    constexpr int GpuTexturesPerPage = 256;
    constexpr int GpuTotalTextures   = GpuTexturesPerPage * GpuTexturePages;
    constexpr int GpuPaletteSize     = 16;
    constexpr int GpuPaletteCount    = 1;
    constexpr int GpuGlobalTextureWidth =
      gcem::sqrt(GpuTexturePages) * gcem::sqrt(GpuTexturesPerPage) * GpuTextureWidth;
    constexpr int GpuGlobalTextureHeight = GpuGlobalTextureWidth;
    constexpr int GpuTexturePagePixelWidth = gcem::sqrt(GpuTexturesPerPage) * GpuTextureWidth;
    constexpr int GpuTexturePagePixelHeight = gcem::sqrt(GpuTexturesPerPage) * GpuTextureHeight;
    constexpr int GpuTexturesPerPageLine = gcem::sqrt(GpuTexturesPerPage);
    constexpr int GpuTexturesPerPageColumn = GpuTexturesPerPageLine;

} // namespace pxly
