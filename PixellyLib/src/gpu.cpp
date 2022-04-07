#include "gpu.hpp"
#include "machine.hpp"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <cstring>
#include <fmt/format.h>

namespace pxly {

    TextMode::TextMode() : m_CsrX{0}, m_CsrY{0}, m_CsrCh{'_'}, m_CsrOn{true}, m_CsrCol{15} {
        memset(m_Buffer, 0, sizeof(TextChar) * GpuTextBufferCount);
    }

    bool TextMode::Initialize() {
        m_CsrX   = 0;
        m_CsrY   = 0;
        m_CsrCh  = '_';
        m_CsrOn  = true;
        m_CsrCol = 15;
        memset(m_Buffer, 0, sizeof(TextChar) * GpuTextBufferCount);
        return true;
    }

    void TextMode::SetCursorX(int16 x) { m_CsrX = x; }

    void TextMode::SetCursorY(int16 y) { m_CsrY = y; }

    void TextMode::SetCursorPosition(int16 x, int16 y) {
        m_CsrX = x;
        m_CsrY = y;
    }

    void TextMode::EnableCursor(bool enable) { m_CsrOn = enable; }

    void TextMode::SetCursorColor(uint8 color) { m_CsrCol = color; }

    void TextMode::SetCursorChar(char ch) { m_CsrCh = ch; }

    void TextMode::PutChar(int16 x, int16 y, TextChar ch) {
        if ((x < 0) || (x >= GpuTextCellsWidth) || (y < 0) || (y >= GpuTextCellsHeight)) { return; }
        int16 index     = x + (y * GpuTextCellsWidth);
        m_Buffer[index] = ch;
    }

    void TextMode::PutString(int16 x, int16 y, uint8 fg, uint8 bg, const std::string & str) {
        auto len = str.length();
        for (int i = 0; i < len; i++) {
            auto ch = TextChar{fg, bg, str[i]};
            PutChar(x + i, y, ch);
        }
    }

    void TextMode::ScrollUp(int16 lines, TextChar fill_ch) {
        if (lines >= GpuTextCellsHeight) {
            ClearScreen(fill_ch);
            return;
        }
        for (int y = 0; y < GpuTextCellsHeight - lines; y++) {
            for (int x = 0; x < GpuTextCellsWidth; x++) {
                int  src_y = y + lines;
                auto ch    = GetChar(x, src_y);
                PutChar(x, y, ch);
            }
        }

        for (int y = GpuTextCellsHeight - lines; y < GpuTextCellsHeight; y++) {
            for (int x = 0; x < GpuTextCellsWidth; x++) { PutChar(x, y, fill_ch); }
        }
    }

    void TextMode::ScrollDown(int16 lines, TextChar fill_ch) {
        if (lines >= GpuTextCellsHeight) {
            ClearScreen(fill_ch);
            return;
        }
        for (int y = GpuTextCellsHeight - lines; y < GpuTextCellsHeight; y++) {
            for (int x = 0; x < GpuTextCellsWidth; x++) {
                int  src_y = y - lines;
                auto ch    = GetChar(x, src_y);
                PutChar(x, y, ch);
            }
        }

        for (int y = 0; y < GpuTextCellsHeight - lines; y++) {
            for (int x = 0; x < GpuTextCellsWidth; x++) { PutChar(x, y, fill_ch); }
        }
    }

    void TextMode::ScrollLeft(int16 cols, TextChar fill_ch) {
        if (cols >= GpuTextCellsWidth) {
            ClearScreen(fill_ch);
            return;
        }
        for (int y = 0; y < GpuTextCellsHeight; y++) {
            for (int x = 0; x < GpuTextCellsWidth - cols; x++) {
                int  src_x = x + cols;
                auto ch    = GetChar(src_x, y);
                PutChar(x, y, ch);
            }
        }

        for (int y = 0; y < GpuTextCellsHeight; y++) {
            for (int x = GpuTextCellsWidth - cols; x < GpuTextCellsWidth; x++) { PutChar(x, y, fill_ch); }
        }
    }

