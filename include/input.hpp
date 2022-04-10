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
        bool Key(int key);
        bool KeyP(int key);
    };

    struct InternalKeyboard {
        std::deque<char> m_CharPresses = {};
    };

} // namespace pxly
