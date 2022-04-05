#include "machine.h"
#include "base_intern.hpp"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>
#include <iostream>
#include <string>

#include <angelscript.h>
#include <angelscript/autowrapper/aswrappedcall.h>
#include <angelscript/datetime/datetime.h>
#include <angelscript/scriptany/scriptany.h>
#include <angelscript/scriptarray/scriptarray.h>
#include <angelscript/scriptbuilder/scriptbuilder.h>
#include <angelscript/scriptdictionary/scriptdictionary.h>
#include <angelscript/scripthandle/scripthandle.h>
#include <angelscript/scripthelper/scripthelper.h>
#include <angelscript/scriptmath/scriptmath.h>
#include <angelscript/scriptstdstring/scriptstdstring.h>
#include <angelscript/weakref/weakref.h>

static pxly_machine * machine_inst = nullptr;

pxly_machine * pxly_new_machine() {
    bool has_keyboard = false;
    bool has_mouse    = false;
    bool has_gamepad  = false;

    pxly_assert(al_init(), []() { return "pxly_new_machine: Failed to initialize Allegro 5"; });
    pxly_assert(al_init_image_addon(), []() { return "pxly_new_machine: Failed to initialize Allegro 5 Image addon"; });
    pxly_assert(al_init_font_addon(), []() { return "pxly_new_machine: Failed to initialize Allegro 5 Font addon"; });
    pxly_assert(al_init_ttf_addon(), []() { return "pxly_new_machine: Failed to initialize Allegro 5 TTF addon"; });
    pxly_assert(al_init_primitives_addon(),
                []() { return "pxly_new_machine: Failed to initialize Allegro 5 Primitives addon"; });

    if (!al_init()) return nullptr;
    if (!al_init_image_addon()) return nullptr;
    if (!al_init_font_addon()) return nullptr;
    if (!al_init_ttf_addon()) return nullptr;
    if (!al_init_primitives_addon()) return nullptr;

    has_keyboard = al_install_keyboard();
    has_mouse    = al_install_mouse();
    has_gamepad  = al_install_joystick();

    pxly_assert(has_keyboard || has_mouse || has_gamepad, []() {
        return "pxly_new_machine: Failed to initialize an input method. Pixelly requires some kind of input method.";
    });

    al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_PROGRAMMABLE_PIPELINE);
    auto display = al_create_display(800, 600);
    pxly_assert(display, []() { return "pxly_new_machine: Failed to create display."; });
    auto eq = al_create_event_queue();
    pxly_assert(eq, []() { return "pxly_new_machine: Failed to create event queue."; });
    auto rtimer = al_create_timer(ALLEGRO_BPS_TO_SECS(60));
    pxly_assert(rtimer, []() { return "pxly_new_machine: Failed to create render timer."; });

    al_register_event_source(eq, al_get_display_event_source(display));
    al_register_event_source(eq, al_get_keyboard_event_source());
    al_register_event_source(eq, al_get_mouse_event_source());
    al_register_event_source(eq, al_get_joystick_event_source());
    al_register_event_source(eq, al_get_timer_event_source(rtimer));

    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_CONVERT_BITMAP);
    auto rtex = al_create_bitmap(PXLY_GPU_PIXEL_WIDTH, PXLY_GPU_PIXEL_HEIGHT);
    pxly_assert(rtex, []() { return "pxly_new_machine: Failed to create render texture."; });

    auto font = al_create_builtin_font();
    pxly_assert(font, []() { return "pxly_new_machine: Failed to load font."; });

    auto machine = new pxly_machine();
    pxly_assert(machine, []() { return "pxly_new_machine: Failed to create Pixelly machine."; });

    machine->m_display        = display;
    machine->m_event_queue    = eq;
    machine->m_render_timer   = rtimer;
    machine->m_render_texture = rtex;
    machine->m_font           = font;

    pxly_init_gpu(&machine->m_gpu);

    machine->m_running            = true;
    machine->m_exit_status        = 0;
    machine->m_script_engine      = nullptr;
    machine->m_script_engine_cart = nullptr;
    machine->m_script_error       = false;
    machine->m_script_context     = nullptr;

    pxly_assert(pxly_init_script_engine(machine),
                []() { return "pxly_new_machine: Failed to create scripting engine."; });

    machine_inst = machine;

    return machine;
}

void pxly_free_machine(pxly_machine * machine) {
    machine->m_script_context->Release();
    machine->m_script_engine->ShutDownAndRelease();

    al_destroy_bitmap(machine->m_render_texture);
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

    if (!machine->m_running) { return false; }

    pxly_render_machine(machine);
    return true;
}

