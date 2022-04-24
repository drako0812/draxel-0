using System.Runtime.InteropServices;

namespace Draxel {

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PixelBuffer_384x216x4_Indexed {
        private unsafe fixed byte buffer[((Gpu.PixelWidth * Gpu.PixelHeight) * Gpu.PixelSize) / 8];

        public PixelBuffer_384x216x4_Indexed() {
            for(int i = 0; i < ((Gpu.PixelWidth * Gpu.PixelHeight) * Gpu.PixelSize) / 8; i++) {
                unsafe {
                    buffer[i] = 0;
                }
            }
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PixelMode_384x216x4_Indexed {}

}
