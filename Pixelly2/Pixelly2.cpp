#include "machine.h"
#include <fmt/format.h>

int main() {
    fmt::print("Size of GPU: {} Bytes, {:.2f} KiB\n", sizeof(pxly_gpu), sizeof(pxly_gpu) / 1024.0);
    fmt::print("Size of Machine: {} Bytes, {:.2f} KiB\n", sizeof(pxly_machine), sizeof(pxly_machine) / 1024.0);

    auto machine = pxly_new_machine();
    if (machine) {
        while (pxly_run_machine(machine)) {}
    }
    pxly_free_machine(machine);

    return 0;
}
