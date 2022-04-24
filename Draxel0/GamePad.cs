using System;
using System.Runtime.InteropServices;
using SFML.Window;
namespace Draxel {

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct GamePadState {
        private enum Button : byte {
            Up = 1 << 7,
            Down = 1 << 6,
            Left = 1 << 5,
            Right = 1 << 4,
            A = 1 << 3,
            B = 1 << 2,
            X = 1 << 1,
            Y = 1,
            Start = 1 << 7,
            Select = 1 << 6,
        }

        private byte data1, data2;

        #region Properties
        public bool Up {
            get {
                return (data1 & (byte)Button.Up) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.Up;
                } else {
                    data1 &= unchecked((byte)~Button.Up);
                }
            }
        }
        public bool Down {
            get {
                return (data1 & (byte)Button.Down) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.Down;
                } else {
                    data1 &= unchecked((byte)~Button.Down);
                }
            }
        }
        public bool Left {
            get {
                return (data1 & (byte)Button.Left) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.Left;
                } else {
                    data1 &= unchecked((byte)~Button.Left);
                }
            }
        }
        public bool Right {
            get {
                return (data1 & (byte)Button.Right) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.Right;
                } else {
                    data1 &= unchecked((byte)~Button.Right);
                }
            }
        }
        public bool A {
            get {
                return (data1 & (byte)Button.A) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.A;
                } else {
                    data1 &= unchecked((byte)~Button.A);
                }
            }
        }
        public bool B {
            get {
                return (data1 & (byte)Button.B) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.B;
                } else {
                    data1 &= unchecked((byte)~Button.B);
                }
            }
        }
        public bool X {
            get {
                return (data1 & (byte)Button.X) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.X;
                } else {
                    data1 &= unchecked((byte)~Button.X);
                }
            }
        }
        public bool Y {
            get {
                return (data1 & (byte)Button.Y) != 0;
            }
            set {
                if (value) {
                    data1 |= (byte)Button.Y;
                } else {
                    data1 &= unchecked((byte)~Button.Y);
                }
            }
        }
        public bool Start {
            get {
                return (data2 & (byte)Button.Start) != 0;
            }
            set {
                if (value) {
                    data2 |= (byte)Button.Start;
                } else {
                    data2 &= unchecked((byte)~Button.Start);
                }
            }
        }
        public bool Select {
            get {
                return (data2 & (byte)Button.Select) != 0;
            }
            set {
                if (value) {
                    data2 |= (byte)Button.Select;
                } else {
                    data2 &= unchecked((byte)~Button.Select);
                }
            }
        }
        #endregion

        public GamePadState() {
            data1 = 0;
            data2 = 0;
        }

        public GamePadState(GamePadState other) {
            data1 = other.data1;
            data2 = other.data2;
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct GamePad {
        public GamePadState Prv;
        public GamePadState Cur;

        public GamePad() {
            Prv = new GamePadState();
            Cur = new GamePadState();
        }

        public bool Initialize() {
            Prv = new GamePadState();
            Cur = new GamePadState();
            return true;
        }

        public void StartFrame() {
            Prv = Cur;
            Cur = new GamePadState();
        }

        public void HandleEvent(Event evt) {
            switch (evt.Type) {
                case EventType.KeyPressed:
                    switch (evt.Key.Code) {
                        case SFML.Window.Keyboard.Key.Up: Cur.Up = true; break;
                        case SFML.Window.Keyboard.Key.Down: Cur.Down = true; break;
                        case SFML.Window.Keyboard.Key.Left: Cur.Left = true; break;
                        case SFML.Window.Keyboard.Key.Right: Cur.Right = true; break;
                        case SFML.Window.Keyboard.Key.Z: Cur.A = true; break;
                        case SFML.Window.Keyboard.Key.X: Cur.B = true; break;
                        case SFML.Window.Keyboard.Key.A: Cur.X = true; break;
                        case SFML.Window.Keyboard.Key.S: Cur.Y = true; break;
                        case SFML.Window.Keyboard.Key.Enter: Cur.Start = true; break;
                        case SFML.Window.Keyboard.Key.RShift: Cur.Select = true; break;
                    }
                    break;
                case EventType.KeyReleased:
                    switch (evt.Key.Code) {
                        case SFML.Window.Keyboard.Key.Up: Cur.Up = false; break;
                        case SFML.Window.Keyboard.Key.Down: Cur.Down = false; break;
                        case SFML.Window.Keyboard.Key.Left: Cur.Left = false; break;
                        case SFML.Window.Keyboard.Key.Right: Cur.Right = false; break;
                        case SFML.Window.Keyboard.Key.Z: Cur.A = false; break;
                        case SFML.Window.Keyboard.Key.X: Cur.B = false; break;
                        case SFML.Window.Keyboard.Key.A: Cur.X = false; break;
                        case SFML.Window.Keyboard.Key.S: Cur.Y = false; break;
                        case SFML.Window.Keyboard.Key.Enter: Cur.Start = false; break;
                        case SFML.Window.Keyboard.Key.RShift: Cur.Select = false; break;
                    }
                    break;
            }
        }

        public bool btn(int btn) {
            switch (btn) {
                case 0: return Cur.Up;
                case 1: return Cur.Down;
                case 2: return Cur.Left;
                case 3: return Cur.Right;
                case 4: return Cur.A;
                case 5: return Cur.B;
                case 6: return Cur.X;
                case 7: return Cur.Y;
                case 8: return Cur.Start;
                case 9: return Cur.Select;
                default: return false;
            }
        }
        public bool btnp(int btn) {
            switch (btn) {
                case 0: return Prv.Up;
                case 1: return Prv.Down;
                case 2: return Prv.Left;
                case 3: return Prv.Right;
                case 4: return Prv.A;
                case 5: return Prv.B;
                case 6: return Prv.X;
                case 7: return Prv.Y;
                case 8: return Prv.Start;
                case 9: return Prv.Select;
                default: return false;
            }
        }
    }

}
