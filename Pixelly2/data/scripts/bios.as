namespace kb {
    enum Key {
        KEY_A = 1,
        KEY_B = 2,
        KEY_C = 3,
        KEY_D = 4,
        KEY_E = 5,
        KEY_F = 6,
        KEY_G = 7,
        KEY_H = 8,
        KEY_I = 9,
        KEY_J = 10,
        KEY_K = 11,
        KEY_L = 12,
        KEY_M = 13,
        KEY_N = 14,
        KEY_O = 15,
        KEY_P = 16,
        KEY_Q = 17,
        KEY_R = 18,
        KEY_S = 19,
        KEY_T = 20,
        KEY_U = 21,
        KEY_V = 22,
        KEY_W = 23,
        KEY_X = 24,
        KEY_Y = 25,
        KEY_Z = 26,

        KEY_0 = 27,
        KEY_1 = 28,
        KEY_2 = 29,
        KEY_3 = 30,
        KEY_4 = 31,
        KEY_5 = 32,
        KEY_6 = 33,
        KEY_7 = 34,
        KEY_8 = 35,
        KEY_9 = 36,

        KEY_PAD_0 = 37,
        KEY_PAD_1 = 38,
        KEY_PAD_2 = 39,
        KEY_PAD_3 = 40,
        KEY_PAD_4 = 41,
        KEY_PAD_5 = 42,
        KEY_PAD_6 = 43,
        KEY_PAD_7 = 44,
        KEY_PAD_8 = 45,
        KEY_PAD_9 = 46,

        F1 = 47,
        F2 = 48,
        F3 = 49,
        F4 = 50,
        F5 = 51,
        F6 = 52,
        F7 = 53,
        F8 = 54,
        F9 = 55,
        F10 = 56,
        F11 = 57,
        F12 = 58,

        ESCAPE = 59,
        TILDE = 60,
        MINUS = 61,
        EQUALS = 62,
        BACKSPACE = 63,
        TAB = 64,
        OPENBRACE = 65,
        CLOSEBRACE = 66,
        ENTER = 67,
        SEMICOLON = 68,
        QUOTE = 69,
        BACKSLASH = 70,
        BACKSLASH2 = 71,
        COMMA = 72,
        FULLSTOP = 73,
        SLASH = 74,
        SPACE = 75,

        INSERT = 76,
        DELETE = 77,
        HOME = 78,
        END = 79,
        PGUP = 80,
        PGDN = 81,
        LEFT = 82,
        RIGHT = 83,
        UP = 84,
        DOWN = 85,

        KEY_PAD_SLASH = 86,
        KEY_PAD_ASTERISK = 87,
        KEY_PAD_MINUS = 88,
        KEY_PAD_PLUS = 89,
        KEY_PAD_DELETE = 90,
        KEY_PAD_ENTER = 91,

        PRINTSCREEN = 92,
        PAUSE = 93,

        ABNT_C1 = 94,
        YEN = 95,
        KANA = 96,
        CONVERT = 97,
        NOCONVERT = 98,
        AT = 99,
        CIRCUMFLEX = 100,
        COLON2 = 101,
        KANJI = 102,

        KEY_PAD_EQUALS = 103,
        KEY_PAD_BACKQUOTE = 104,
        KEY_PAD_SEMICOLON2 = 105,
        COMMAND = 106,

        BACK = 107,
        VOLUME_UP = 108,
        VOLUME_DOWN = 109,

        UNKNOWN = 128,

        MODIFIERS = 215,

        LSHIFT = 215,
        RSHIFT = 216,
        LCTRL = 217,
        RCTRL = 218,
        ALT = 219,
        ALTGR = 220,
        LWIN = 221,
        RWIN = 222,
        MENU = 223,
        SCROLLLOCK = 224,
        NUMLOCK = 225,
        CAPSLOCK = 226,

        MAX
    }
}

namespace bios {
    
    uint8 CurrentForeground = 15;
    uint8 CurrentBackground = 0;
    uint8 CurrentFillCh = 0x20;

    void print_ch(uint8 ch) {
        if(ch == 0x09) {
            gpu::text::csrx((gpu::text::gcsrx()+4) & ~(4-1));
        } else if(ch == 0x08 && gpu::text::gcsrx() >= 0) {
            gpu::text::csrx(gpu::text::gcsrx() - 1);
        } else if(ch == 0x0D) {
            gpu::text::csrx(0);
        } else if(ch == 0x0A) {
            gpu::text::csrx(0);
            gpu::text::csry(gpu::text::gcsry() + 1);
        } else if(ch >= 0x20 && ch <= 0x7E) {
            gpu::text::putch(gpu::text::gcsrx(), gpu::text::gcsry(), CurrentForeground, CurrentBackground, ch);
            gpu::text::csrx(gpu::text::gcsrx() + 1);
        }

        if(gpu::text::gcsrx() >= 48) {
            gpu::text::csrx(0);
            gpu::text::csry(gpu::text::gcsry() + 1);
        }

        if(gpu::text::gcsry() >= 27) {
            gpu::text::csry(26);
            gpu::text::scrollu(1, CurrentForeground, CurrentBackground, CurrentFillCh);
        }
    }

    void print_str(const string &in str) {
        for(uint i = 0; i < str.length(); i++) {
            print_ch(str[i]);
        }
    }

    void log(const string &in str) {
        print(str);
        print_str(str);
    }

    void init() {
        log("Initializing Pixelly BIOS.\n");
    }

}
