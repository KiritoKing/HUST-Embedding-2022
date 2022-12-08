#include "lab.h"

pos_image *init_image(fb_image *data)
{
  pos_image *img = (pos_image *)malloc(sizeof(pos_image));
  img->data = data;
  img->scale = 100;
  img->x = 0;
  img->y = 0;
  return img;
}

void free_image(pos_image *img)
{
  fb_free_image(img->data);
  free(img);
}

void move_image(int x, int y, pos_image *img)
{
  int w = img->data->pixel_w; // img width
  int h = img->data->pixel_h; // img height
  if (x + SCREEN_WIDTH < w)
    img->x = x;
  else
  {
    printf("over scrolled x\n");
    if (w > SCREEN_WIDTH)
      img->x = w - SCREEN_WIDTH;
    else
      img->x = 0;
  }
  if (y + SCREEN_HEIGHT < h)
    img->y = y;
  else
  {
    printf("over scrolled y\n");
    if (h > SCREEN_HEIGHT)
      img->y = h - SCREEN_HEIGHT;
    else
      img->y = 0;
  }
  printf("moved to (%d, %d)\n", img->x, img->y);
}

void display_image(pos_image *img_ptr)
{
  pos_image img = *img_ptr;
  int w = img.data->pixel_w > SCREEN_WIDTH ? SCREEN_WIDTH : img.data->pixel_w;   // sub-img width
  int h = img.data->pixel_h > SCREEN_HEIGHT ? SCREEN_HEIGHT : img.data->pixel_h; // sub-img height
  fb_image *part = fb_new_image(FB_COLOR_RGB_8880, w, h, w * 4);
  char *buf = (char *)malloc(w * h * 4);
  int row, written;
  for (row = img.y, written = 0; row < img.y + h; row++, written++)
  {
    char *src = img.data->content + (row * img.data->pixel_w + img.x) * 4; // 写入图像行首地址
    char *dst = buf + written * part->line_byte;                           // 新图像行首地址
    memcpy(dst, src, w * 4);
  }
  part->content = buf;
  fb_draw_image(0, 0, part, 0);
  fb_update();
  fb_free_image(part);
}