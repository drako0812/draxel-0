#include "gpu.hpp"
#include "base.hpp"
#include <fmt/format.h>

namespace drxl {

    uint8 Textures_4bpp_Indexed::GetPagePixel(int page, int x, int y) const {
        int tex_x         = x / GpuTexturesPerPageLine;
        int pix_x         = x % GpuTexturesPerPageLine;
        int tex_y         = y / GpuTexturesPerPageColumn;
        int pix_y         = y % GpuTexturesPerPageColumn;
        int tex_sub_index = tex_x + (tex_y * GpuTexturesPerPageLine);
        int tex_index     = page + tex_sub_index;
        return m_Textures[tex_index].GetPixel(pix_x, pix_y);
    }

    uint8 Textures_4bpp_Indexed::GetGlobalPixel(int x, int y) const {
        // Convert Global Pixel coordinate to texture page and texture-position

        int page_x     = x / static_cast<int>(gcem::round(gcem::sqrt(GpuTexturePages)));
        int tex_x      = x % static_cast<int>(gcem::round(gcem::sqrt(GpuTexturePages)));
        int page_y     = y / static_cast<int>(gcem::round(gcem::sqrt(GpuTexturePages)));
        int tex_y      = y % static_cast<int>(gcem::round(gcem::sqrt(GpuTexturePages)));
        int page_index = page_x + (page_y * gcem::sqrt(GpuTexturePages));
        return GetPagePixel(page_index, tex_x, tex_y);
    }

    const Texture_4bpp_Indexed & Textures_4bpp_Indexed::GetTexture(uint16 id) const {
        const auto max_index = GpuTexturesPerPage * GpuTexturePages;
        drxl_assert(id < max_index, fmt::format("Texture ID {} out of range. Must be less than {}", id, max_index));
        return m_Textures[id];
    }

    Texture_4bpp_Indexed & Textures_4bpp_Indexed::GetTexture(uint16 id) {
        const auto max_index = GpuTexturesPerPage * GpuTexturePages;
        drxl_assert(id < max_index, fmt::format("Texture ID {} out of range. Must be less than {}", id, max_index));
        return m_Textures[id];
    }

} // namespace drxl
