using System.Runtime.InteropServices;

namespace Draxel {
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Color {
        public byte R, G, B;

        public static implicit operator SFML.Graphics.Color(Color c) {
            return new SFML.Graphics.Color(c.R, c.G, c.B);
        }

        public static implicit operator Color(SFML.Graphics.Color c) {
            return new Color { R = c.R, G = c.G, B = c.B };
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Palette {
        private unsafe fixed byte Colors[3 * Gpu.PaletteSize];

        Color GetColor(int index) {
            Utility.Assert(index < Gpu.PaletteSize, () => $"Color index {index} out of bounds, maximum is {Gpu.PaletteSize - 1}");
            int i = index * 3;
            unsafe {
                return new Color { R = Colors[i], G = Colors[i + 1], B = Colors[i + 2] };
            }
        }

        void SetColor(int index, Color color) {
            Utility.Assert(index < Gpu.PaletteSize, () => $"Color index {index} out of bounds, maximum is {Gpu.PaletteSize - 1}");
            int i = index * 3;
            unsafe {
                Colors[i] = color.R;
                Colors[i + 1] = color.G;
                Colors[i + 2] = color.B;
            }
        }
    }

}
