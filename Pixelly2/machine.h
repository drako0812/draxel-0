#pragma once

#include "base.h"
#include "gpu.h"
#include "input.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

typedef struct asIScriptEngine asIScriptEngine;
typedef struct asIScriptContext asIScriptContext;

/// This holds the raw memory of the Pixelly machine as well as other state needed to run the machine.
typedef struct pxly_machine {
    // Internal Resources
    pxly_gpu m_gpu;
    pxly_input_keyboard m_keyb;

    // External Resources
    ALLEGRO_DISPLAY *     m_display;
    ALLEGRO_EVENT_QUEUE * m_event_queue;
    ALLEGRO_TIMER *       m_render_timer;
    ALLEGRO_BITMAP *      m_render_texture;
    ALLEGRO_FONT *        m_font;

    bool               m_running;
    int                m_exit_status;
    asIScriptEngine *  m_script_engine;
    asIScriptEngine *  m_script_engine_cart;
    asIScriptContext * m_script_context;
    bool               m_script_error;
} pxly_machine;

PXLY_API pxly_machine * pxly_new_machine();
PXLY_API void           pxly_free_machine(pxly_machine * machine);
PXLY_API bool           pxly_run_machine(pxly_machine * machine);
PXLY_API void           pxly_update_machine(pxly_machine * machine);
PXLY_API void           pxly_render_machine(pxly_machine * machine);
PXLY_API bool           pxly_init_script_engine(pxly_machine * machine);
