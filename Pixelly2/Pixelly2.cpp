#include "machine.h"
#include <cassert>
#include <fmt/format.h>

int main() {
    fmt::print("Size of GPU: {} Bytes, {:.2f} KiB\n", sizeof(pxly_gpu), sizeof(pxly_gpu) / 1024.0);
    fmt::print("Size of Machine: {} Bytes, {:.2f} KiB\n", sizeof(pxly_machine), sizeof(pxly_machine) / 1024.0);

    auto machine = pxly_new_machine();
    pxly_assert(machine != nullptr, []() { return "Failed to create Pixelly machine"; });
    pxly_gpu_text_cls(&machine->m_gpu.text_mode, pxly_gpu_text_char{15, 0, ' '});
    pxly_gpu_text_put_string(&machine->m_gpu.text_mode, 0, 0, 15, 0, "Hello, World!");
    while (pxly_run_machine(machine)) {}
    pxly_free_machine(machine);

    return 0;
}
