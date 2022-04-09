#pragma once

#include "base.hpp"

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

} // namespace pxly
