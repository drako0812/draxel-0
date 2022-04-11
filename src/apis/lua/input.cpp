#include "machine.hpp"

namespace pxly {

    void RegisterLuaAPI_input(Machine & machine) {
        auto & lua    = machine.m_Lua;
        auto   mod_kb = lua.create_named_table("kb");
        mod_kb.set_function("keyp", [&machine](int key) {
            //machine.synchronize();
            return machine.m_Kb.KeyP(key);
        });
        mod_kb.set_function("key", [&machine](int key) {
            //machine.synchronize();
            return machine.m_Kb.Key(key);
        });
        mod_kb.set_function("peekch", [&machine]() {
            if (machine.m_IKb.m_CharPresses.size() == 0) { /*synchronize();*/
                return '\0';
            }
            auto ret = machine.m_IKb.m_CharPresses.front();
            /*synchronize();*/
            return ret;
        });
        mod_kb.set_function("keych", [&machine]() {
            if (machine.m_IKb.m_CharPresses.size() == 0) { /*synchronize();*/
                return '\0';
            }
            auto ret = machine.m_IKb.m_CharPresses.front();
            machine.m_IKb.m_CharPresses.pop_front();
            /*synchronize();*/
            return ret;
        });
        mod_kb.set_function("flush", [&machine]() {
            machine.m_IKb.m_CharPresses.clear();
            /*synchronize();*/
        });
    }

} // namespace pxly
