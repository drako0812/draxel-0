#pragma once

#include "stdint.h"

#define PXLY_GPU_TEXT_CELLS_WIDTH 48
#define PXLY_GPU_TEXT_CELLS_HEIGHT 27
#define PXLY_GPU_TEXT_BUFFER_COUNT (PXLY_GPU_TEXT_CELLS_WIDTH * PXLY_GPU_TEXT_CELLS_HEIGHT)
#define PXLY_GPU_TEXT_BUFFER_BYTES (PXLY_GPU_TEXT_BUFFER_COUNT * 2)
#define PXLY_GPU_PIXEL_WIDTH 384
#define PXLY_GPU_PIXEL_HEIGHT 216
#define PXLY_GPU_PIXEL_BUFFER_COUNT (PXLY_GPU_PIXEL_WIDTH * PXLY_GPU_PIXEL_HEIGHT)
#define PXLY_GPU_PIXEL_SIZE 4
#define PXLY_GPU_PIXEL_BUFFER_BYTES ((PXLY_GPU_PIXEL_BUFFER_COUNT * PXLY_GPU_PIXEL_SIZE) / 8)
#define PXLY_GPU_PIXELS_TO_BYTES(__pixels__) ((__pixels__ * PXLY_GPU_PIXEL_SIZE) / 8)
#define PXLY_GPU_TEXTURE_WIDTH 8
#define PXLY_GPU_TEXTURE_HEIGHT 8
#define PXLY_GPU_TEXTURE_PAGES 16
#define PXLY_GPU_TEXTURES_PER_PAGE 256
#define PXLY_GPU_PALETTE_SIZE 16
#define PXLY_GPU_PALETTE_COUNT 1

typedef uint8_t pxly_color;

typedef struct pxly_gpu_text_char {
    pxly_color fg : 4;
    pxly_color bg : 4;
    char    ch;
} pxly_gpu_text_char;

typedef struct pxly_gpu_text {
    int16_t cursor_x;
    int16_t cursor_y;
    int16_t cursor_char;
    uint8_t cursor_enabled : 1;
    pxly_color cursor_color : 7;
    pxly_gpu_text_char buffer[PXLY_GPU_TEXT_BUFFER_COUNT];
} pxly_gpu_text;

typedef struct pxly_gpu_pixel {
    int16_t cursor_x;
    int16_t cursor_y;
    int16_t cursor_texture;
    uint8_t cursor_enabled;
    pxly_color buffer[PXLY_GPU_PIXEL_BUFFER_BYTES];
} pxly_gpu_pixel;

typedef struct pxly_gpu_texture {
    pxly_color pixels[PXLY_GPU_PIXELS_TO_BYTES(PXLY_GPU_TEXTURE_WIDTH * PXLY_GPU_TEXTURE_HEIGHT)];
} pxly_gpu_texture;

enum pxly_gpu_mode : uint8_t {
    PXLY_GPU_MODE_TEXT,
    PXLY_GPU_MODE_PIXEL,
};

typedef struct pxly_full_color {
    uint8_t r, g, b;
} pxly_full_color;

typedef struct pxly_palette {
    pxly_full_color colors[PXLY_GPU_PALETTE_SIZE];
} pxly_palette;

typedef struct pxly_gpu {
    pxly_gpu_mode mode;
    union {
        pxly_gpu_text text_mode;
        pxly_gpu_pixel pixel_mode;
    };
    pxly_gpu_texture textures[PXLY_GPU_TEXTURES_PER_PAGE * PXLY_GPU_TEXTURE_PAGES];
    pxly_palette     palettes[PXLY_GPU_PALETTE_COUNT];
} pxly_gpu;