void pxly_script_engine_callback(const asSMessageInfo * msg, void * param);
int  pxly_script_execute_with_timeout(pxly_machine * machine, asIScriptContext * ctx);
void pxly_script_line_callback(asIScriptContext * ctx, double * timeOut);

void pxly_update_machine(pxly_machine * machine) {
    if (machine->m_script_context != nullptr) {
        auto ret = pxly_script_execute_with_timeout(machine, machine->m_script_context);
        if (ret == asCONTEXT_NOT_PREPARED) {
            fmt::print(std::cerr, "pxly_update_machine: Script returned asCONTEXT_NOT_PREPARED\n");
            machine->m_running = false;
            return;
        } else if (ret == asEXECUTION_ABORTED) {
            fmt::print(std::cerr, "pxly_update_machine: Script returned asEXECUTION_ABORTED\n");
            machine->m_running = false;
            return;
        } else if (ret == asEXECUTION_FINISHED) {
            machine->m_running = false;
            return;
        } else if (ret == asEXECUTION_EXCEPTION) {
            fmt::print(std::cerr,
                       "Script Exception: {} : {} : {}\n",
                       machine->m_script_context->GetExceptionString(),
                       machine->m_script_context->GetExceptionFunction()->GetName(),
                       machine->m_script_context->GetExceptionLineNumber());
            machine->m_running = false;
            return;
        } else if (ret < 0) {
            fmt::print(std::cerr, "pxly_update_machine: Script returned an unknown error: {}\n", ret);
            machine->m_running = false;
            return;
        }
    }
}

void pxly_render_machine(pxly_machine * machine) {
    al_set_target_backbuffer(machine->m_display);
    al_clear_to_color(al_map_rgb(32, 32, 32));
    pxly_gpu_render(&machine->m_gpu, machine);
    al_flip_display();
}

void pxly_script_engine_callback(const asSMessageInfo * msg, void * param) {
    if (msg->type == asMSGTYPE_WARNING) {
        fmt::print(std::clog, "{} ({}, {}) : {} : {}\n", msg->section, msg->row, msg->col, "WARNING", msg->message);
    } else if (msg->type == asMSGTYPE_ERROR) {
        fmt::print(std::cerr, "{} ({}, {}) : {} : {}\n", msg->section, msg->row, msg->col, "ERROR  ", msg->message);
        ((pxly_machine *)param)->m_script_error = true;
    }
}

int pxly_script_execute_with_timeout(pxly_machine * machine, asIScriptContext * ctx) {
    auto timeOut = al_get_time() + ALLEGRO_BPS_TO_SECS(60);
    ctx->SetLineCallback(asFUNCTION(pxly_script_line_callback), &timeOut, asCALL_CDECL);
    int status = ctx->Execute();
    return status;
}

void pxly_script_line_callback(asIScriptContext * ctx, double * timeOut) {
    if (*timeOut < al_get_time()) { ctx->Suspend(); }
}

void pxly_script_api_print(const std::string & s) { fmt::print("{}", s); }

void pxly_script_api_exit(int status) {
    machine_inst->m_running     = false;
    machine_inst->m_exit_status = status;
}

void pxly_script_api_gpu_text_set_cursor_x(int new_csr_x) {
    pxly_gpu_text_set_cursor_x(&machine_inst->m_gpu.text_mode, new_csr_x);
}

void pxly_script_api_gpu_text_set_cursor_y(int new_csr_y) {
    pxly_gpu_text_set_cursor_y(&machine_inst->m_gpu.text_mode, new_csr_y);
}

void pxly_script_api_gpu_text_set_cursor_pos(int new_csr_x, int new_csr_y) {
    pxly_gpu_text_set_cursor_position(&machine_inst->m_gpu.text_mode, new_csr_x, new_csr_y);
}

void pxly_script_api_gpu_text_enable_cursor(bool enable) {
    pxly_gpu_text_enable_cursor(&machine_inst->m_gpu.text_mode, enable);
}

void pxly_script_api_gpu_text_set_cursor_color(pxly_color color) {
    pxly_gpu_text_set_cursor_color(&machine_inst->m_gpu.text_mode, color);
}

void pxly_script_api_gpu_text_set_cursor_char(char ch) {
    pxly_gpu_text_set_cursor_char(&machine_inst->m_gpu.text_mode, ch);
}

