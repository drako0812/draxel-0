#include "machine.hpp"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

namespace pxly {

    Machine * Machine::Instance = nullptr;

    Machine::Machine() :
            m_Disp{nullptr},
            m_EQ{nullptr},
            m_RTimer{nullptr},
            m_RTex{nullptr},
            m_Font{nullptr},
            m_Running{false},
            m_ExitStatus{0} {}

    Machine::~Machine() {
        if (m_Font) { al_destroy_font(m_Font); }
        if (m_RTex) { al_destroy_bitmap(m_RTex); }
        if (m_RTimer) { al_destroy_timer(m_RTimer); }
        if (m_EQ) { al_destroy_event_queue(m_EQ); }
        if (m_Disp) { al_destroy_display(m_Disp); }
        Instance = nullptr;
    }

    bool Machine::Initialize() {
        bool has_kb = false, has_mouse = false, has_gamepad = false;

        pxly_assert(al_init(), "Failed to initialize Allegro 5");
        pxly_assert(al_init_image_addon(), "Failed to initialize Allegro 5 Image addon");
        pxly_assert(al_init_font_addon(), "Failed to initialize Allegro 5 Font addon");
        pxly_assert(al_init_ttf_addon(), "Failed to initialize Allegro 5 TTF addon");
        pxly_assert(al_init_primitives_addon(), "Failed to initialize Allegro 5 Primitives addon");

        has_kb      = al_install_keyboard();
        has_mouse   = al_install_mouse();
        has_gamepad = al_install_joystick();

        pxly_assert(has_kb || has_mouse || has_gamepad,
                    "Failed to initialize an input method. Pixelly requires some kind of input method.");

        al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);
        m_Disp = al_create_display(800, 600);
        pxly_assert(m_Disp, "Failed to create display");
        m_EQ = al_create_event_queue();
        pxly_assert(m_EQ, "Failed to create event queue");
        m_RTimer = al_create_timer(ALLEGRO_BPS_TO_SECS(60));
        pxly_assert(m_RTimer, "Failed to create render timer");

        al_register_event_source(m_EQ, al_get_display_event_source(m_Disp));
        al_register_event_source(m_EQ, al_get_timer_event_source(m_RTimer));
        if (has_kb) { al_register_event_source(m_EQ, al_get_keyboard_event_source()); }
        if (has_mouse) { al_register_event_source(m_EQ, al_get_mouse_event_source()); }
        if (has_gamepad) { al_register_event_source(m_EQ, al_get_joystick_event_source()); }

        al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_CONVERT_BITMAP);
        m_RTex = al_create_bitmap(GpuPixelWidth, GpuPixelHeight);
        pxly_assert(m_RTex, "Failed to create render texture");

        m_Font = al_create_builtin_font();
        pxly_assert(m_Font, "Failed to load font");

        pxly_assert(m_Gpu.Initialize(), "Failed to initialize Pixelly GPU");
        pxly_assert(m_Kb.Initialize(), "Failed to initialize Pixelly Keyboard");

        m_Running    = true;
        m_ExitStatus = 0;

        pxly_assert(InitScriptEngine(), "Failed to create scripting engine");

        Instance = this;

        return true;
    }

    bool Machine::Run() {
        m_Kb.StartFrame();
        
        ALLEGRO_EVENT evt;
        while (al_get_next_event(m_EQ, &evt)) {
            m_Kb.HandleEvent(&evt);
            switch (evt.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE: return false; break;
            case ALLEGRO_EVENT_KEY_DOWN:
                switch (evt.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE: return false; break;
                }
                break;
            }
        }

        Update();

        if (!m_Running) { return false; }

        Render();
        return true;
    }

    void Machine::Update() {
        // Nothing just yet.
    }

    void Machine::Render() {
        al_set_target_backbuffer(m_Disp);
        al_clear_to_color(al_map_rgb(32, 32, 32));
        m_Gpu.Render(this);
        al_flip_display();
    }

    bool Machine::InitScriptEngine() { return true; }

} // namespace pxly
