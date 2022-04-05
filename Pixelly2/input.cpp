#include "input.h"
#include "base_intern.hpp"
#include <allegro5/allegro5.h>
#include <cstring>
#include <fmt/format.h>

bool pxly_get_bit(int index, uint8_t * arr, int size) {
    auto byte_index = index / 8;
    auto bit_index  = index % 8;

    pxly_assert(byte_index < size, [byte_index, size] () { return fmt::format("pxly_get_bit: byte_index ({}) out of bounds, array size {}", byte_index, size); });

    auto value = arr[byte_index];
    auto bit   = 1 & (value >> bit_index);
    return bit == 1;
}

void pxly_set_bit(int index, uint8_t * arr, int size, bool value) {
    auto byte_index = index / 8;
    auto bit_index  = index % 8;

    pxly_assert(byte_index < size, [byte_index, size] () { return fmt::format("pxly_set_bit: byte_index ({}) out of bounds, array size {}", byte_index, size); });

    auto ovalue     = arr[byte_index];
    auto mask       = ~(1 << bit_index);
    ovalue          = (ovalue & mask) | ((value ? 1 : 0) << bit_index);
    arr[byte_index] = ovalue;
}

void pxly_input_keyb_init(pxly_input_keyboard * keyb) {
    memset(keyb->keys_prev, 0, sizeof(keyb->keys_prev));
    memset(keyb->keys_cur, 0, sizeof(keyb->keys_cur));
}

void pxly_input_keyb_start_frame(pxly_input_keyboard * keyb) {
    memcpy(keyb->keys_prev, keyb->keys_cur, sizeof(keyb->keys_prev));
}

void pxly_input_keyb_handle_event(pxly_input_keyboard * keyb, void * evt) {
    ALLEGRO_EVENT * evtal = (ALLEGRO_EVENT *)evt;
    switch (evtal->type) {
    case ALLEGRO_EVENT_KEY_DOWN:
        pxly_set_bit(evtal->keyboard.keycode, keyb->keys_cur, sizeof(keyb->keys_cur), true);
        break;
    case ALLEGRO_EVENT_KEY_UP:
        pxly_set_bit(evtal->keyboard.keycode, keyb->keys_cur, sizeof(keyb->keys_cur), false);
        break;
    }
}

bool pxly_input_keyb_key(pxly_input_keyboard * keyb, int key) {
    return pxly_get_bit(key, keyb->keys_cur, sizeof(keyb->keys_cur));
}

bool pxly_input_keyb_keyp(pxly_input_keyboard * keyb, int key) {
    return pxly_get_bit(key, keyb->keys_prev, sizeof(keyb->keys_prev));
}
