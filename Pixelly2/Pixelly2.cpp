#include "machine.h"
#include <iostream>

int main() {
    auto machine = pxly_new_machine();
    if (machine) {
        while (pxly_run_machine(machine)) {}
    }
    pxly_free_machine(machine);

    return 0;
}
