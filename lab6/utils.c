#include "lab.h"

extern int total;
extern int now;

pos_image *init_image(fb_image *data)
{
  pos_image *img = (pos_image *)malloc(sizeof(pos_image));
  img->data = data;
  img->scale = 100;
  img->x = 0;
  img->y = 0;
  img->tmp = NULL;
  return img;
}

void free_image(pos_image *img)
{
  fb_free_image(img->data);
  if (img->tmp != NULL)
    fb_free_image(img->tmp);
  free(img);
}

void move_image(int x, int y, pos_image *img)
{
  int w = img->data->pixel_w; // img width - overscroll alias
  int h = img->data->pixel_h; // img height

  if (img->scale != 100 && img->tmp != NULL) // choose zoomed img
  {
    printf("move zoomed image\n");
    w = img->tmp->pixel_w;
    h = img->tmp->pixel_h;
  }

  if (x + SCREEN_WIDTH < w)
    img->x = x > 0 ? x : 0;
  else
  {
    printf("over scrolled x\n");
    if (w > SCREEN_WIDTH)
      img->x = w - SCREEN_WIDTH;
    else
      img->x = 0;
  }

  if (y + SCREEN_HEIGHT < h)
    img->y = y > 0 ? y : 0;
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

void display_image(pos_image *img)
{
  // refresh screen
  fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
  fb_update();

  fb_image *data = img->tmp == NULL ? img->data : img->tmp;                        // choose zoomed img if exist
  int w = data->pixel_w > SCREEN_WIDTH ? SCREEN_WIDTH : data->pixel_w;             // sub-img width
  int h = data->pixel_h > SCREEN_HEIGHT ? SCREEN_HEIGHT : data->pixel_h;           // sub-img height
  int x = data->pixel_w > SCREEN_WIDTH ? 0 : (SCREEN_WIDTH - data->pixel_w) / 2;   // screen x
  int y = data->pixel_h > SCREEN_HEIGHT ? 0 : (SCREEN_HEIGHT - data->pixel_h) / 2; // screen y

  fb_image *part = fb_new_image(data->color_type, w, h, w * 4);
  char *buf = (char *)malloc(w * h * 4);
  int row, written;
  for (row = img->y, written = 0; row < img->y + h; row++, written++)
  {
    char *src = data->content + (row * data->pixel_w + img->x) * 4; // 写入图像行首地址
    char *dst = buf + written * part->line_byte;                    // 新图像行首地址
    memcpy(dst, src, w * 4);
  }
  part->content = buf;
  fb_draw_image(x, y, part, 0);
  fb_update();
  fb_free_image(part);
}

void zoom_image(int scale, pos_image *img) // 采用临近插值法计算
{
  if (scale < 1 || scale == img->scale || scale > 200)
    return;
  float ratio = (float)scale / (float)100;

  if (scale == 100)
  {
    printf("reset and clear zoom temp\n");
    move_image(img->x * ratio, img->y * ratio, img); // move anchor
    img->scale = scale;
    if (img->tmp != NULL)
    {
      fb_free_image(img->tmp);
      img->tmp = NULL;
    }
    return;
  }

  img->scale = scale;

  int iw = img->data->pixel_w * ratio;
  int ih = img->data->pixel_h * ratio;
  printf("zoom: %d%% (%d*%d)\n", scale, iw, ih);

  fb_image *zoom_img = fb_new_image(img->data->color_type, iw, ih, iw * 4);
  char *buf = (char *)malloc(iw * ih * 4 * sizeof(char));
  int i, j, written_byte;

  for (i = 0, written_byte = 0; i < ih; i++) // traverse lines: y_dst
  {
    for (j = 0; j < iw; j++, written_byte++) // traverse items: x_dst
    {
      if (written_byte > iw * ih)
      {
        printf("buffer overflow when zooming\n");
        exit(1);
      }
      char *dst = buf + written_byte * 4;

      int ox = ((float)j / ratio) + 0.5; // origin x 四舍五入
      int oy = ((float)i / ratio) + 0.5; // origin y 四舍五入
      // printf("(%d, %d) <=> (%d, %d)\n", j, i, ox, oy);
      char *src = img->data->content + (oy * img->data->pixel_w + ox) * 4;
      memcpy(dst, src, 4);
    }
  }
  if (img->tmp != NULL)
    fb_free_image(img->tmp);
  zoom_img->content = buf;
  img->tmp = zoom_img;

  move_image(img->x * ratio, img->y * ratio, img); // move anchor
}

void fit_screen(pos_image *img)
{
  float ratio = 0;
  // 只考虑原数据
  int w = img->data->pixel_w;
  int h = img->data->pixel_h;
  if (w > h)
  {
    if (w < SCREEN_WIDTH)
      return;
    ratio = (float)SCREEN_WIDTH / (float)w;
  }
  else
  {
    if (h < SCREEN_HEIGHT)
      return;
    ratio = (float)SCREEN_HEIGHT / (float)h;
  }
  if (ratio == 0)
    return;
  int scale = (int)(ratio * (float)100);
  scale = scale / 10 * 10;
  printf("fit ratio: %f, scale: %d\n", ratio, scale);
  zoom_image(scale, img);
}

char *get_ext(const char *fname)
{
  int len = strlen(fname);
  for (int i = len - 1; i >= 0; i--)
  {
    if (fname[i] == '.')
    {
      int ext_len = len - i - 1;
      char *ext = (char *)malloc(ext_len + 1);
      memset(ext, '\0', ext_len + 1);
      memcpy(ext, fname + i + 1, ext_len);
      printf("type: %s file\n", ext);
      return ext;
    }
  }
  return NULL;
}

int str_cmp_head(const char *dst, const char *src)
{
  int flag = 1;
  for (int i = 0; dst[i] != '\0'; i++)
  {
    if (src[i] == '\0')
      break;
    if (dst[i] != src[i])
    {
      flag = 0;
      break;
    }
  }
  return flag;
}

int next_pic(int now)
{
  if (now + 1 < total)
    return now + 1;
  else
    return 0;
}

int prev_pic(int now)
{
  if (now > 0)
    return now - 1;
  else
    return total - 1;
}