void pxly_script_api_gpu_text_put_char(int x, int y, pxly_color fg, pxly_color bg, char ch) {
    pxly_gpu_text_put_char(&machine_inst->m_gpu.text_mode, x, y, pxly_gpu_text_char{fg, bg, ch});
}

void pxly_script_api_gpu_text_put_string(int x, int y, pxly_color fg, pxly_color bg, const std::string & str) {
    pxly_gpu_text_put_string(&machine_inst->m_gpu.text_mode, x, y, fg, bg, str.c_str());
}

void pxly_script_api_gpu_text_scroll_up(int lines, pxly_color fg, pxly_color bg, char ch) {
    pxly_gpu_text_scroll_up(&machine_inst->m_gpu.text_mode, lines, pxly_gpu_text_char{fg, bg, ch});
}

void pxly_script_api_gpu_text_scroll_down(int lines, pxly_color fg, pxly_color bg, char ch) {
    pxly_gpu_text_scroll_down(&machine_inst->m_gpu.text_mode, lines, pxly_gpu_text_char{fg, bg, ch});
}

void pxly_script_api_gpu_text_scroll_left(int cols, pxly_color fg, pxly_color bg, char ch) {
    pxly_gpu_text_scroll_left(&machine_inst->m_gpu.text_mode, cols, pxly_gpu_text_char{fg, bg, ch});
}

void pxly_script_api_gpu_text_scroll_right(int cols, pxly_color fg, pxly_color bg, char ch) {
    pxly_gpu_text_scroll_right(&machine_inst->m_gpu.text_mode, cols, pxly_gpu_text_char{fg, bg, ch});
}

void pxly_script_api_gpu_text_cls(pxly_color fg, pxly_color bg, char ch) {
    pxly_gpu_text_cls(&machine_inst->m_gpu.text_mode, pxly_gpu_text_char{fg, bg, ch});
}

char pxly_script_api_gpu_text_get_char(int x, int y) {
    return pxly_gpu_text_get_char(&machine_inst->m_gpu.text_mode, x, y).ch;
}

pxly_color pxly_script_api_gpu_text_get_fg(int x, int y) {
    return pxly_gpu_text_get_char(&machine_inst->m_gpu.text_mode, x, y).fg;
}

pxly_color pxly_script_api_gpu_text_get_bg(int x, int y) {
    return pxly_gpu_text_get_char(&machine_inst->m_gpu.text_mode, x, y).bg;
}

int16_t pxly_script_api_gpu_text_get_cursor_x() { return pxly_gpu_text_get_cursor_x(&machine_inst->m_gpu.text_mode); }

int16_t pxly_script_api_gpu_text_get_cursor_y() { return pxly_gpu_text_get_cursor_y(&machine_inst->m_gpu.text_mode); }

void pxly_script_api_gpu_text_get_cursor_pos(int16_t & x, int16_t & y) {
    pxly_gpu_text_get_cursor_pos(&machine_inst->m_gpu.text_mode, &x, &y);
}

bool pxly_script_api_input_keyb_key(int key) {
    return pxly_input_keyb_key(&machine_inst->m_keyb, key);
}

bool pxly_script_api_input_keyb_keyp(int key) {
    return pxly_input_keyb_keyp(&machine_inst->m_keyb, key);
}

