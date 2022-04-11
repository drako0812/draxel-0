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
    auto machine = std::make_shared<pxly::Machine>();
    pxly_assert(machine->Initialize(), "Failed to create Pixelly machine");
    while (machine->Run()) {}
    machine = nullptr;

    return EXIT_SUCCESS;
}
