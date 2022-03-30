#include "machine.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

pxly_machine * pxly_new_machine() {
    bool has_keyboard = false;
    bool has_mouse    = false;
    bool has_gamepad  = false;

    if (!al_init()) return nullptr;
    if (!al_init_image_addon()) return nullptr;
    if (!al_init_font_addon()) return nullptr;
    if (!al_init_ttf_addon()) return nullptr;
    if (!al_init_primitives_addon()) return nullptr;

    has_keyboard = al_install_keyboard();
    has_mouse    = al_install_mouse();
    has_gamepad  = al_install_joystick();

    auto display = al_create_display(800, 600);
    if (!display) { return nullptr; }
    auto eq = al_create_event_queue();
    if (!eq) {
        al_destroy_display(display);
        return nullptr;
    }
    auto rtimer = al_create_timer(ALLEGRO_BPS_TO_SECS(60));
    if (!rtimer) {
        al_destroy_event_queue(eq);
        al_destroy_display(display);
        return nullptr;
    }

    al_register_event_source(eq, al_get_display_event_source(display));
    al_register_event_source(eq, al_get_keyboard_event_source());
    al_register_event_source(eq, al_get_mouse_event_source());
    al_register_event_source(eq, al_get_joystick_event_source());
    al_register_event_source(eq, al_get_timer_event_source(rtimer));

    auto machine = new pxly_machine();
    if (!machine) {
        al_unregister_event_source(eq, al_get_timer_event_source(rtimer));
        al_destroy_timer(rtimer);
        al_destroy_event_queue(eq);
        al_destroy_display(display);
        return nullptr;
    }

    machine->m_display      = display;
    machine->m_event_queue  = eq;
    machine->m_render_timer = rtimer;

    return machine;
}

void pxly_free_machine(pxly_machine * machine) {
    al_unregister_event_source(machine->m_event_queue, al_get_timer_event_source(machine->m_render_timer));
    al_destroy_timer(machine->m_render_timer);
    al_destroy_event_queue(machine->m_event_queue);
    al_destroy_display(machine->m_display);

    delete machine;
}

bool pxly_run_machine(pxly_machine * machine) {
    ALLEGRO_EVENT evt;
    while (al_get_next_event(machine->m_event_queue, &evt)) {
        switch (evt.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE: return false; break;
        case ALLEGRO_EVENT_KEY_DOWN:
            switch (evt.keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE: return false; break;
            }
            break;
        }
    }

    pxly_update_machine(machine);
    pxly_render_machine(machine);
    return true;
}

void pxly_update_machine(pxly_machine * machine) {
    // Do things
}

void pxly_render_machine(pxly_machine * machine) {
    al_set_target_backbuffer(machine->m_display);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
}
