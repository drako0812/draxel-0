#pragma once

#include "base.hpp"
#include "gpu/pixmode1.hpp"
#include "gpu/textmode.hpp"
#include <SFML/Graphics.hpp>
#include <cstdint>

namespace drxl {
    struct Machine;

    struct Color {
        uint8 r, g, b;
    };

    enum class GpuMode : uint8 {
        Text,
        Pixel_384x216x4_Indexed,
        Pixel_256x144x8_Indexed,
        Pixel_256x144x8_TrueColor,
        Pixel_192x108x16_TrueColor,
        Pixel_144x81x24_TrueColor,
    };

    struct Palette {
        Color m_Colors[GpuPaletteSize];

        Color GetColor(uint8 index) const;
        void  SetColor(uint8 index, Color color);
    };

    struct Gpu {
        GpuMode m_Mode;
        union {
            TextMode                    m_TextMode;
            PixelMode_384x216x4_Indexed m_PixelMode_384x216x4_Indexed;
        };
        union {
            Textures_4bpp_Indexed m_Textures_4bpp_Indexed;
        };

        Palette m_Palettes[GpuPaletteCount];

        Gpu();
        bool    Initialize();
        void    Render(Machine * machine);
        GpuMode GetMode() const;
        void    SetMode(GpuMode mode, Machine * machine);
    };

    struct InternalGpu {
        sf::Image   m_PixelBufferImage;
        sf::Texture m_PixelBufferTexture;
    };

} // namespace drxl
