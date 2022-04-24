#include "gpu.hpp"
#include "base.hpp"
#include "internal/utility.hpp"
#include "machine.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstring>
#include <fmt/format.h>

namespace drxl {

    Gpu::Gpu() : m_Mode{GpuMode::Text}, m_TextMode{} {}

    bool Gpu::Initialize() {
        m_Mode = GpuMode::Text;

        m_Palettes[0].m_Colors[0x0] = {0x00, 0x00, 0x00};
        m_Palettes[0].m_Colors[0x1] = {0x80, 0x40, 0x40};
        m_Palettes[0].m_Colors[0x2] = {0x80, 0x80, 0x40};
        m_Palettes[0].m_Colors[0x3] = {0x40, 0x80, 0x40};
        m_Palettes[0].m_Colors[0x4] = {0x40, 0x80, 0x80};
        m_Palettes[0].m_Colors[0x5] = {0x40, 0x40, 0x80};
        m_Palettes[0].m_Colors[0x6] = {0x80, 0x40, 0x80};
        m_Palettes[0].m_Colors[0x7] = {0xC0, 0xC0, 0xC0};
        m_Palettes[0].m_Colors[0x8] = {0x40, 0x40, 0x40};
        m_Palettes[0].m_Colors[0x9] = {0xf0, 0x80, 0x80};
        m_Palettes[0].m_Colors[0xA] = {0xf0, 0xf0, 0x80};
        m_Palettes[0].m_Colors[0xB] = {0x80, 0xf0, 0x80};
        m_Palettes[0].m_Colors[0xC] = {0x80, 0xf0, 0xf0};
        m_Palettes[0].m_Colors[0xD] = {0x80, 0x80, 0xf0};
        m_Palettes[0].m_Colors[0xE] = {0xf0, 0x80, 0xf0};
        m_Palettes[0].m_Colors[0xF] = {0xf0, 0xf0, 0xf0};

        if (!m_TextMode.Initialize()) { return false; }
        return true;
    }

    void Gpu::Render(Machine * machine) {
        static float xscale = static_cast<float>(machine->m_Win.getSize().x) / GpuPixelWidth;
        static float yscale = static_cast<float>(machine->m_Win.getSize().y) / GpuPixelHeight;
        switch (m_Mode) {
        case GpuMode::Text: m_TextMode.Render(machine); break;
        case GpuMode::Pixel_384x216x4_Indexed: m_PixelMode_384x216x4_Indexed.Render(machine); break;
        default: drxl_assert(false, fmt::format("Invalid GPU Mode: {}", static_cast<int>(m_Mode))); break;
        }
        machine->m_RTex.display();
        sf::Sprite rtexspr(machine->m_RTex.getTexture());
        rtexspr.setScale(xscale, yscale);
        machine->m_Win.draw(rtexspr);
    }

    GpuMode Gpu::GetMode() const { return m_Mode; }

    void Gpu::SetMode(GpuMode mode, Machine * machine) {
        switch (mode) {
        case GpuMode::Text:
            m_Mode = mode;
            m_TextMode.Initialize();
            break;
        case GpuMode::Pixel_384x216x4_Indexed:
            m_Mode = mode;
            m_PixelMode_384x216x4_Indexed.Initialize(machine);
            break;
        default: drxl_assert(false, fmt::format("Invalid GPU Mode: {}", static_cast<int>(mode))); break;
        }
    }

} // namespace drxl