    void TextMode::ScrollRight(int16 cols, TextChar fill_ch) {
        if (cols >= GpuTextCellsWidth) {
            ClearScreen(fill_ch);
            return;
        }
        for (int y = 0; y < GpuTextCellsHeight; y++) {
            for (int x = GpuTextCellsWidth - cols; x < GpuTextCellsWidth; x++) {
                int  src_x = x - cols;
                auto ch    = GetChar(src_x, y);
                PutChar(x, y, ch);
            }
        }

        for (int y = 0; y < GpuTextCellsHeight; y++) {
            for (int x = 0; x < GpuTextCellsWidth - cols; x++) { PutChar(x, y, fill_ch); }
        }
    }

    void TextMode::ClearScreen(TextChar fill_ch) {
        for (int y = 0; y < GpuTextCellsHeight; y++) {
            for (int x = 0; x < GpuTextCellsWidth; x++) { PutChar(x, y, fill_ch); }
        }
    }

    TextChar TextMode::GetChar(int16 x, int16 y) const {
        if ((x < 0) || (x >= GpuTextCellsWidth) || (y < 0) || (y >= GpuTextCellsHeight)) { return {0, 0, '\0'}; }
        int16 index = x + (y * GpuTextCellsWidth);
        return m_Buffer[index];
    }

    int16 TextMode::GetCursorX() const { return m_CsrX; }

    int16 TextMode::GetCursorY() const { return m_CsrY; }

    void TextMode::GetCursorPosition(int16 & x, int16 & y) const {
        x = m_CsrX;
        y = m_CsrY;
    }

    bool TextMode::IsCursorEnabled() const { return m_CsrOn; }

    uint8 TextMode::GetCursorColor() const { return m_CsrCol; }

    char TextMode::GetCursorChar() const { return m_CsrCh; }

    ALLEGRO_COLOR convert(const Color & color) { return al_map_rgb(color.r, color.g, color.b); };

    Color convert(const ALLEGRO_COLOR & color) {
        Color col;
        al_unmap_rgb(color, &col.r, &col.g, &col.b);
        return col;
    }

    void TextMode::Render(Machine * machine) {
        al_hold_bitmap_drawing(true);
        for (int y = 0; y < GpuTextCellsHeight; y++) {
            for (int x = 0; x < GpuTextCellsWidth; x++) {
                auto ch = GetChar(x, y);
                int  xx = x * GpuTextureWidth;
                int  yy = y * GpuTextureHeight;
                al_draw_filled_rectangle(xx,
                                         yy,
                                         xx + GpuTextureWidth,
                                         yy + GpuTextureHeight,
                                         convert(machine->m_Gpu.m_Palettes[0].m_Colors[ch.bg]));
                al_draw_glyph(machine->m_Font, convert(machine->m_Gpu.m_Palettes[0].m_Colors[ch.fg]), xx, yy, ch.ch);
            }
        }
        if (IsCursorEnabled()) {
            al_draw_glyph(machine->m_Font,
                          convert(machine->m_Gpu.m_Palettes[0].m_Colors[GetCursorColor()]),
                          GetCursorX() * GpuTextureWidth,
                          GetCursorY() * GpuTextureHeight,
                          GetCursorChar());
        }
        al_hold_bitmap_drawing(false);
    }

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
        al_set_target_bitmap(machine->m_RTex);
        switch (m_Mode) {
        case GpuMode::Text: m_TextMode.Render(machine); break;
        default: pxly_assert(false, fmt::format("Invalid GPU Mode: {}", static_cast<int>(m_Mode))); break;
        }
        al_set_target_backbuffer(machine->m_Disp);
        al_draw_bitmap_region(machine->m_RTex, 0, 0, GpuPixelWidth, GpuPixelHeight, 0, 0, 0);
    }

} // namespace pxly
