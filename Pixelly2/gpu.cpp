#include "gpu.h"
#include "machine.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstring>

void pxly_init_gpu_text(pxly_gpu_text * gpu_text) {
    gpu_text->cursor_x       = 0;
    gpu_text->cursor_y       = 0;
    gpu_text->cursor_char    = '_';
    gpu_text->cursor_color   = 15;
    gpu_text->cursor_enabled = true;

    for (int i = 0; i < PXLY_GPU_TEXT_BUFFER_COUNT; i++) { gpu_text->buffer[i] = pxly_gpu_text_char{15, 0, ' '}; }
}

void pxly_gpu_text_set_cursor_x(pxly_gpu_text * gpu_text, int16_t new_cursor_x) { gpu_text->cursor_x = new_cursor_x; }

void pxly_gpu_text_set_cursor_y(pxly_gpu_text * gpu_text, int16_t new_cursor_y) { gpu_text->cursor_y = new_cursor_y; }

void pxly_gpu_text_set_cursor_position(pxly_gpu_text * gpu_text, int16_t new_cursor_x, int16_t new_cursor_y) {
    gpu_text->cursor_x = new_cursor_x;
    gpu_text->cursor_y = new_cursor_y;
}

void pxly_gpu_text_enable_cursor(pxly_gpu_text * gpu_text, bool enable) { gpu_text->cursor_enabled = enable; }

void pxly_gpu_text_set_cursor_color(pxly_gpu_text * gpu_text, pxly_color color) { gpu_text->cursor_color = color; }

void pxly_gpu_text_set_cursor_char(pxly_gpu_text * gpu_text, char ch) { gpu_text->cursor_char = ch; }

void pxly_gpu_text_put_char(pxly_gpu_text * gpu_text, int16_t x, int16_t y, pxly_gpu_text_char ch) {
    if ((x < 0) || (x >= PXLY_GPU_TEXT_CELLS_WIDTH) || (y < 0) || (y >= PXLY_GPU_TEXT_CELLS_HEIGHT)) { return; }
    int16_t index           = x + (y * PXLY_GPU_TEXT_CELLS_WIDTH);
    gpu_text->buffer[index] = ch;
}

void pxly_gpu_text_put_string(
  pxly_gpu_text * gpu_text, int16_t x, int16_t y, pxly_color fg, pxly_color bg, const char * str) {
    auto len = std::strlen(str);
    for (int i = 0; i < len; i++) {
        auto ch = pxly_gpu_text_char{fg, bg, str[i]};
        pxly_gpu_text_put_char(gpu_text, x + i, y, ch);
    }
}

void pxly_gpu_text_scroll_up(pxly_gpu_text * gpu_text, int16_t lines, pxly_gpu_text_char fill_ch) {
    if (lines >= PXLY_GPU_TEXT_CELLS_HEIGHT) {
        pxly_gpu_text_cls(gpu_text, fill_ch);
        return;
    }

    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT - lines; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) {
            int  src_y = y + lines;
            auto ch    = pxly_gpu_text_get_char(gpu_text, x, src_y);
            pxly_gpu_text_put_char(gpu_text, x, y, ch);
        }
    }

    /*memmove(gpu_text->buffer,
            gpu_text->buffer + (lines * PXLY_GPU_TEXT_CELLS_WIDTH * sizeof(pxly_gpu_text_char)),
            (PXLY_GPU_TEXT_CELLS_HEIGHT - lines) * PXLY_GPU_TEXT_CELLS_WIDTH * sizeof(pxly_gpu_text_char));*/

    for (int y = PXLY_GPU_TEXT_CELLS_HEIGHT - lines; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) { pxly_gpu_text_put_char(gpu_text, x, y, fill_ch); }
    }
}

void pxly_gpu_text_scroll_down(pxly_gpu_text * gpu_text, int16_t lines, pxly_gpu_text_char fill_ch) {
    if (lines >= PXLY_GPU_TEXT_CELLS_HEIGHT) {
        pxly_gpu_text_cls(gpu_text, fill_ch);
        return;
    }

    for (int y = PXLY_GPU_TEXT_CELLS_HEIGHT - lines; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) {
            int  src_y = y - lines;
            auto ch    = pxly_gpu_text_get_char(gpu_text, x, src_y);
            pxly_gpu_text_put_char(gpu_text, x, y, ch);
        }
    }

    /*memmove(gpu_text->buffer + (lines * PXLY_GPU_TEXT_CELLS_WIDTH * sizeof(pxly_gpu_text_char)),
            gpu_text->buffer,
            (PXLY_GPU_TEXT_CELLS_HEIGHT - lines) * PXLY_GPU_TEXT_CELLS_WIDTH * sizeof(pxly_gpu_text_char));*/

    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT - lines; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) { pxly_gpu_text_put_char(gpu_text, x, y, fill_ch); }
    }
}

void pxly_gpu_text_scroll_left(pxly_gpu_text * gpu_text, int16_t cols, pxly_gpu_text_char fill_ch) {
    if (cols >= PXLY_GPU_TEXT_CELLS_WIDTH) {
        pxly_gpu_text_cls(gpu_text, fill_ch);
        return;
    }

    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH - cols; x++) {
            int  src_x = x + cols;
            auto ch    = pxly_gpu_text_get_char(gpu_text, src_x, y);
            pxly_gpu_text_put_char(gpu_text, x, y, ch);
        }
    }

    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = PXLY_GPU_TEXT_CELLS_WIDTH - cols; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) {
            pxly_gpu_text_put_char(gpu_text, x, y, fill_ch);
        }
    }
}

