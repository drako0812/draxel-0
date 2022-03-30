#pragma once

#include "gpu.h"
#include <allegro5/allegro5.h>

/// This holds the raw memory of the Pixelly machine as well as other state needed to run the machine.
typedef struct pxly_machine {
    // Internal Resources
    pxly_gpu m_gpu;

    // External Resources
    ALLEGRO_DISPLAY *     m_display;
    ALLEGRO_EVENT_QUEUE * m_event_queue;
    ALLEGRO_TIMER *       m_render_timer;
} pxly_machine;

#ifdef __cplusplus
extern "C" {
#endif

pxly_machine * pxly_new_machine();
void           pxly_free_machine(pxly_machine * machine);
bool           pxly_run_machine(pxly_machine * machine);
void           pxly_update_machine(pxly_machine * machine);
void           pxly_render_machine(pxly_machine * machine);

#ifdef __cplusplus
}
#endif
