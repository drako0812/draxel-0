#include "gpu.hpp"
#include "internal/utility.hpp"
#include "machine.hpp"

namespace drxl {

    inline sf::Vector3f cross(sf::Vector3f v1, sf::Vector3f v2) {
        return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    }

    inline sf::Vector3f barycentric(int x0, int y0, int x1, int y1, int x2, int y2, int Px, int Py) {
        auto u = cross({static_cast<float>(x2) - x0, static_cast<float>(x1) - x0, static_cast<float>(x0) - Px},
                       {static_cast<float>(y2) - y0, static_cast<float>(y1) - y0, static_cast<float>(y0) - Py});
        if (std::abs(u.z) < 1) { return {-1, 1, 1}; }
        return {1 - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z};
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
            drxl_assert(machine->m_IGpu.m_PixelBufferTexture.create(neededsz.x, neededsz.y),
                        "Failed to create texture");
            drxl_assert(machine->m_IGpu.m_PixelBufferTexture.loadFromImage(machine->m_IGpu.m_PixelBufferImage),
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
        int sub_sprite_width  = std::round(static_cast<float>(dw) / sw);
        int sub_sprite_height = std::round(static_cast<float>(dh) / sh);
        int page              = index % GpuTexturesPerPage;
        index                 = index / GpuTexturesPerPage;
        for (int sprh = 0; sprh < sh; sprh++) {
            for (int sprw = 0; sprw < sw; sprw++) {
                int base_x           = index % GpuTexturesPerPageLine;
                int base_y           = index / GpuTexturesPerPageLine;
                int base_xx          = (base_x + sprw) % GpuTexturesPerPageLine;
                int base_yy          = (base_y + sprh) % GpuTexturesPerPageColumn;
                int sub_sprite_index = base_xx + (base_yy * GpuTexturesPerPageLine);
                sub_sprite_index     = (page * GpuTexturesPerPage) + sub_sprite_index;

                Spr_Scaled(machine,
                           x + (sprw * sub_sprite_width),
                           y + (sprh * sub_sprite_height),
                           sub_sprite_index,
                           sub_sprite_width,
                           sub_sprite_height);

                // Spr(machine, x + (sprw * GpuTextureWidth), y + (sprh * GpuTextureHeight), sub_sprite_index);
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Spr(
      Machine * machine, int x, int y, int index, int key, int sw, int sh, int dw, int dh) {
        int sub_sprite_width  = std::round(static_cast<float>(dw) / sw);
        int sub_sprite_height = std::round(static_cast<float>(dh) / sh);
        int page              = index % GpuTexturesPerPage;
        index                 = index / GpuTexturesPerPage;
        for (int sprh = 0; sprh < sh; sprh++) {
            for (int sprw = 0; sprw < sw; sprw++) {
                int base_x           = index % GpuTexturesPerPageLine;
                int base_y           = index / GpuTexturesPerPageLine;
                int base_xx          = (base_x + sprw) % GpuTexturesPerPageLine;
                int base_yy          = (base_y + sprh) % GpuTexturesPerPageColumn;
                int sub_sprite_index = base_xx + (base_yy * GpuTexturesPerPageLine);
                sub_sprite_index     = (page * GpuTexturesPerPage) + sub_sprite_index;

                Spr_Scaled(machine,
                           x + (sprw * sub_sprite_width),
                           y + (sprh * sub_sprite_height),
                           sub_sprite_index,
                           key,
                           sub_sprite_width,
                           sub_sprite_height);

                // Spr(machine, x + (sprw * GpuTextureWidth), y + (sprh * GpuTextureHeight), sub_sprite_index, key);
            }
        }
    }

    void PixelMode_384x216x4_Indexed::Spr_Scaled(Machine * machine, int x, int y, int index, int w, int h) {
        drxl_assert(w != 0, "Width cannot be zero");
        drxl_assert(h != 0, "Height cannot be zero");
        const auto & tex  = machine->m_Gpu.m_Textures_4bpp_Indexed.GetTexture(index);
        bool         xpos = w >= 0;
        bool         ypos = h >= 0;
        for (int dy = y, sy = ypos ? 0 : (h - 1); dy < y + h; dy++, sy += ypos ? 1 : -1) {
            float yf  = (static_cast<float>(sy) / h) * GpuTextureHeight;
            int   syr = std::round(yf);
            for (int dx = x, sx = xpos ? 0 : (w - 1); dx < x + w; dx++, sx += xpos ? 1 : -1) {
                float xf  = (static_cast<float>(sx) / w) * GpuTextureWidth;
                int   sxr = std::round(xf);
                m_PixelBuffer.SetPixel(dx, dy, tex.GetPixel(sxr, syr));
            }
        }

        // for (int dy = y, sy = 0; sy < GpuTextureHeight; dy++, sy++) {
        //     for (int dx = x, sx = 0; dx < GpuTextureWidth; dx++, sx++) {
        //         for (int ys = 0; ys < h; ys++) {
        //             for (int xs = 0; xs < w; xs++) { m_PixelBuffer.SetPixel(dx + xs, dy + ys, tex.GetPixel(sx, sy)); }
        //         }
        //     }
        // }
    }

    void PixelMode_384x216x4_Indexed::Spr_Scaled(Machine * machine, int x, int y, int index, int key, int w, int h) {
        drxl_assert(w != 0, "Width cannot be zero");
        drxl_assert(h != 0, "Height cannot be zero");
        const auto & tex  = machine->m_Gpu.m_Textures_4bpp_Indexed.GetTexture(index);
        bool         xpos = w >= 0;
        bool         ypos = h >= 0;
        for (int dy = y, sy = ypos ? 0 : (h - 1); dy < y + h; dy++, sy += ypos ? 1 : -1) {
            float yf  = (static_cast<float>(sy) / h) * GpuTextureHeight;
            int   syr = std::round(yf);
            for (int dx = x, sx = xpos ? 0 : (w - 1); dx < x + w; dx++, sx += xpos ? 1 : -1) {
                float xf  = (static_cast<float>(sx) / w) * GpuTextureWidth;
                int   sxr = std::round(xf);
                auto  c   = tex.GetPixel(sxr, syr);
                if (c != key) { m_PixelBuffer.SetPixel(dx, dy, c); }
            }
        }

        // for (int dy = y, sy = 0; sy < GpuTextureHeight; dy++, sy++) {
        //     for (int dx = x, sx = 0; dx < GpuTextureWidth; dx++, sx++) {
        //         for (int ys = 0; ys < h; ys++) {
        //             for (int xs = 0; xs < w; xs++) {
        //                 auto c = tex.GetPixel(sx, sy);
        //                 if (c != key) { m_PixelBuffer.SetPixel(dx + xs, dy + ys, c); }
        //             }
        //         }
        //     }
        // }
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
                                             int       sy2) {
        auto bboxmin = sf::Vector2i{GpuPixelWidth - 1, GpuPixelHeight - 1};
        auto bboxmax = sf::Vector2i{0, 0};
        auto clampv  = sf::Vector2i{GpuPixelWidth - 1, GpuPixelHeight - 1};

        bboxmin.x = std::max(0, std::min(bboxmin.x, dx0));
        bboxmin.y = std::max(0, std::min(bboxmin.y, dy0));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, dx0));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, dy0));

        bboxmin.x = std::max(0, std::min(bboxmin.x, dx1));
        bboxmin.y = std::max(0, std::min(bboxmin.y, dy1));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, dx1));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, dy1));

        bboxmin.x = std::max(0, std::min(bboxmin.x, dx2));
        bboxmin.y = std::max(0, std::min(bboxmin.y, dy2));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, dx2));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, dy2));

        sf::Vector2f s0, s1, s2;
        s0 = {static_cast<float>(sx0) / GpuTexturePagePixelWidth, static_cast<float>(sy0) / GpuTexturePagePixelHeight};
        s1 = {static_cast<float>(sx1) / GpuTexturePagePixelWidth, static_cast<float>(sy1) / GpuTexturePagePixelHeight};
        s2 = {static_cast<float>(sx2) / GpuTexturePagePixelWidth, static_cast<float>(sy2) / GpuTexturePagePixelHeight};

        sf::Vector2i P;

        for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
            for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
                auto bc_screen = barycentric(dx0, dy0, dx1, dy1, dx2, dy2, P.x, P.y);
                if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) { continue; }
                auto uvx = bc_screen.x * s0.x + bc_screen.y * s1.x + bc_screen.z * s2.x;
                auto uvy = bc_screen.x * s0.y + bc_screen.y * s1.y + bc_screen.z * s2.y;
                auto tx  = static_cast<int>(uvx) * GpuTexturePagePixelWidth;
                auto ty  = static_cast<int>(uvy) * GpuTexturePagePixelHeight;
                auto c   = machine->m_Gpu.m_Textures_4bpp_Indexed.GetGlobalPixel(tx, ty);
                m_PixelBuffer.SetPixel(P.x, P.y, c);
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
                                             int       sy2,
                                             int       key) {
        auto bboxmin = sf::Vector2i{GpuPixelWidth - 1, GpuPixelHeight - 1};
        auto bboxmax = sf::Vector2i{0, 0};
        auto clampv  = sf::Vector2i{GpuPixelWidth - 1, GpuPixelHeight - 1};

        bboxmin.x = std::max(0, std::min(bboxmin.x, dx0));
        bboxmin.y = std::max(0, std::min(bboxmin.y, dy0));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, dx0));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, dy0));

        bboxmin.x = std::max(0, std::min(bboxmin.x, dx1));
        bboxmin.y = std::max(0, std::min(bboxmin.y, dy1));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, dx1));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, dy1));

        bboxmin.x = std::max(0, std::min(bboxmin.x, dx2));
        bboxmin.y = std::max(0, std::min(bboxmin.y, dy2));
        bboxmax.x = std::min(clampv.x, std::max(bboxmax.x, dx2));
        bboxmax.y = std::min(clampv.y, std::max(bboxmax.y, dy2));

        sf::Vector2f s0, s1, s2;
        s0 = {static_cast<float>(sx0) / GpuTexturePagePixelWidth, static_cast<float>(sy0) / GpuTexturePagePixelHeight};
        s1 = {static_cast<float>(sx1) / GpuTexturePagePixelWidth, static_cast<float>(sy1) / GpuTexturePagePixelHeight};
        s2 = {static_cast<float>(sx2) / GpuTexturePagePixelWidth, static_cast<float>(sy2) / GpuTexturePagePixelHeight};

        sf::Vector2i P;

        for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
            for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
                auto bc_screen = barycentric(dx0, dy0, dx1, dy1, dx2, dy2, P.x, P.y);
                if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) { continue; }
                auto uvx = bc_screen.x * s0.x + bc_screen.y * s1.x + bc_screen.z * s2.x;
                auto uvy = bc_screen.x * s0.y + bc_screen.y * s1.y + bc_screen.z * s2.y;
                auto tx  = static_cast<int>(uvx) * GpuTexturePagePixelWidth;
                auto ty  = static_cast<int>(uvy) * GpuTexturePagePixelHeight;
                auto c   = machine->m_Gpu.m_Textures_4bpp_Indexed.GetGlobalPixel(tx, ty);
                if (c != key) { m_PixelBuffer.SetPixel(P.x, P.y, c); }
            }
        }
    }

    const PixelBuffer_384x216x4_Indexed & PixelMode_384x216x4_Indexed::GetPixelBuffer() const { return m_PixelBuffer; }

    PixelBuffer_384x216x4_Indexed & PixelMode_384x216x4_Indexed::GetPixelBuffer() { return m_PixelBuffer; }

} // namespace drxl
