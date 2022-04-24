using System.Runtime.InteropServices;

namespace Draxel {

    public class Machine {

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct Internal {
            public Gpu Gpu;
            public Keyboard Kb;
            public GamePad Gp;
            public Mouse Mouse;
        }

        public Internal Data;

        public bool Initialize() {
            return false;
        }

        public bool Run() {
            return false;
        }
    }

}
