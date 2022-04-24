using System.Runtime.InteropServices;
using SFML.Window;

namespace Draxel {

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Keyboard {
        public const int KeyCount = 256;
        public const int KeySize = KeyCount / 8;
        private unsafe fixed byte Prv[KeySize];
        private unsafe fixed byte Cur[KeySize];

        private unsafe static bool getBit(int index, byte* arr, int size) {
            var byte_index = index / 8;
            var bit_index = index % 8;

            Utility.Assert(byte_index < size, () => $"byte_index ({byte_index}) out of bounds, array size {size}");

            var value = arr[byte_index];
            var bit = 1 & (value >> bit_index);
            return bit == 1;
        }

        private unsafe static void setBit(int index, byte* arr, int size, bool value) {
            var byte_index = index / 8;
            var bit_index = index % 8;

            Utility.Assert(byte_index < size, () => $"byte_index ({byte_index}) out of bounds, array size {size}");

            var ovalue = arr[byte_index];
            var mask = ~(1 << bit_index);
            ovalue = (byte)((ovalue & mask) | ((value ? 1 : 0) << bit_index));
            arr[byte_index] = ovalue;
        }

        public Keyboard() {
            for (int i = 0; i < KeySize; i++) {
                unsafe {
                    Prv[i] = 0;
                    Cur[i] = 0;
                }
            }
        }

        bool Initialize() {
            for (int i = 0; i < KeySize; i++) {
                unsafe {
                    Prv[i] = 0;
                    Cur[i] = 0;
                }
            }
            return true;
        }

        void StartFrame() {
            for (int i = 0; i < KeySize; i++) {
                unsafe {
                    Prv[i] = Cur[i];
                }
            }
        }

        void HandleEvent(Event evt) {
            switch (evt.Type) {
                case SFML.Window.EventType.KeyPressed:
                    unsafe {
                        fixed (byte* b = Cur) {
                            setBit((int)evt.Key.Code, b, KeySize, true);
                        }
                    }
                    break;
                case SFML.Window.EventType.KeyReleased:
                    unsafe {
                        fixed (byte* b = Cur) {
                            setBit((int)evt.Key.Code, b, KeySize, false);
                        }
                    }
                    break;
            }
        }

        bool key(int k) {
            bool ret;
            unsafe {
                fixed (byte* b = Cur) {
                    ret = getBit(k, b, KeySize);
                }
            }
            return ret;
        }

        bool keyp(int k) {
            bool ret;
            unsafe {
                fixed (byte* b = Prv) {
                    ret = getBit(k, b, KeySize);
                }
            }
            return ret;
        }
    }

}
