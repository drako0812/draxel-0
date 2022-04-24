#include "gpu.hpp"

namespace drxl {

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

} // namespace drxl
