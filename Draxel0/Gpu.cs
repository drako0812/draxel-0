using System.Runtime.InteropServices;
using SFML.Graphics;

namespace Draxel {

    [StructLayout(LayoutKind.Explicit, Pack = 1)]
    public struct GpuModeUnion {
        [FieldOffset(0)]
        TextMode TextMode;

        [FieldOffset(0)]
        PixelMode_384x216x4_Indexed PixelMode_384x216x4_Indexed;
    }

    [StructLayout(LayoutKind.Explicit, Pack = 1)]
    public struct GpuTextureUnion {
        [FieldOffset(0)]
        Textures_4bpp_Indexed Textures_4bpp_Indexed;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Gpu {
        public const int PaletteSize = 16;
        public const int PixelWidth = 384;
        public const int PixelHeight = 216;
        public const int PixelSize = 4;

        public enum Mode : byte {
            Text,
            Pixel_384x216x4_Indexed,
        }

        public Mode GpuMode;
        public GpuModeUnion ModeData;
        public GpuTextureUnion TextureData;
        public Palette Palette;

        public Gpu() { throw new NotImplementedException(); }
        public bool Initialize() { throw new NotImplementedException(); }
        public void Render(Machine machine) { throw new NotImplementedException(); }
        public Mode GetMode() { return GpuMode; }
        public void SetMode(Mode mode, Machine machine) { throw new NotImplementedException(); }
    }

    public class InternalGpu {
        public Image? PixelBufferImage;
        public Texture? PixelBufferTexture;
    }
}
