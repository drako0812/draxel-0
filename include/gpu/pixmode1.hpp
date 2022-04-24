#pragma once

#include "base.hpp"

namespace drxl {

    struct PixelBuffer_384x216x4_Indexed {
        uint8 m_Buffer[GpuPixelsToBytes(GpuPixelWidth * GpuPixelHeight, GpuPixelSize)];

        uint8 GetPixel(int x, int y) const;
        void  SetPixel(int x, int y, uint8 color);
    };

    struct PixelMode_384x216x4_Indexed {
        int16                         m_CsrX;
        int16                         m_CsrY;
        int16                         m_CsrTex;
        bool                          m_CsrOn;
        PixelBuffer_384x216x4_Indexed m_PixelBuffer;

        PixelMode_384x216x4_Indexed();
        bool                                  Initialize(Machine * machine);
        void                                  Render(Machine * machine);
        int16                                 GetCursorX() const;
        int16                                 GetCursorY() const;
        void                                  GetCursorPosition(int16 & x, int16 & y) const;
        void                                  SetCursorX(int16 x);
        void                                  SetCursorY(int16 y);
        void                                  SetCursorPosition(int16 x, int16 y);
        bool                                  IsCursorEnabled() const;
        void                                  EnableCursor(bool enabled);
        int16                                 GetCursorTexture() const;
        void                                  SetCursorTexture(uint16 id);
        const PixelBuffer_384x216x4_Indexed & GetPixelBuffer() const;
        PixelBuffer_384x216x4_Indexed &       GetPixelBuffer();
        void                                  uploadPixelsToImage(Machine * machine);

        void Line(int x0, int y0, int x1, int y1, uint8 color);
        void Tri(int x0, int y0, int x1, int y1, int x2, int y2, uint8 color);
        void Trif(int x0, int y0, int x1, int y1, int x2, int y2, uint8 color);
        void Clear(uint8 color);
        void HLine(int x, int y, int w, uint8 color);
        void VLine(int x, int y, int h, uint8 color);
        void Rect(int x, int y, int w, int h, uint8 color);
        void Rectf(int x, int y, int w, int h, uint8 color);
        void Circ(int x, int y, int r, uint8 color);
        void Circf(int x, int y, int r, uint8 color);

        /// \brief Draws sprite index at (x, y)
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        void Spr(Machine * machine, int x, int y, int index);

        /// \brief Draws sprite index at (x, y) scaled to size (w, h)
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        /// \param w The width of the image to scale to.
        /// \param h The height of the image to scale to.
        void Spr_Scaled(Machine * machine, int x, int y, int index, int w, int h);

        /// \brief Draws sprite index at (x, y) with transparency key.
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        /// \param key The color index that should be considered transparent
        void Spr(Machine * machine, int x, int y, int index, int key);

        /// \brief Draws sprite index at (x, y) scaled to size (w, h) with transparency key.
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        /// \param key The color index that should be considered transparent
        /// \param w The width of the image to scale to.
        /// \param h The height of the image to scale to.
        void Spr_Scaled(Machine * machine, int x, int y, int index, int key, int w, int h);

        /// \brief Draws sprite index at (x, y) with sprite size (w, h)
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        /// \param w Width of sprite in sprite tiles (wraps around if out of bounds)
        /// \param h Height of sprite in sprite tiles (wraps around if out of bounds)
        void Spr(Machine * machine, int x, int y, int index, int w, int h);

        /// \brief Draws sprite index at (x, y) with sprite size (w, h) with transparency key.
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        /// \param key The color index that should be considered transparent
        /// \param w Width of sprite in sprite tiles (wraps around if out of bounds)
        /// \param h Height of sprite in sprite tiles (wraps around if out of bounds)
        void Spr(Machine * machine, int x, int y, int index, int key, int w, int h);

        /// \brief Draws sprite index at (x, y) with sprite size (sw, sh) scaled to size (dw, dh).
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        /// \param sw Width of sprite in sprite tiles (wraps around if out of bounds)
        /// \param sh Height of sprite in sprite tiles (wraps around if out of bounds)
        /// \param dw Width to draw the sprite with in pixels
        /// \param dh Height to draw the sprite with in pixels
        void Spr(Machine * machine, int x, int y, int index, int sw, int sh, int dw, int dh);

        /// \brief Draws sprite index at (x, y) with sprite size (sw, sh) scaled to size (dw, dh) with transparency key.
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param index The index of the sprite in the graphics memory
        /// \param key The color index that should be considered transparent
        /// \param sw Width of sprite in sprite tiles (wraps around if out of bounds)
        /// \param sh Height of sprite in sprite tiles (wraps around if out of bounds)
        /// \param dw Width to draw the sprite with in pixels
        /// \param dh Height to draw the sprite with in pixels
        void Spr(Machine * machine, int x, int y, int index, int key, int sw, int sh, int dw, int dh);

        /// \brief Draws a textured triangle at [(dx0, dy1), (dx1, dy1), (dx2, dy2)]
        /// \param dx0 Destination X-Coordiate 1
        /// \param dy0 Destination Y-Coordiate 1
        /// \param dx1 Destination X-Coordiate 2
        /// \param dy1 Destination Y-Coordiate 2
        /// \param dx2 Destination X-Coordiate 3
        /// \param dy2 Destination Y-Coordiate 3
        /// \param sx0 Source X-Coordiate 1 in global texture coordinates
        /// \param sy0 Source Y-Coordiate 1 in global texture coordinates
        /// \param sx1 Source X-Coordiate 2 in global texture coordinates
        /// \param sy1 Source Y-Coordiate 2 in global texture coordinates
        /// \param sx2 Source X-Coordiate 3 in global texture coordinates
        /// \param sy2 Source Y-Coordiate 3 in global texture coordinates
        void Textri(Machine * machine,
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
                    int       sy2);

        /// \brief Draws a textured triangle at [(dx0, dy1), (dx1, dy1), (dx2, dy2)] with transparency key.
        /// \param dx0 Destination X-Coordiate 1
        /// \param dy0 Destination Y-Coordiate 1
        /// \param dx1 Destination X-Coordiate 2
        /// \param dy1 Destination Y-Coordiate 2
        /// \param dx2 Destination X-Coordiate 3
        /// \param dy2 Destination Y-Coordiate 3
        /// \param sx0 Source X-Coordiate 1 in global texture coordinates
        /// \param sy0 Source Y-Coordiate 1 in global texture coordinates
        /// \param sx1 Source X-Coordiate 2 in global texture coordinates
        /// \param sy1 Source Y-Coordiate 2 in global texture coordinates
        /// \param sx2 Source X-Coordiate 3 in global texture coordinates
        /// \param sy2 Source Y-Coordiate 3 in global texture coordinates
        /// \param key The color index that should be considered transparent
        void Textri(Machine * machine,
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
                    int       key);
    };

    struct Texture_4bpp_Indexed {
        uint8 m_Buffer[GpuPixelsToBytes(GpuTextureWidth * GpuTextureHeight, GpuPixelSize)];

        uint8 GetPixel(int8 x, int8 y) const;
        void  SetPixel(int8 x, int8 y, uint8 color);
    };

    struct Textures_4bpp_Indexed {
        Texture_4bpp_Indexed m_Textures[GpuTexturesPerPage * GpuTexturePages];

        Texture_4bpp_Indexed &       GetTexture(uint16 id);
        const Texture_4bpp_Indexed & GetTexture(uint16 id) const;

        uint8 GetPagePixel(int page, int x, int y) const;
        uint8 GetGlobalPixel(int x, int y) const;
    };

}