bool pxly_init_script_engine(pxly_machine * machine) {
    machine->m_script_error  = false;
    machine->m_script_engine = asCreateScriptEngine();
    RegisterScriptArray(machine->m_script_engine, true);
    RegisterStdString(machine->m_script_engine);
    RegisterStdStringUtils(machine->m_script_engine);
    RegisterScriptAny(machine->m_script_engine);
    RegisterScriptHandle(machine->m_script_engine);
    RegisterScriptWeakRef(machine->m_script_engine);
    RegisterScriptDictionary(machine->m_script_engine);
    RegisterScriptMath(machine->m_script_engine);
    RegisterScriptDateTime(machine->m_script_engine);
    RegisterExceptionRoutines(machine->m_script_engine);

    pxly_assert(
      machine->m_script_engine->SetMessageCallback(asFUNCTION(pxly_script_engine_callback), machine, asCALL_CDECL) >= 0,
      []() { return "pxly_init_script_engine: Failed to set message callback."; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void exit(int)", asFUNCTION(pxly_script_api_exit), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: Failed to bind 'void exit(int)'"; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void print(const string &in)", asFUNCTION(pxly_script_api_print), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });

    auto default_ns = machine->m_script_engine->GetDefaultNamespace();

    machine->m_script_engine->SetDefaultNamespace("gpu::text");
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void csrx(int)", asFUNCTION(pxly_script_api_gpu_text_set_cursor_x), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void csry(int)", asFUNCTION(pxly_script_api_gpu_text_set_cursor_y), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void csrpos(int, int)", asFUNCTION(pxly_script_api_gpu_text_set_cursor_pos), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void csron(bool)", asFUNCTION(pxly_script_api_gpu_text_enable_cursor), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void csrcol(uint8)", asFUNCTION(pxly_script_api_gpu_text_set_cursor_color), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void csrch(uint8)", asFUNCTION(pxly_script_api_gpu_text_set_cursor_char), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("void putch(int, int, uint8, uint8, uint8)",
                                                                 asFUNCTION(pxly_script_api_gpu_text_put_char),
                                                                 asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(
      machine->m_script_engine->RegisterGlobalFunction("void putstr(int, int, uint8, uint8, const string &in)",
                                                       asFUNCTION(pxly_script_api_gpu_text_put_string),
                                                       asCALL_CDECL) >= 0,
      []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("void scrollu(int, uint8, uint8, uint8)",
                                                                 asFUNCTION(pxly_script_api_gpu_text_scroll_up),
                                                                 asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("void scrolld(int, uint8, uint8, uint8)",
                                                                 asFUNCTION(pxly_script_api_gpu_text_scroll_down),
                                                                 asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("void scrolll(int, uint8, uint8, uint8)",
                                                                 asFUNCTION(pxly_script_api_gpu_text_scroll_left),
                                                                 asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("void scrollr(int, uint8, uint8, uint8)",
                                                                 asFUNCTION(pxly_script_api_gpu_text_scroll_right),
                                                                 asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "void cls(uint8, uint8, uint8)", asFUNCTION(pxly_script_api_gpu_text_cls), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "uint8 gchar(int x, int y)", asFUNCTION(pxly_script_api_gpu_text_get_char), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "uint8 gfg(int x, int y)", asFUNCTION(pxly_script_api_gpu_text_get_fg), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "uint8 gbg(int x, int y)", asFUNCTION(pxly_script_api_gpu_text_get_bg), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "int16 gcsrx()", asFUNCTION(pxly_script_api_gpu_text_get_cursor_x), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction(
                  "int16 gcsry()", asFUNCTION(pxly_script_api_gpu_text_get_cursor_y), asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("void gcsrpos(int16 &out, int16 &out)",
                                                                 asFUNCTION(pxly_script_api_gpu_text_get_cursor_pos),
                                                                 asCALL_CDECL) >= 0,
                []() { return "pxly_init_script_engine: "; });

    machine->m_script_engine->SetDefaultNamespace("kb");
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("bool key(int k)", asFUNCTION(pxly_script_api_input_keyb_key), asCALL_CDECL) >= 0,
                [] () {return "pxly_init_script_engine: Unable to bind function 'bool kb::key(int k)'"; });
    pxly_assert(machine->m_script_engine->RegisterGlobalFunction("bool keyp(int k)", asFUNCTION(pxly_script_api_input_keyb_keyp), asCALL_CDECL) >= 0,
                [] () {return "pxly_init_script_engine: Unable to bind function 'bool kb::keyp(int k)'"; });

    machine->m_script_engine->SetDefaultNamespace(default_ns);

    CScriptBuilder builder;
    pxly_assert(builder.StartNewModule(machine->m_script_engine, "boot") >= 0,
                []() { return "pxly_init_script_engine: Unable to start new module 'boot'"; });
    pxly_assert(builder.AddSectionFromFile("data/scripts/boot.as") >= 0,
                []() { return "pxly_init_script_engine: Unable to load script 'data/scripts/boot.as'"; });
    pxly_assert(builder.BuildModule() >= 0, []() { return "pxly_init_script_engine: Unable to build script module"; });

    auto mod  = machine->m_script_engine->GetModule("boot");
    auto func = mod->GetFunctionByDecl("void main()");
    pxly_assert(func, []() { return "pxly_init_script_engine: Unable to get function 'void main()'"; });

    auto ctx = machine->m_script_engine->CreateContext();
    pxly_assert(ctx, []() { return "pxly_init_script_engine: Unable to create script context"; });
    pxly_assert(ctx->Prepare(func) >= 0,
                []() { return "pxly_init_script_engine: Unable to prepare script function 'void main()'"; });

    machine->m_script_context = ctx;

    return true;
}
