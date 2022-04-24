using System.Runtime.InteropServices;

namespace Draxel {

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PixelBuffer_384x216x4_Indexed {
        private unsafe fixed byte buffer[((Gpu.PixelWidth * Gpu.PixelHeight) * Gpu.PixelSize) / 8];

        public PixelBuffer_384x216x4_Indexed() {
            Clear(0);
        }

        public void Clear(byte color) {
            for (int i = 0; i < ((Gpu.PixelWidth * Gpu.PixelHeight) * Gpu.PixelSize) / 8; i++) {
                unsafe {
                    buffer[i] = color;
                }
            }
        }

        public byte GetPixel(int x, int y) {
            if(x < 0 || x >= Gpu.PixelWidth || y < 0 || y >= Gpu.PixelHeight) {
                return 0;
            }
            int index = x + (Gpu.PixelWidth * y);
            int byte_index = index / 2;
            int nibble_index = index % 2;
            unsafe {
                switch (nibble_index) {
                    case 0: return (byte)((buffer[index] & 0xF0) >> 4);
                    case 1: return (byte)(buffer[index] & 0x0F);
                    default: return 0;
                }
            }
        }

        public void SetPixel(int x, int y, byte color) {
            if(x < 0 || x >= Gpu.PixelWidth || y < 0 || y >= Gpu.PixelHeight) {
                return;
            }
            int index = x + (Gpu.PixelWidth * y);
            int byte_index = index / 2;
            int nibble_index = index % 2;
            unsafe {
                switch (nibble_index) {
                    case 0: buffer[index] = (byte)((buffer[index] & 0x0F) | (color << 4)); break;
                    case 1: buffer[index] = (byte)((buffer[index] & 0xF0) | (color & 0x0F)); break;
                    default: break;
                }
            }
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PixelMode_384x216x4_Indexed {
        
    }

}
