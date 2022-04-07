#pragma once

#include "base.hpp"
#include <cstdint>

namespace pxly {
    struct Machine;

    struct TextChar {
        uint8 fg : 4;
        uint8 bg : 4;
        char ch;
    };

    struct TextMode {
        int16 m_CsrX;
        int16 m_CsrY;
        char m_CsrCh;
        bool m_CsrOn : 1;
        uint8 m_CsrCol : 7;
        TextChar m_Buffer[GpuTextBufferCount];

        PXLY_API TextMode();
        PXLY_API bool Initialize();
        PXLY_API void SetCursorX(int16 x);
        PXLY_API void SetCursorY(int16 y);
        PXLY_API void SetCursorPosition(int16 x, int16 y);
        PXLY_API void EnableCursor(bool enable);
        PXLY_API void SetCursorColor(uint8 color);
        PXLY_API void SetCursorChar(char ch);
        PXLY_API void PutChar(int16 x, int16 y, TextChar ch);
        PXLY_API void PutString(int16 x, int16 y, uint8 fg, uint8 bg, const std::string & str);
        PXLY_API void ScrollUp(int16 lines, TextChar fill_ch);
        PXLY_API void ScrollDown(int16 lines, TextChar fill_ch);
        PXLY_API void ScrollLeft(int16 cols, TextChar fill_ch);
        PXLY_API void ScrollRight(int16 cols, TextChar fill_ch);
        PXLY_API void ClearScreen(TextChar fill_ch);
        PXLY_API TextChar GetChar(int16 x, int16 y) const;
        PXLY_API void Render(Machine * machine);
        PXLY_API int16 GetCursorX() const;
        PXLY_API int16 GetCursorY() const;
        PXLY_API void GetCursorPosition(int16 & x, int16 & y) const;
        PXLY_API bool IsCursorEnabled() const;
        PXLY_API uint8 GetCursorColor() const;
        PXLY_API char GetCursorChar() const;
    };

    enum class GpuMode : uint8 {
        Text,
        Pixel_384x216x4_Indexed,
        Pixel_256x144x8_Indexed,
        Pixel_256x144x8_TrueColor,
        Pixel_192x108x16_TrueColor,
        Pixel_144x81x24_TrueColor,
    };

    struct Color {
        uint8 r, g, b;
    };

    struct Palette {
        Color m_Colors[GpuPaletteSize];
    };

    struct Gpu {
        GpuMode m_Mode;
        union {
            TextMode m_TextMode;
        };
        // Texture m_Textures[GpuTexturesPerPage * GpuTexturePages];

        Palette m_Palettes[GpuPaletteCount];

        PXLY_API Gpu();
        PXLY_API bool Initialize();
        PXLY_API void Render(Machine * machine);
    };

}
