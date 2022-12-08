#pragma once
#include "../common/common.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define RED FB_COLOR(255, 0, 0)
#define ORANGE FB_COLOR(255, 165, 0)
#define YELLOW FB_COLOR(255, 255, 0)
#define GREEN FB_COLOR(0, 255, 0)
#define CYAN FB_COLOR(0, 127, 255)
#define BLUE FB_COLOR(0, 0, 255)
#define PURPLE FB_COLOR(139, 0, 255)
#define WHITE FB_COLOR(255, 255, 255)
#define BLACK FB_COLOR(0, 0, 0)

enum TOUCH_EVENT
{
  DRAG_L2R = 1,
  DRAG_R2L,
  DRAG_B2T,
  DRAG_T2B,
  ZOOM_IN,
  ZOOM_OUT,
  INVALID
};

typedef struct
{
  int x;     // x in img
  int y;     // y in img
  int scale; // percentage
  fb_image *data;
  fb_image *tmp; // temp for zoom
} pos_image;

void SHELL_TEST(pos_image *img);

// 图片渲染相关
pos_image *init_image(fb_image *data);
void free_image(pos_image *img);
void move_image(int x, int y, pos_image *img);
void display_image(pos_image *img_ptr);
void zoom_image(int scale, pos_image *img);
void fit_screen(pos_image *img);

// 多文件相关
char *get_ext(const char *fname);
int str_cmp_head(const char *dst, const char *src);
int next_pic(int now);
int prev_pic(int now);

// 触控相关
int get_single_event(int x1, int y1, int x2, int y2);