void pxly_gpu_text_scroll_right(pxly_gpu_text * gpu_text, int16_t cols, pxly_gpu_text_char fill_ch) {
    if (cols >= PXLY_GPU_TEXT_CELLS_WIDTH) {
        pxly_gpu_text_cls(gpu_text, fill_ch);
        return;
    }

    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = PXLY_GPU_TEXT_CELLS_WIDTH - cols; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) {
            int  src_x = x - cols;
            auto ch    = pxly_gpu_text_get_char(gpu_text, src_x, y);
            pxly_gpu_text_put_char(gpu_text, x, y, ch);
        }
    }

    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH - cols; x++) { pxly_gpu_text_put_char(gpu_text, x, y, fill_ch); }
    }
}

void pxly_gpu_text_cls(pxly_gpu_text * gpu_text, pxly_gpu_text_char fill_ch) {
    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) { pxly_gpu_text_put_char(gpu_text, x, y, fill_ch); }
    }
}

pxly_gpu_text_char pxly_gpu_text_get_char(pxly_gpu_text * gpu_text, int16_t x, int16_t y) {
    if ((x < 0) || (x >= PXLY_GPU_TEXT_CELLS_WIDTH) || (y < 0) || (y >= PXLY_GPU_TEXT_CELLS_HEIGHT)) {
        return pxly_gpu_text_char{0, 0, '\0'};
    }
    int16_t index = x + (y * PXLY_GPU_TEXT_CELLS_WIDTH);
    return gpu_text->buffer[index];
}

ALLEGRO_COLOR   convert(const pxly_full_color & color) { return al_map_rgb(color.r, color.g, color.b); }
pxly_full_color convert(const ALLEGRO_COLOR & color) {
    uint8_t r, g, b;
    al_unmap_rgb(color, &r, &g, &b);
    return pxly_full_color{r, g, b};
}

void pxly_gpu_text_render(pxly_gpu_text * gpu_text, pxly_machine * machine) {
    al_hold_bitmap_drawing(true);
    for (int y = 0; y < PXLY_GPU_TEXT_CELLS_HEIGHT; y++) {
        for (int x = 0; x < PXLY_GPU_TEXT_CELLS_WIDTH; x++) {
            auto ch = pxly_gpu_text_get_char(gpu_text, x, y);
            int  xx = x * PXLY_GPU_TEXTURE_WIDTH;
            int  yy = y * PXLY_GPU_TEXTURE_HEIGHT;
            al_draw_filled_rectangle(xx,
                                     yy,
                                     xx + PXLY_GPU_TEXTURE_WIDTH,
                                     yy + PXLY_GPU_TEXTURE_HEIGHT,
                                     convert(machine->m_gpu.palettes[0].colors[ch.bg]));
            al_draw_glyph(machine->m_font, convert(machine->m_gpu.palettes[0].colors[ch.fg]), xx, yy, ch.ch);
        }
    }
    if (gpu_text->cursor_enabled) {
        al_draw_glyph(machine->m_font,
                      convert(machine->m_gpu.palettes[0].colors[gpu_text->cursor_color]),
                      gpu_text->cursor_x * PXLY_GPU_TEXTURE_WIDTH,
                      gpu_text->cursor_y * PXLY_GPU_TEXTURE_HEIGHT,
                      gpu_text->cursor_char);
    }
    al_hold_bitmap_drawing(false);
}

int16_t pxly_gpu_text_get_cursor_x(pxly_gpu_text * gpu_text) { return gpu_text->cursor_x; }

int16_t pxly_gpu_text_get_cursor_y(pxly_gpu_text * gpu_text) { return gpu_text->cursor_y; }

void pxly_gpu_text_get_cursor_pos(pxly_gpu_text * gpu_text, int16_t * x, int16_t * y) {
    *x = gpu_text->cursor_x;
    *y = gpu_text->cursor_y;
}

void pxly_init_gpu(pxly_gpu * gpu) {
    gpu->mode = PXLY_GPU_MODE_TEXT;
    memset(gpu->textures, 0, sizeof(gpu->textures));
    gpu->palettes[0].colors[0]  = {0, 0, 0};
    gpu->palettes[0].colors[1]  = {0x80, 0, 0};
    gpu->palettes[0].colors[2]  = {0x80, 0x80, 0};
    gpu->palettes[0].colors[3]  = {0, 0x80, 0};
    gpu->palettes[0].colors[4]  = {0, 0x80, 0x80};
    gpu->palettes[0].colors[5]  = {0, 0, 0x80};
    gpu->palettes[0].colors[6]  = {0x80, 0, 0x80};
    gpu->palettes[0].colors[7]  = {0xc0, 0xc0, 0xc0};
    gpu->palettes[0].colors[8]  = {0x40, 0x40, 0x40};
    gpu->palettes[0].colors[9]  = {0xf0, 0, 0};
    gpu->palettes[0].colors[10] = {0xf0, 0xf0, 0};
    gpu->palettes[0].colors[11] = {0, 0xf0, 0};
    gpu->palettes[0].colors[12] = {0, 0xf0, 0xf0};
    gpu->palettes[0].colors[13] = {0, 0, 0xf0};
    gpu->palettes[0].colors[14] = {0xf0, 0, 0xf0};
    gpu->palettes[0].colors[15] = {0xf0, 0xf0, 0xf0};
    pxly_init_gpu_text(&gpu->text_mode);
}

void pxly_gpu_render(pxly_gpu * gpu, pxly_machine * machine) {
    al_set_target_bitmap(machine->m_render_texture);
    pxly_gpu_text_render(&gpu->text_mode, machine);
    al_set_target_backbuffer(machine->m_display);
    al_draw_bitmap_region(machine->m_render_texture, 0, 0, PXLY_GPU_PIXEL_WIDTH, PXLY_GPU_PIXEL_HEIGHT, 0, 0, 0);
}
