#include "base.hpp"
#include "gpu.hpp"
#include "internal/utility.hpp"
#include "machine.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstring>
#include <fmt/format.h>

namespace drxl {

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

    sf::Color convert(const Color & color) { return sf::Color(color.r, color.g, color.b, 255); }

    Color convert(const sf::Color & color) { return Color{.r = color.r, .g = color.g, .b = color.b}; }

    void TextMode::Render(Machine * machine) {
        sf::Sprite charSprite;
        charSprite.setTexture(machine->m_Font.getTexture(8), true);
        sf::RectangleShape rectShape({GpuTextureWidth, GpuTextureHeight});

        for (int y = 0; y < GpuTextCellsHeight; y++) {
            for (int x = 0; x < GpuTextCellsWidth; x++) {
                auto ch    = GetChar(x, y);
                int  xx    = x * GpuTextureWidth;
                int  yy    = y * GpuTextureHeight;
                auto glyph = machine->m_Font.getGlyph(ch.ch, 8, false);
                //charSprite.setOrigin(0, 0);
                charSprite.setPosition(xx + glyph.bounds.left, yy + (8 + glyph.bounds.top));
                charSprite.setTextureRect(glyph.textureRect);
                charSprite.setColor(convert(machine->m_Gpu.m_Palettes[0].m_Colors[ch.fg]));
                rectShape.setPosition(xx, yy);
                rectShape.setFillColor(convert(machine->m_Gpu.m_Palettes[0].m_Colors[ch.bg]));
                machine->m_RTex.draw(rectShape);
                machine->m_RTex.draw(charSprite);
            }
        }

        if (IsCursorEnabled()) {
            auto glyph = machine->m_Font.getGlyph(GetCursorChar(), 8, false);
            //charSprite.setOrigin(0, -2);
            charSprite.setPosition((GetCursorX() * GpuTextureWidth) + glyph.bounds.left,
                                   (GetCursorY() * GpuTextureHeight) + (8 + glyph.bounds.top));
            charSprite.setTextureRect(glyph.textureRect);
            charSprite.setColor(convert(machine->m_Gpu.m_Palettes[0].m_Colors[GetCursorColor()]));
            machine->m_RTex.draw(charSprite);
        }
    }

} // namespace drxl
