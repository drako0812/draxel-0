#include "machine.hpp"

namespace pxly {

    void RegisterLuaAPI_gpu(Machine & machine) {
        auto & lua          = machine.m_Lua;
        auto   mod_gpu      = lua.create_named_table("gpu");
        auto   mod_gpu_text = mod_gpu.create_named("text");

        mod_gpu_text.set_function("csrx",
                                  sol::overload(
                                    [&machine](int x) {
                                        machine.m_Gpu.m_TextMode.SetCursorX(x);
                                        machine.synchronize();
                                    },
                                    [&machine]() {
                                        machine.synchronize();
                                        return machine.m_Gpu.m_TextMode.GetCursorX();
                                    }));
        mod_gpu_text.set_function("csry",
                                  sol::overload(
                                    [&machine](int y) {
                                        machine.m_Gpu.m_TextMode.SetCursorY(y);
                                        machine.synchronize();
                                    },
                                    [&machine]() {
                                        machine.synchronize();
                                        return machine.m_Gpu.m_TextMode.GetCursorY();
                                    }));
        mod_gpu_text.set_function("csrpos",
                                  sol::overload(
                                    [&machine](int x, int y) {
                                        machine.m_Gpu.m_TextMode.SetCursorPosition(x, y);
                                        machine.synchronize();
                                    },
                                    [&machine]() -> std::tuple<int, int> {
                                        machine.synchronize();
                                        int16 x, y;
                                        machine.m_Gpu.m_TextMode.GetCursorPosition(x, y);
                                        return {x, y};
                                    }));
        mod_gpu_text.set_function("csren",
                                  sol::overload(
                                    [&machine](bool enable) {
                                        machine.m_Gpu.m_TextMode.EnableCursor(enable);
                                        machine.synchronize();
                                    },
                                    [&machine]() {
                                        machine.synchronize();
                                        return machine.m_Gpu.m_TextMode.IsCursorEnabled();
                                    }));
        mod_gpu_text.set_function("csrcol",
                                  sol::overload(
                                    [&machine](int color) {
                                        machine.m_Gpu.m_TextMode.SetCursorColor(color);
                                        machine.synchronize();
                                    },
                                    [&machine]() {
                                        machine.synchronize();
                                        return machine.m_Gpu.m_TextMode.GetCursorColor();
                                    }));
        mod_gpu_text.set_function("csrch",
                                  sol::overload(
                                    [&machine](char ch) {
                                        machine.m_Gpu.m_TextMode.SetCursorChar(ch);
                                        machine.synchronize();
                                    },
                                    [&machine]() {
                                        machine.synchronize();
                                        return machine.m_Gpu.m_TextMode.GetCursorChar();
                                    }));
        mod_gpu_text.set_function("putc", [&machine](int x, int y, int fg, int bg, char ch) {
            machine.m_Gpu.m_TextMode.PutChar(x, y, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            machine.synchronize();
        });
        mod_gpu_text.set_function("puts", [&machine](int x, int y, int fg, int bg, const std::string & str) {
            machine.m_Gpu.m_TextMode.PutString(x, y, fg, bg, str);
            machine.synchronize();
        });
        mod_gpu_text.set_function("scrollu", [&machine](int lines, int fg, int bg, char ch) {
            machine.m_Gpu.m_TextMode.ScrollUp(lines, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            machine.synchronize();
        });
        mod_gpu_text.set_function("scrolld", [&machine](int lines, int fg, int bg, char ch) {
            machine.m_Gpu.m_TextMode.ScrollDown(lines, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            machine.synchronize();
        });
        mod_gpu_text.set_function("scrolll", [&machine](int cols, int fg, int bg, char ch) {
            machine.m_Gpu.m_TextMode.ScrollLeft(cols, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            machine.synchronize();
        });
        mod_gpu_text.set_function("scrollr", [&machine](int cols, int fg, int bg, char ch) {
            machine.m_Gpu.m_TextMode.ScrollRight(cols, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            machine.synchronize();
        });
        mod_gpu_text.set_function("cls", [&machine](int fg, int bg, char ch) {
            machine.m_Gpu.m_TextMode.ClearScreen(TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            machine.synchronize();
        });
        mod_gpu_text.set_function("getch", [&machine](int x, int y) -> std::tuple<int, int, char> {
            machine.synchronize();
            auto c = machine.m_Gpu.m_TextMode.GetChar(x, y);
            return {static_cast<int>(c.fg), static_cast<int>(c.bg), c.ch};
        });
    }

} // namespace pxly
