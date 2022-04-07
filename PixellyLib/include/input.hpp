#pragma once

#include "base.hpp"

namespace pxly {

    struct Machine;

    struct Keyboard {
        uint8 m_KeyP[256/8];
        uint8 m_Key[256/8];

        PXLY_API Keyboard();
        PXLY_API bool Initialize();
        PXLY_API void StartFrame();
        PXLY_API void HandleEvent(void * evt);
        PXLY_API bool Key(int key);
        PXLY_API bool KeyP(int key);
    };

}
