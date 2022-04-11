#pragma once

#include "base.hpp"
#include <deque>

namespace pxly {

    struct Machine;

    struct Keyboard {
        uint8 m_KeyP[256 / 8];
        uint8 m_Key[256 / 8];

        Keyboard();
        bool Initialize();
        void StartFrame();
        void HandleEvent(void * evt);
        bool Key(int key) const;
        bool KeyP(int key) const;
    };

    struct GamePadState {
        bool m_Up : 1;
        bool m_Down : 1;
        bool m_Left : 1;
        bool m_Right : 1;
        bool m_A : 1;
        bool m_B : 1;
        bool m_X : 1;
        bool m_Y : 1;
        bool m_Start : 1;
        bool m_Select : 1;
        bool m_Filler1 : 1;
        bool m_Filler2 : 1;
        bool m_Filler3 : 1;
        bool m_Filler4 : 1;
        bool m_Filler5 : 1;
        bool m_Filler6 : 1;
    };

    struct GamePad {
        GamePadState m_GpP;
        GamePadState m_Gp;

        GamePad();
        bool Initialize();
        void StartFrame();
        void HandleEvent(void * evt);
        bool Btn(int btn) const;
        bool BtnP(int btn) const;
    };

    struct MouseButtons {
        bool m_Left : 1;
        bool m_Right : 1;
        bool m_Middle : 1;
        bool m_X1 : 1;
        bool m_X2 : 1;
        bool m_Filler1 : 1;
        bool m_Filler2 : 1;
        bool m_Filler3 : 1;
    };

    struct MouseState {
        int16 m_X;
        int16 m_Y;
        MouseButtons m_Buttons;
        int8 m_Scroll;
    };

    struct Mouse {
        MouseState m_MouseP;
        MouseState m_Mouse;

        Mouse();
        bool Initialize();
        void StartFrame();
        void HandleEvent(void * evt);
        int16 MxP() const;
        int16 Mx() const;
        int16 MyP() const;
        int16 My() const;
        bool MBtnP(int btn) const;
        bool MBtn(int btn) const;
        int8 MScrollP() const;
        int8 MScroll() const;
    };

    struct InternalKeyboard {
        std::deque<char> m_CharPresses = {};
    };

} // namespace pxly
