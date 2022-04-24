using System.Runtime.InteropServices;
using SFML.Window;

namespace Draxel {

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct MouseButtons {
        private enum MouseButton : byte {
            Left = 1 << 7,
            Right = 1 << 6,
            Middle = 1 << 5,
            X1 = 1 << 4,
            X2 = 1 << 3,
        }

        private byte data1;

        #region Properties

        public bool Left {
            get {
                return (data1 & (byte)MouseButton.Left) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)MouseButton.Left;
                } else {
                    data1 &= unchecked((byte)~MouseButton.Left);
                }
            }
        }

        public bool Right {
            get {
                return (data1 & (byte)MouseButton.Right) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)MouseButton.Right;
                } else {
                    data1 &= unchecked((byte)~MouseButton.Right);
                }
            }
        }

        public bool Middle {
            get {
                return (data1 & (byte)MouseButton.Middle) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)MouseButton.Middle;
                } else {
                    data1 &= unchecked((byte)~MouseButton.Middle);
                }
            }
        }

        public bool X1 {
            get {
                return (data1 & (byte)MouseButton.X1) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)MouseButton.X1;
                } else {
                    data1 &= unchecked((byte)~MouseButton.X1);
                }
            }
        }

        public bool X2 {
            get {
                return (data1 & (byte)MouseButton.X2) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)MouseButton.X2;
                } else {
                    data1 &= unchecked((byte)~MouseButton.X2);
                }
            }
        }

        #endregion

        public MouseButtons() {
            data1 = 0;
        }

        public MouseButtons(MouseButtons other) {
            data1 = other.data1;
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct MouseState {
        public short X;
        public short Y;
        public MouseButtons Buttons;
        public sbyte Scroll;

        public MouseState() {
            X = 0;
            Y = 0;
            Buttons = new MouseButtons();
            Scroll = 0;
        }

        public MouseState(MouseState other) {
            X = other.X;
            Y = other.Y;
            Buttons = other.Buttons;
            Scroll = other.Scroll;
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Mouse {
        public MouseState Prv;
        public MouseState Cur;

        public Mouse() {
            Prv = new MouseState();
            Cur = new MouseState();
        }

        public bool Initialize() {
            Prv = new MouseState();
            Cur = new MouseState();
            return true;
        }

        public void StartFrame() {
            Prv = Cur;
            Cur.Scroll = 0;
        }

        public void HandleEvent(Event evt) {
            switch (evt.Type) {
                case EventType.MouseButtonPressed:
                    switch (evt.MouseButton.Button) {
                        case SFML.Window.Mouse.Button.Left: Cur.Buttons.Left = true; break;
                        case SFML.Window.Mouse.Button.Right: Cur.Buttons.Right = true; break;
                        case SFML.Window.Mouse.Button.Middle: Cur.Buttons.Middle = true; break;
                        case SFML.Window.Mouse.Button.XButton1: Cur.Buttons.X1 = true; break;
                        case SFML.Window.Mouse.Button.XButton2: Cur.Buttons.X2 = true; break;
                    }
                    break;
                case EventType.MouseButtonReleased:
                    switch (evt.MouseButton.Button) {
                        case SFML.Window.Mouse.Button.Left: Cur.Buttons.Left = false; break;
                        case SFML.Window.Mouse.Button.Right: Cur.Buttons.Right = false; break;
                        case SFML.Window.Mouse.Button.Middle: Cur.Buttons.Middle = false; break;
                        case SFML.Window.Mouse.Button.XButton1: Cur.Buttons.X1 = false; break;
                        case SFML.Window.Mouse.Button.XButton2: Cur.Buttons.X2 = false; break;
                    }
                    break;
                case EventType.MouseMoved:
                    Cur.X = (short)evt.MouseMove.X;
                    Cur.Y = (short)evt.MouseMove.Y;
                    break;
                case EventType.MouseWheelScrolled:
                    ref var scroll = ref evt.MouseWheelScroll;
                    if (scroll.Wheel == SFML.Window.Mouse.Wheel.VerticalWheel) {
                        float d = scroll.Delta;
                        if ((d < 0) && (d > -1.0f)) { d = -1.0f; }
                        if ((d > 0) && (d < 1.0f)) { d = 1.0f; }
                        Cur.Scroll += (sbyte)d;
                    }
                    break;
            }
        }

        public short mxp() { return Prv.X; }
        public short mx() { return Cur.X; }
        public short myp() { return Prv.Y; }
        public short my() { return Cur.Y; }
        public bool mbtnp(int btn) {
            switch(btn) {
                case 0: return Prv.Buttons.Left;
                case 1: return Prv.Buttons.Right;
                case 2: return Prv.Buttons.Middle;
                case 3: return Prv.Buttons.X1;
                case 4: return Prv.Buttons.X2;
                default: return false;
            }
        }
        public bool mbtn(int btn) {
            switch(btn) {
                case 0: return Cur.Buttons.Left;
                case 1: return Cur.Buttons.Right;
                case 2: return Cur.Buttons.Middle;
                case 3: return Cur.Buttons.X1;
                case 4: return Cur.Buttons.X2;
                default: return false;
            }
        }
        public sbyte mscrollp() { return Prv.Scroll; }
        public sbyte mscroll() { return Cur.Scroll; }
    }

}
