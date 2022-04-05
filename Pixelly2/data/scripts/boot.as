#include "bios.as"

void main() {
    bios::init();
    bios::log("Booting Pixelly.\n");

    while(true) {
        bios::print_str("Hello, World! ");

        //for(int i = 0; i < 1000000; i++) {}
    }

    exit(0);
}
