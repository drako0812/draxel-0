#include "gpu.hpp"

namespace drxl {

    Color Palette::GetColor(uint8 index) const { return m_Colors[index]; }

    void Palette::SetColor(uint8 index, Color color) { m_Colors[index] = color; }

} // namespace drxl
