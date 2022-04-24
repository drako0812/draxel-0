#include "machine.hpp"

#define KEY(__name__) #__name__, sf::Keyboard::__name__

namespace drxl {

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

        mod_kb.new_enum(
            "keycode",
            KEY(Unknown),
            KEY(A), KEY(B), KEY(C), KEY(D), KEY(E), KEY(F), KEY(G), KEY(H), KEY(I), KEY(J), KEY(K), KEY(L), KEY(M),
            KEY(N), KEY(O), KEY(P), KEY(Q), KEY(R), KEY(S), KEY(T), KEY(U), KEY(V), KEY(W), KEY(X), KEY(Y), KEY(Z),
            KEY(Num0), KEY(Num1), KEY(Num2), KEY(Num3), KEY(Num4), KEY(Num5), KEY(Num6), KEY(Num7), KEY(Num8), KEY(Num9),
            KEY(Escape), KEY(LControl), KEY(LShift), KEY(LAlt), KEY(LSystem), KEY(RControl), KEY(RShift), KEY(RAlt), KEY(RSystem),
            KEY(Menu), KEY(LBracket), KEY(RBracket), KEY(Semicolon), KEY(Comma), KEY(Period), KEY(Quote), KEY(Slash), KEY(Backslash),
            KEY(Tilde), KEY(Equal), KEY(Hyphen), KEY(Space), KEY(Enter), KEY(Backspace), KEY(Tab), KEY(PageUp), KEY(PageDown),
            KEY(End), KEY(Home), KEY(Insert), KEY(Delete), KEY(Add), KEY(Subtract), KEY(Multiply), KEY(Divide),
            KEY(Left), KEY(Right), KEY(Up), KEY(Down),
            KEY(Numpad0), KEY(Numpad1), KEY(Numpad2), KEY(Numpad3), KEY(Numpad4), KEY(Numpad5), KEY(Numpad6), KEY(Numpad7), KEY(Numpad8), KEY(Numpad9),
            KEY(F1), KEY(F2), KEY(F3), KEY(F4), KEY(F5), KEY(F6), KEY(F7), KEY(F8), 
            KEY(F9), KEY(F10), KEY(F11), KEY(F12), KEY(F13), KEY(F14), KEY(F15),
            KEY(Pause)
        );

        auto mod_gp = lua.create_named_table("gp");
        mod_gp.set_function("btnp", [&machine](int btn) {
            return machine.m_Gp.BtnP(btn);
        });
        mod_gp.set_function("btn", [&machine](int btn) {
            return machine.m_Gp.Btn(btn);
        });

        mod_gp.new_enum(
            "button",
            "Up", 0, "Down", 1, "Left", 2, "Right", 3,
            "A", 4, "B", 5, "X", 6, "Y", 7,
            "Start", 8, "Select", 9
        );

        auto mod_mouse = lua.create_named_table("mouse");
        mod_mouse.set_function("mxp", [&machine]() {
            return machine.m_Mouse.MxP();
        });
        mod_mouse.set_function("mx", [&machine]() {
            return machine.m_Mouse.Mx();
        });
        mod_mouse.set_function("myp", [&machine]() {
            return machine.m_Mouse.MyP();
        });
        mod_mouse.set_function("my", [&machine]() {
            return machine.m_Mouse.My();
        });
        mod_mouse.set_function("mbtnp", [&machine](int btn) {
            return machine.m_Mouse.MBtnP(btn);
        });
        mod_mouse.set_function("mbtn", [&machine](int btn) {
            return machine.m_Mouse.MBtn(btn);
        });
        mod_mouse.set_function("mscrollp", [&machine]() {
            return machine.m_Mouse.MScrollP();
        });
        mod_mouse.set_function("mscroll", [&machine]() {
            return machine.m_Mouse.MScroll();
        });

        mod_mouse.new_enum(
            "button",
            "Left", 0, "Right", 1, "Middle", 2, "X1", 3, "X2", 4
        );
    }   

} // namespace drxl

#undef KEY
