#pragma once
#include "../common/common.h"
#include <stdio.h>

#define RED FB_COLOR(255, 0, 0)
#define ORANGE FB_COLOR(255, 165, 0)
#define YELLOW FB_COLOR(255, 255, 0)
#define GREEN FB_COLOR(0, 255, 0)
#define CYAN FB_COLOR(0, 127, 255)
#define BLUE FB_COLOR(0, 0, 255)
#define PURPLE FB_COLOR(139, 0, 255)
#define WHITE FB_COLOR(255, 255, 255)
#define BLACK FB_COLOR(0, 0, 0)

typedef struct
{
  int x;     // x in img
  int y;     // y in img
  int scale; // percentage
  fb_image *data;
  fb_image *tmp; // temp for zoom
} pos_image;

pos_image *init_image(fb_image *data);
void free_image(pos_image *img);
void move_image(int x, int y, pos_image *img);
void display_image(pos_image *img_ptr);
void zoom_image(int scale, pos_image *img);