#include "lab.h"

int main()
{
	printf("\n=============Lab6: Image Display App Running=============\n");

	fb_init("/dev/fb0");
	font_init("./font.ttc");

	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
	fb_update();

	printf("Name Of JPG: ");
	char file[10] = "";
	scanf("%s", file);
	char getBuf;
	while ((getBuf = getchar()) != '\n' && getBuf != EOF)
		continue;

	fb_image *data = fb_read_jpeg_image(file);
	pos_image *img = init_image(data);
	display_image(img);
	int scale;
	while (scanf("%d", &scale))
	{
		while ((getBuf = getchar()) != '\n' && getBuf != EOF)
			continue;
		zoom_image(scale, img);
		display_image(img);
	}
	// display_image(img);
	// zoom_image(108, img);
	// int offset_x, offset_y;
	// while (scanf("%d%d", &offset_x, &offset_y) == 2)
	// {
	// 	move_image(offset_x, offset_y, img);
	// 	display_image(img);
	// }

	free_image(img);
	return 0;
}