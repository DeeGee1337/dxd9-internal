#pragma once

#include "hack.h"

void draw_fill_rect(int x, int y, int w, int h, D3DCOLOR col);
void draw_fill_rect_transparent(int x, int y, int w, int h, D3DCOLOR color);
void draw_line(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color);
void draw_line(Vec2 src, Vec2 dst, int thickness, D3DCOLOR color);
void draw_crosshair(int x, int y, int w, int h, D3DCOLOR color);
void draw_esp_box_2d(Vec2 top, Vec2 bot, int thickness, D3DCOLOR color);
void draw_esp_cornerbox_2d(Vec2 top, Vec2 bot, int thickness, D3DCOLOR inlinecolor, D3DCOLOR OutLine);
void draw_text(const char* text, float x, float y, D3DCOLOR color);