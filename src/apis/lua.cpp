#include "machine.hpp"
#include <map>
#include <tuple>
#include <vector>

namespace pxly {

    void Machine::registerLuaAPI() {
        auto & lua = m_Lua;

        lua.open_libraries(sol::lib::base,
                           sol::lib::string,
                           sol::lib::math,
                           sol::lib::table,
                           sol::lib::bit32,
                           sol::lib::utf8,
                           sol::lib::os,
                           sol::lib::io,
                           sol::lib::package,
                           sol::lib::debug,
                           sol::lib::coroutine);

        auto mod_gpu      = lua.create_named_table("gpu");
        auto mod_gpu_text = mod_gpu.create_named("text");
        auto mod_kb       = lua.create_named_table("kb");
        auto mod_intern   = lua.create_named_table("intern");

        lua.set_function("sync", [this]() { synchronize(); });

        mod_kb.set_function("keyp", [this](int key) {
            synchronize();
            return m_Kb.KeyP(key);
        });
        mod_kb.set_function("key", [this](int key) {
            synchronize();
            return m_Kb.Key(key);
        });
        mod_kb.set_function("peekch", [this]() {
            if (m_IKb.m_CharPresses.size() == 0) { /*synchronize();*/ return '\0'; }
            auto ret = m_IKb.m_CharPresses.front();
            /*synchronize();*/
            return ret;
        });
        mod_kb.set_function("keych", [this]() {
            if (m_IKb.m_CharPresses.size() == 0) { /*synchronize();*/ return '\0'; }
            auto ret = m_IKb.m_CharPresses.front();
            m_IKb.m_CharPresses.pop_front();
            /*synchronize();*/
            return ret;
        });
        mod_kb.set_function("flush", [this]() {
            m_IKb.m_CharPresses.clear();
            /*synchronize();*/
        });

        mod_gpu_text.set_function("csrx",
                                  sol::overload(
                                    [this](int x) {
                                        m_Gpu.m_TextMode.SetCursorX(x);
                                        synchronize();
                                    },
                                    [this]() {
                                        synchronize();
                                        return m_Gpu.m_TextMode.GetCursorX();
                                    }));
        mod_gpu_text.set_function("csry",
                                  sol::overload(
                                    [this](int y) {
                                        m_Gpu.m_TextMode.SetCursorY(y);
                                        synchronize();
                                    },
                                    [this]() {
                                        synchronize();
                                        return m_Gpu.m_TextMode.GetCursorY();
                                    }));
        mod_gpu_text.set_function("csrpos",
                                  sol::overload(
                                    [this](int x, int y) {
                                        m_Gpu.m_TextMode.SetCursorPosition(x, y);
                                        synchronize();
                                    },
                                    [this]() -> std::tuple<int, int> {
                                        synchronize();
                                        int16 x, y;
                                        m_Gpu.m_TextMode.GetCursorPosition(x, y);
                                        return {x, y};
                                    }));
        mod_gpu_text.set_function("csren",
                                  sol::overload(
                                    [this](bool enable) {
                                        m_Gpu.m_TextMode.EnableCursor(enable);
                                        synchronize();
                                    },
                                    [this]() {
                                        synchronize();
                                        return m_Gpu.m_TextMode.IsCursorEnabled();
                                    }));
        mod_gpu_text.set_function("csrcol",
                                  sol::overload(
                                    [this](int color) {
                                        m_Gpu.m_TextMode.SetCursorColor(color);
                                        synchronize();
                                    },
                                    [this]() {
                                        synchronize();
                                        return m_Gpu.m_TextMode.GetCursorColor();
                                    }));
        mod_gpu_text.set_function("csrch",
                                  sol::overload(
                                    [this](char ch) {
                                        m_Gpu.m_TextMode.SetCursorChar(ch);
                                        synchronize();
                                    },
                                    [this]() {
                                        synchronize();
                                        return m_Gpu.m_TextMode.GetCursorChar();
                                    }));
        mod_gpu_text.set_function("putc", [this](int x, int y, int fg, int bg, char ch) {
            m_Gpu.m_TextMode.PutChar(x, y, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            synchronize();
        });
        mod_gpu_text.set_function("puts", [this](int x, int y, int fg, int bg, const std::string & str) {
            m_Gpu.m_TextMode.PutString(x, y, fg, bg, str);
            synchronize();
        });
        mod_gpu_text.set_function("scrollu", [this](int lines, int fg, int bg, char ch) {
            m_Gpu.m_TextMode.ScrollUp(lines, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            synchronize();
        });
        mod_gpu_text.set_function("scrolld", [this](int lines, int fg, int bg, char ch) {
            m_Gpu.m_TextMode.ScrollDown(lines, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            synchronize();
        });
        mod_gpu_text.set_function("scrolll", [this](int cols, int fg, int bg, char ch) {
            m_Gpu.m_TextMode.ScrollLeft(cols, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            synchronize();
        });
        mod_gpu_text.set_function("scrollr", [this](int cols, int fg, int bg, char ch) {
            m_Gpu.m_TextMode.ScrollRight(cols, TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            synchronize();
        });
        mod_gpu_text.set_function("cls", [this](int fg, int bg, char ch) {
            m_Gpu.m_TextMode.ClearScreen(TextChar{static_cast<uint8>(fg), static_cast<uint8>(bg), ch});
            synchronize();
        });
        mod_gpu_text.set_function("getch", [this](int x, int y) -> std::tuple<int, int, char> {
            synchronize();
            auto c = m_Gpu.m_TextMode.GetChar(x, y);
            return {static_cast<int>(c.fg), static_cast<int>(c.bg), c.ch};
        });

        mod_intern.set_function("get_cwd", [this]() {
            synchronize();
            return Cwd();
        });
        mod_intern.set_function("set_cwd", [this](const std::string & path) {
            synchronize();
            return Cwd(path);
        });
    }

} // namespace pxly
