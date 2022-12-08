#include "lab.h"
#define COLOR_BACKGROUND FB_COLOR(0xff, 0xff, 0xff) // white

static char pic_list[3][50] = {"big.jpg", "big2.jpg", "test.jpg"};
int total = 3, now = 2;
static pos_image *img = NULL;
static fb_image *data = NULL;

static int touch_fd;
static struct
{
	int x;
	int y;
} fin_pos[5];
static int unit = 100;

static void touch_event_cb(int fd)
{
	if (img == NULL)
		return;
	static int touched = 0; // num of finger touched
	static int lock = -1;		// occupied finger

	fb_image *data = img->tmp == NULL ? img->data : img->tmp;
	int iw = data->pixel_w;
	int type, x, y, finger;
	type = touch_read(fd, &x, &y, &finger);
	switch (type)
	{
	case TOUCH_PRESS:
		if (touched > 0)
			return;
		printf("t_b: %d\n", touched++);
		printf("TOUCH_PRESS:x=%d,y=%d,finger=%d\n", x, y, finger);
		lock = finger;
		fin_pos[finger].x = x;
		fin_pos[finger].y = y;
		break;
	case TOUCH_MOVE:
		// printf("TOUCH_MOVE：x=%d,y=%d,finger=%d\n", x, y, finger);
		// fin_pos[finger].x = x;
		// fin_pos[finger].y = y;
		break;
	case TOUCH_RELEASE:
		if (finger != lock)
			return;
		printf("t_b: %d\n", touched--);
		printf("TOUCH_RELEASE:x=%d,y=%d,finger=%d\n", x, y, finger);
		int event = get_single_event(fin_pos[finger].x, fin_pos[finger].y, x, y);
		switch (event)
		{
		case DRAG_B2T:
			printf("↑\n");
			move_image(img->x, img->y + unit, img);
			display_image(img);
			break;
		case DRAG_T2B:
			printf("↓\n");
			move_image(img->x, img->y - unit, img);
			display_image(img);
			break;
		case DRAG_L2R:
			printf("→\n");
			if (iw < SCREEN_WIDTH)
			{
				now = prev_pic(now);
				free_image(img);
				data = fb_read_jpeg_image(pic_list[now]);
				img = init_image(data);
				printf("prev pic\n");
			}
			else
			{
				move_image(img->x - unit, img->y, img);
			}
			display_image(img);
			break;
		case DRAG_R2L:
			printf("←\n");
			if (iw < SCREEN_WIDTH)
			{
				now = next_pic(now);
				free_image(img);
				data = fb_read_jpeg_image(pic_list[now]);
				img = init_image(data);
				printf("next pic\n");
			}
			else
				move_image(img->x + unit, img->y, img);
			display_image(img);
			break;
		default:
			break;
		}
		lock = -1; // unlock
		break;
	case TOUCH_ERROR:
		printf("close touch fd\n");
		close(fd);
		task_delete_file(fd);
		break;
	default:
		return;
	}
	return;
}

int main()
{
	printf("\n=============Lab6: JPEG Viewer=============\n");
	fb_init("/dev/fb0");
	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
	fb_update();

	data = fb_read_jpeg_image(pic_list[now]);
	if (data == NULL)
		exit(1);
	img = init_image(data);
	printf("image size: (%d*%d)\n", data->pixel_w, data->pixel_h);

	display_image(img);

	// touch utils
	touch_fd = touch_init("/dev/input/event1");
	task_add_file(touch_fd, touch_event_cb);

	task_loop(); // 进入任务循环

	return 0;
}