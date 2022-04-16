#include "gpu.hpp"
#include "base.hpp"
#include "machine.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
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

    uint8 PixelBuffer_384x216x4_Indexed::GetPixel(int x, int y) const {
        int index        = x + (GpuPixelWidth * y);
        int byte_index   = index / 2;
        int nibble_index = index % 2;
        switch (nibble_index) {
        case 0: return (m_Buffer[index] & 0xF0) >> 4; break;
        case 1: return (m_Buffer[index] & 0x0F); break;
        default: return 0; break;
        }
    }

    void PixelBuffer_384x216x4_Indexed::SetPixel(int x, int y, uint8 color) {
        int index        = x + (GpuPixelWidth * y);
        int byte_index   = index / 2;
        int nibble_index = index % 2;
        switch (nibble_index) {
        case 0: m_Buffer[index] = (m_Buffer[index] & 0x0F) | (color << 4); break;
        case 1: m_Buffer[index] = (m_Buffer[index] & 0xF0) | color; break;
        default: break;
        }
    }

    PixelMode_384x216x4_Indexed::PixelMode_384x216x4_Indexed() {}

    bool PixelMode_384x216x4_Indexed::Initialize(Machine * machine) {
        m_CsrX   = 0;
        m_CsrY   = 0;
        m_CsrTex = -1;
        m_CsrOn  = true;
        memset(&m_PixelBuffer, 0, sizeof(m_PixelBuffer));

        auto currentsz = machine->m_IGpu.m_PixelBufferImage.getSize();
        auto neededsz  = sf::Vector2u(GpuPixelWidth, GpuPixelHeight);
        if (currentsz != neededsz) {
            machine->m_IGpu.m_PixelBufferImage.create(neededsz.x, neededsz.y);
            pxly_assert(machine->m_IGpu.m_PixelBufferTexture.create(neededsz.x, neededsz.y),
                        "Failed to create texture");
            pxly_assert(machine->m_IGpu.m_PixelBufferTexture.loadFromImage(machine->m_IGpu.m_PixelBufferImage),
                        "Failed to load image to texture");
        }
        return true;
    }

    void PixelMode_384x216x4_Indexed::Render(Machine * machine) {
        auto &     texture = machine->m_IGpu.m_PixelBufferTexture;
        sf::Sprite spr(texture);
        machine->m_RTex.draw(spr);
    }

    int16 PixelMode_384x216x4_Indexed::GetCursorX() const { return m_CsrX; }

    int16 PixelMode_384x216x4_Indexed::GetCursorY() const { return m_CsrY; }

    void PixelMode_384x216x4_Indexed::GetCursorPosition(int16 & x, int16 & y) const {
        x = m_CsrX;
        y = m_CsrY;
    }

    void PixelMode_384x216x4_Indexed::SetCursorX(int16 x) { m_CsrX = x; }

    void PixelMode_384x216x4_Indexed::SetCursorY(int16 y) { m_CsrY = y; }

    void PixelMode_384x216x4_Indexed::SetCursorPosition(int16 x, int16 y) {
        m_CsrX = x;
        m_CsrY = y;
    }

    bool PixelMode_384x216x4_Indexed::IsCursorEnabled() const { return m_CsrOn; }

    void PixelMode_384x216x4_Indexed::EnableCursor(bool enabled) { m_CsrOn = enabled; }

    int16 PixelMode_384x216x4_Indexed::GetCursorTexture() const { return m_CsrTex; }

    void PixelMode_384x216x4_Indexed::SetCursorTexture(uint16 id) { m_CsrTex = id; }

    void PixelMode_384x216x4_Indexed::uploadPixelsToImage(Machine * machine) {
        static std::shared_ptr<std::array<uint8, (GpuPixelWidth * GpuPixelHeight) * 3>> buffer =
          std::make_shared<std::array<uint8, (GpuPixelWidth * GpuPixelHeight) * 3>>();
        auto & image   = machine->m_IGpu.m_PixelBufferImage;
        auto & texture = machine->m_IGpu.m_PixelBufferTexture;

        for (int y = 0; y < GpuPixelHeight; y++) {
            for (int x = 0; x < GpuPixelWidth; x++) {
                image.setPixel(x, y, convert(machine->m_Gpu.m_Palettes->GetColor(m_PixelBuffer.GetPixel(x, y))));
            }
        }

        texture.loadFromImage(image);
    }

    void PixelMode_384x216x4_Indexed::Line(int x0, int y0, int x1, int y1, uint8 color) {
        // Implementation based on https://github.com/ssloy/tinyrenderer/wiki/Lesson-1:-Bresenham%E2%80%99s-Line-Drawing-Algorithm#timings-fifth-and-final-attempt
        auto steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        auto dx      = x1 - x0;
        auto dy      = y1 - y0;
        auto derror2 = std::abs(dy) * 2;
        auto error2  = 0;
        auto y       = y0;
        for (int x = x0; x <= x1; x++) {
            if (steep) {
                m_PixelBuffer.SetPixel(y, x, color);
            } else {
                m_PixelBuffer.SetPixel(x, y, color);
            }
            error2 += derror2;
            if (error2 > dx) {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx * 2;
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Tri(int x0, int y0, int x1, int y1, int x2, int y2, uint8 color) {
        Line(x0, y0, x1, y1, color);
        Line(x0, y0, x2, y2, color);
        Line(x1, y1, x2, y2, color);
    }

    inline sf::Vector3f cross(sf::Vector3f v1, sf::Vector3f v2) {
        return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    }

    inline sf::Vector3f barycentric(int x0, int y0, int x1, int y1, int x2, int y2, int Px, int Py) {
        auto u = cross({x2 - x0, x1 - x0, x0 - Px}, {y2 - y0, y1 - y0, y0 - Py});
        if (std::abs(u.z) < 1) { return {-1, 1, 1}; }
        return {1 - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
    }

    void PixelMode_384x216x4_Indexed::Trif(int x0, int y0, int x1, int y1, int x2, int y2, uint8 color) {
        // Note: Implementation based on https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling#the-method-i-adopt-for-my-code
        auto bboxmin = sf::Vector2i{GpuPixelWidth - 1, GpuPixelHeight - 1};
        auto bboxmax = sf::Vector2i{0, 0};
        auto clampv  = sf::Vector2i{GpuPixelWidth - 1, GpuPixelHeight - 1};

        bboxmin.x = std::max(0, std::min(bboxmin.x, x0));
        bboxmin.y = std::max(0, std::min(bboxmin.y, y0));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, x0));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, y0));

        bboxmin.x = std::max(0, std::min(bboxmin.x, x1));
        bboxmin.y = std::max(0, std::min(bboxmin.y, y1));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, x1));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, y1));

        bboxmin.x = std::max(0, std::min(bboxmin.x, x2));
        bboxmin.y = std::max(0, std::min(bboxmin.y, y2));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, x2));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, y2));

        sf::Vector2i P;

        for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
            for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
                auto bc_screen = barycentric(x0, y0, x1, y1, x2, y2, P.x, P.y);
                if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) { continue; }
                m_PixelBuffer.SetPixel(P.x, P.y, color);
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Clear(uint8 color) {
        for (int y = 0; y < GpuPixelHeight; y++) {
            for (int x = 0; x < GpuPixelWidth; x++) { m_PixelBuffer.SetPixel(x, y, color); }
        }
    }

    void PixelMode_384x216x4_Indexed::HLine(int x, int y, int w, uint8 color) {
        for (int xx = x, i = 0; i < w; xx++, i++) { m_PixelBuffer.SetPixel(xx, y, color); }
    }

    void PixelMode_384x216x4_Indexed::VLine(int x, int y, int h, uint8 color) {
        for (int yy = y, i = 0; i < h; yy++, i++) { m_PixelBuffer.SetPixel(x, yy, color); }
    }

    void PixelMode_384x216x4_Indexed::Rect(int x, int y, int w, int h, uint8 color) {
        HLine(x, y, w, color);
        HLine(x, y + h, w, color);
        VLine(x, y, h, color);
        VLine(x + w, y, h, color);
    }

    void PixelMode_384x216x4_Indexed::Rectf(int x, int y, int w, int h, uint8 color) {
        for (int yy = y, i = 0; i < h; yy++, i++) { HLine(x, yy, w, color); }
    }

    void PixelMode_384x216x4_Indexed::Circ(int x, int y, int r, uint8 color) {
        int          f   = 1 - r;
        sf::Vector2i ddF = {0, -2 * r};
        sf::Vector2i pos = {0, r};

        m_PixelBuffer.SetPixel(x, y + r, color);
        m_PixelBuffer.SetPixel(x, y - r, color);
        m_PixelBuffer.SetPixel(x + r, y, color);
        m_PixelBuffer.SetPixel(x - r, y, color);

        while (pos.x < pos.y) {
            if (f >= 0) {
                pos.y--;
                ddF.y += 2;
                f += ddF.y;
            }
            pos.x++;
            ddF.x += 2;
            f += ddF.x + 1;

            m_PixelBuffer.SetPixel(x + pos.x, y + pos.y, color);
            m_PixelBuffer.SetPixel(x - pos.x, y + pos.y, color);
            m_PixelBuffer.SetPixel(x + pos.x, y - pos.y, color);
            m_PixelBuffer.SetPixel(x - pos.x, y - pos.y, color);
            m_PixelBuffer.SetPixel(x + pos.y, y + pos.x, color);
            m_PixelBuffer.SetPixel(x - pos.y, y + pos.x, color);
            m_PixelBuffer.SetPixel(x + pos.y, y - pos.x, color);
            m_PixelBuffer.SetPixel(x - pos.y, y - pos.x, color);
        }
    }

    void PixelMode_384x216x4_Indexed::Circf(int x, int y, int r, uint8 color) {
        sf::Vector2i pos = {0, r};
        int          m   = 5 - 4 * r;

        while (pos.x <= pos.y) {
            sf::Vector2i p1 = {x - pos.y, y - pos.x};
            for (int xx = x - pos.y; xx <= x + pos.y; xx++) { m_PixelBuffer.SetPixel(p1.x++, p1.y, color); }
            p1 = {x - pos.y, y + pos.x};
            for (int xx = x - pos.y; xx <= x + pos.y; xx++) { m_PixelBuffer.SetPixel(p1.x++, p1.y, color); }

            if (m > 0) {
                p1 = {x - pos.x, y - pos.y};
                for (int xx = x - pos.x; xx <= x + pos.x; xx++) { m_PixelBuffer.SetPixel(p1.x++, p1.y, color); }
                p1 = {x - pos.x, y + pos.y};
                for (int xx = x - pos.x; xx <= x + pos.x; xx++) { m_PixelBuffer.SetPixel(p1.x++, p1.y, color); }

                pos.y--;
                m -= 8 * pos.y;
            }
            pos.x++;
            m += 8 * pos.x + 4;
        }
    }

    void PixelMode_384x216x4_Indexed::Spr(Machine * machine, int x, int y, int index) {
        const auto & tex = machine->m_Gpu.m_Textures_4bpp_Indexed.GetTexture(index);
        for (int dy = y, sy = 0; sy < GpuTextureHeight; dy++, sy++) {
            for (int dx = x, sx = 0; dx < GpuTextureWidth; dx++, sx++) {
                m_PixelBuffer.SetPixel(dx, dy, tex.GetPixel(sx, sy));
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Spr(Machine * machine, int x, int y, int index, int key) {
        const auto & tex = machine->m_Gpu.m_Textures_4bpp_Indexed.GetTexture(index);
        for (int dy = y, sy = 0; sy < GpuTextureHeight; dy++, sy++) {
            for (int dx = x, sx = 0; dx < GpuTextureWidth; dx++, sx++) {
                auto c = tex.GetPixel(sx, sy);
                if (c != key) { m_PixelBuffer.SetPixel(dx, dy, tex.GetPixel(sx, sy)); }
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Spr(Machine * machine, int x, int y, int index, int w, int h) {
        int page = index % GpuTexturesPerPage;
        index    = index / GpuTexturesPerPage;
        for (int sprh = 0; sprh < h; sprh++) {
            for (int sprw = 0; sprw < w; sprw++) {
                int base_x           = index % GpuTexturesPerPageLine;
                int base_y           = index / GpuTexturesPerPageLine;
                int base_xx          = (base_x + sprw) % GpuTexturesPerPageLine;
                int base_yy          = (base_y + sprh) % GpuTexturesPerPageColumn;
                int sub_sprite_index = base_xx + (base_yy * GpuTexturesPerPageLine);
                sub_sprite_index     = (page * GpuTexturesPerPage) + sub_sprite_index;

                Spr(machine, x + (sprw * GpuTextureWidth), y + (sprh * GpuTextureHeight), sub_sprite_index);
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Spr(Machine * machine, int x, int y, int index, int key, int w, int h) {
        int page = index % GpuTexturesPerPage;
        index    = index / GpuTexturesPerPage;
        for (int sprh = 0; sprh < h; sprh++) {
            for (int sprw = 0; sprw < w; sprw++) {
                int base_x           = index % GpuTexturesPerPageLine;
                int base_y           = index / GpuTexturesPerPageLine;
                int base_xx          = (base_x + sprw) % GpuTexturesPerPageLine;
                int base_yy          = (base_y + sprh) % GpuTexturesPerPageColumn;
                int sub_sprite_index = base_xx + (base_yy * GpuTexturesPerPageLine);
                sub_sprite_index     = (page * GpuTexturesPerPage) + sub_sprite_index;

                Spr(machine, x + (sprw * GpuTextureWidth), y + (sprh * GpuTextureHeight), sub_sprite_index, key);
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Spr(Machine * machine, int x, int y, int index, int sw, int sh, int dw, int dh) {
        
    }

    void PixelMode_384x216x4_Indexed::Spr(
      Machine * machine, int x, int y, int index, int key, int sw, int sh, int dw, int dh) {}

    void PixelMode_384x216x4_Indexed::Spr_Scaled(Machine * machine, int x, int y, int index, int w, int h) {
        const auto & tex = machine->m_Gpu.m_Textures_4bpp_Indexed.GetTexture(index);
        for (int dy = y, sy = 0; sy < GpuTextureHeight; dy++, sy++) {
            for (int dx = x, sx = 0; dx < GpuTextureWidth; dx++, sx++) {
                for (int ys = 0; ys < h; ys++) {
                    for (int xs = 0; xs < w; xs++) { m_PixelBuffer.SetPixel(dx + xs, dy + ys, tex.GetPixel(sx, sy)); }
                }
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Spr_Scaled(Machine * machine, int x, int y, int index, int key, int w, int h) {
        const auto & tex = machine->m_Gpu.m_Textures_4bpp_Indexed.GetTexture(index);
        for (int dy = y, sy = 0; sy < GpuTextureHeight; dy++, sy++) {
            for (int dx = x, sx = 0; dx < GpuTextureWidth; dx++, sx++) {
                for (int ys = 0; ys < h; ys++) {
                    for (int xs = 0; xs < w; xs++) {
                        auto c = tex.GetPixel(sx, sy);
                        if (c != key) { m_PixelBuffer.SetPixel(dx + xs, dy + ys, tex.GetPixel(sx, sy)); }
                    }
                }
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Textri(Machine * machine,
                                             int       dx0,
                                             int       dy0,
                                             int       dx1,
                                             int       dy1,
                                             int       dx2,
                                             int       dy2,
                                             int       sx0,
                                             int       sy0,
                                             int       sx1,
                                             int       sy1,
                                             int       sx2,
                                             int       sy2) {}

    void PixelMode_384x216x4_Indexed::Textri(Machine * machine,
                                             int       dx0,
                                             int       dy0,
                                             int       dx1,
                                             int       dy1,
                                             int       dx2,
                                             int       dy2,
                                             int       sx0,
                                             int       sy0,
                                             int       sx1,
                                             int       sy1,
                                             int       sx2,
                                             int       sy2,
                                             int       key) {}

    const PixelBuffer_384x216x4_Indexed & PixelMode_384x216x4_Indexed::GetPixelBuffer() const { return m_PixelBuffer; }

    PixelBuffer_384x216x4_Indexed & PixelMode_384x216x4_Indexed::GetPixelBuffer() { return m_PixelBuffer; }

    Color Palette::GetColor(uint8 index) const { return m_Colors[index]; }

    void Palette::SetColor(uint8 index, Color color) { m_Colors[index] = color; }

    uint8 Texture_4bpp_Indexed::GetPixel(int8 x, int8 y) const {
        int index        = x + (GpuTextureWidth * y);
        int byte_index   = index / 2;
        int nibble_index = index % 2;
        switch (nibble_index) {
        case 0: return (m_Buffer[index] & 0xF0) >> 4; break;
        case 1: return (m_Buffer[index] & 0x0F); break;
        default: return 0; break;
        }
    }

    void Texture_4bpp_Indexed::SetPixel(int8 x, int8 y, uint8 color) {
        int index        = x + (GpuTextureWidth * y);
        int byte_index   = index / 2;
        int nibble_index = index % 2;
        switch (nibble_index) {
        case 0: m_Buffer[index] = (m_Buffer[index] & 0x0F) | (color << 4); break;
        case 1: m_Buffer[index] = (m_Buffer[index] & 0xF0) | color; break;
        default: break;
        }
    }

    uint8 Textures_4bpp_Indexed::GetGlobalPixel(int x, int y) const {blah}

    uint8 Textures_4bpp_Indexed::GetGlobalPixel(int start, int x, int y) const {
        blah
    }

    const Texture_4bpp_Indexed & Textures_4bpp_Indexed::GetTexture(uint16 id) const {
        const auto max_index = GpuTexturesPerPage * GpuTexturePages;
        pxly_assert(id < max_index, fmt::format("Texture ID {} out of range. Must be less than {}", id, max_index));
    }

    Texture_4bpp_Indexed & Textures_4bpp_Indexed::GetTexture(uint16 id) {
        const auto max_index = GpuTexturesPerPage * GpuTexturePages;
        pxly_assert(id < max_index, fmt::format("Texture ID {} out of range. Must be less than {}", id, max_index));
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
        static float xscale = static_cast<float>(machine->m_Win.getSize().x) / GpuPixelWidth;
        static float yscale = static_cast<float>(machine->m_Win.getSize().y) / GpuPixelHeight;
        switch (m_Mode) {
        case GpuMode::Text: m_TextMode.Render(machine); break;
        case GpuMode::Pixel_384x216x4_Indexed: m_PixelMode_384x216x4_Indexed.Render(machine); break;
        default: pxly_assert(false, fmt::format("Invalid GPU Mode: {}", static_cast<int>(m_Mode))); break;
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
        default: pxly_assert(false, fmt::format("Invalid GPU Mode: {}", static_cast<int>(mode))); break;
        }
    }

} // namespace pxly
