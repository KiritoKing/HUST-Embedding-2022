#include <stdio.h>
#include "../common/common.h"
#include "lab.h"

int main()
{
	printf("\n=============Lab6: Image Display App Running=============\n");

	fb_init("/dev/fb0");
	font_init("./font.ttc");

	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
	fb_update();

	printf("file dir:");
	char file[10] = "";
	scanf("%s", file);

	fb_image *img;
	img = fb_read_jpeg_image(file);
	printf("line_byte: %d\n", img->line_byte);
	fb_draw_image(0, 0, img, 0);
	fb_update();
	fb_free_image(img);

	return 0;
}