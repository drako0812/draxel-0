#include "SFML/Window/Keyboard.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <fmt/format.h>

#include <sol/sol.hpp>

#include <cmath>
#include <iostream>
#include <memory>

#include "base.hpp"
#include "machine.hpp"

int main() {
    sol::state lua;
    auto machine = std::make_shared<pxly::Machine>();
    pxly_assert(machine->Initialize(), "Failed to create Pixelly machine");
    lua.set_function("cls", [&](){machine->m_Gpu.m_TextMode.ClearScreen(pxly::TextChar{0xF, 0x0, '.'});});
    sol::function cls = lua["cls"];
    machine->m_Gpu.m_TextMode.SetCursorPosition(10, 10);
    machine->m_Gpu.m_TextMode.PutString(0, 0, 0x9, 0x0, "Hello, World!");
    while (machine->Run()) {
        // A pressed last frame
        if (machine->m_Kb.KeyP(sf::Keyboard::A)) {
            machine->m_Gpu.m_TextMode.PutChar(0, 1, pxly::TextChar{0x9, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(0, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        // A pressed this frame
        if (machine->m_Kb.Key(sf::Keyboard::A)) {
            machine->m_Gpu.m_TextMode.PutChar(1, 1, pxly::TextChar{0xA, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(1, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        // A just released
        if (machine->m_Kb.KeyP(sf::Keyboard::A) && !machine->m_Kb.Key(sf::Keyboard::A)) {
            machine->m_Gpu.m_TextMode.PutChar(2, 1, pxly::TextChar{0xB, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(2, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        // A just pressed
        if (!machine->m_Kb.KeyP(sf::Keyboard::A) && machine->m_Kb.Key(sf::Keyboard::A)) {
            machine->m_Gpu.m_TextMode.PutChar(3, 1, pxly::TextChar{0xC, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(3, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        // A held
        if (machine->m_Kb.KeyP(sf::Keyboard::A) && machine->m_Kb.Key(sf::Keyboard::A)) {
            machine->m_Gpu.m_TextMode.PutChar(4, 1, pxly::TextChar{0xD, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(4, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        if(machine->m_Kb.KeyP(sf::Keyboard::C)) {
            cls();
        }
    }
    machine = nullptr;

    return EXIT_SUCCESS;
}
