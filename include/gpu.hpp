#pragma once

#include "base.hpp"
#include <cstdint>

namespace pxly {
    struct Machine;

    struct TextChar {
        uint8 fg : 4;
        uint8 bg : 4;
        char  ch;
    };

    struct TextMode {
        int16    m_CsrX;
        int16    m_CsrY;
        char     m_CsrCh;
        bool     m_CsrOn : 1;
        uint8    m_CsrCol : 7;
        TextChar m_Buffer[GpuTextBufferCount];

        TextMode();
        bool     Initialize();
        void     SetCursorX(int16 x);
        void     SetCursorY(int16 y);
        void     SetCursorPosition(int16 x, int16 y);
        void     EnableCursor(bool enable);
        void     SetCursorColor(uint8 color);
        void     SetCursorChar(char ch);
        void     PutChar(int16 x, int16 y, TextChar ch);
        void     PutString(int16 x, int16 y, uint8 fg, uint8 bg, const std::string & str);
        void     ScrollUp(int16 lines, TextChar fill_ch);
        void     ScrollDown(int16 lines, TextChar fill_ch);
        void     ScrollLeft(int16 cols, TextChar fill_ch);
        void     ScrollRight(int16 cols, TextChar fill_ch);
        void     ClearScreen(TextChar fill_ch);
        TextChar GetChar(int16 x, int16 y) const;
        void     Render(Machine * machine);
        int16    GetCursorX() const;
        int16    GetCursorY() const;
        void     GetCursorPosition(int16 & x, int16 & y) const;
        bool     IsCursorEnabled() const;
        uint8    GetCursorColor() const;
        char     GetCursorChar() const;
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

        Gpu();
        bool Initialize();
        void Render(Machine * machine);
    };

} // namespace pxly
