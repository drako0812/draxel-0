#include <iostream>
#include <memory>
#include "base.hpp"
#include "machine.hpp"

int main() {
    auto machine = std::make_shared<pxly::Machine>();
    pxly_assert(machine->Initialize(), "Failed to create Pixelly machine");
    machine->m_Gpu.m_TextMode.PutString(0, 0, 0x9, 0x0, "Hello, World!");
    while(machine->Run()) {

        if(machine->m_Kb.KeyP(ALLEGRO_KEY_A)) {
            machine->m_Gpu.m_TextMode.PutChar(0, 1, pxly::TextChar{0x9, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(0, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        if(machine->m_Kb.Key(ALLEGRO_KEY_A)) {
            machine->m_Gpu.m_TextMode.PutChar(1, 1, pxly::TextChar{0xA, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(1, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        if(machine->m_Kb.KeyP(ALLEGRO_KEY_A) && !machine->m_Kb.Key(ALLEGRO_KEY_A)) {
            machine->m_Gpu.m_TextMode.PutChar(2, 1, pxly::TextChar{0xB, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(2, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        if(!machine->m_Kb.KeyP(ALLEGRO_KEY_A) && machine->m_Kb.Key(ALLEGRO_KEY_A)) {
            machine->m_Gpu.m_TextMode.PutChar(3, 1, pxly::TextChar{0xC, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(3, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

        if(machine->m_Kb.KeyP(ALLEGRO_KEY_A) && machine->m_Kb.Key(ALLEGRO_KEY_A)) {
            machine->m_Gpu.m_TextMode.PutChar(4, 1, pxly::TextChar{0xD, 0x0, 'A'});
        } else {
            machine->m_Gpu.m_TextMode.PutChar(4, 1, pxly::TextChar{0xF, 0x0, ' '});
        }

    }
    machine = nullptr;

    return EXIT_SUCCESS;
}
