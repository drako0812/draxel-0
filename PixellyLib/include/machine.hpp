#pragma once

#include "base.hpp"
#include "gpu.hpp"
#include "input.hpp"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

namespace pxly {

    struct Machine {
        // Internal Resources
        Gpu m_Gpu;
        Keyboard m_Kb;

        // External Resources
        ALLEGRO_DISPLAY *     m_Disp;
        ALLEGRO_EVENT_QUEUE * m_EQ;
        ALLEGRO_TIMER *       m_RTimer;
        ALLEGRO_BITMAP *      m_RTex;
        ALLEGRO_FONT *        m_Font;

        bool m_Running;
        int  m_ExitStatus;
        // asIScriptEngine * m_script_engine;
        // asIScriptEngine * m_script_engine_cart;
        // asIScriptContext * m_script_context;
        // bool m_script_error;

        PXLY_API Machine();
        PXLY_API ~Machine();
        PXLY_API bool Initialize();
        PXLY_API bool Run();
        PXLY_API void Update();
        PXLY_API void Render();
        PXLY_API bool InitScriptEngine();

        static Machine * Instance;
    };

} // namespace pxly
