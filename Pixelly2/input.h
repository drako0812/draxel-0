#pragma once

#include "base.h"
#include <stdint.h>

struct pxly_machine;

typedef struct pxly_input_keyboard {
    uint8_t keys_prev[256/8];
    uint8_t keys_cur[256/8];
};

PXLY_API bool pxly_get_bit(int index, uint8_t * arr, int size);
PXLY_API void pxly_set_bit(int index, uint8_t * arr, int size, bool value);

PXLY_API void pxly_input_keyb_init(pxly_input_keyboard * keyb);
PXLY_API void pxly_input_keyb_start_frame(pxly_input_keyboard * keyb);
PXLY_API void pxly_input_keyb_handle_event(pxly_input_keyboard * keyb, void * evt);

PXLY_API bool pxly_input_keyb_key(pxly_input_keyboard * keyb, int key);
PXLY_API bool pxly_input_keyb_keyp(pxly_input_keyboard * keyb, int key);
