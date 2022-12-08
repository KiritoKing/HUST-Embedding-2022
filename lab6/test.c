#include "lab.h"

void SHELL_TEST(pos_image *img)
{
  printf("input command(move/zoom/prev/next): ");
  // get command
  char cmd[100] = "";
  while (scanf("%s", cmd) == 1)
  {
    char getBuf;
    while ((getBuf = getchar()) != '\n' && getBuf != EOF)
      continue;

    if (strcmp(cmd, "move"))
    {
      int offset_x, offset_y;
      while (scanf("%d%d", &offset_x, &offset_y) == 2)
      {
        move_image(offset_x, offset_y, img);
        display_image(img);
      }
    }
    else if (strcmp(cmd, "zoom"))
    {
      int scale;
      while (scanf("%d", &scale))
      {
        zoom_image(scale, img);
        display_image(img);
      }
    }
    else if (strcmp(cmd, "prev"))
    {
      printf("switch to previous pic\n");
    }
    else if (strcmp(cmd, "next"))
    {
      printf("switch to next pic\n");
    }
    else
    {
      printf("Error: Invalid command\n");
    }
    while ((getBuf = getchar()) != '\n' && getBuf != EOF)
      continue;
    printf("input command(move/zoom/prev/next): ");
  }
}