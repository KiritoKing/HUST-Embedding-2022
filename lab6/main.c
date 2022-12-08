#include "lab.h"
#define COLOR_BACKGROUND FB_COLOR(0xff, 0xff, 0xff) // white

static char pic_list[3][50] = {"big.jpg", "big2.jpg", "test.jpg"};
int total = 3, now = 0;
static pos_image *img = NULL;
static fb_image *data = NULL;

static int touch_fd;
static finger_pos fin_pos[5];
static int mv_unit = 100;
static int zm_unit = 10;

static void touch_event_cb(int fd)
{
	if (img == NULL)
		return;
	static int lock = -1;			 // multi-touch lock, start from -1
	static int pinch_mode = 0; // flag of zoom

	fb_image *data = img->tmp == NULL ? img->data : img->tmp;
	int iw = data->pixel_w;
	int type, x, y, finger;
	type = touch_read(fd, &x, &y, &finger);
	switch (type)
	{
	case TOUCH_PRESS:
		if (lock > 0 || pinch_mode) // maximum touch: 2
			return;
		printf("lock: %d\n", ++lock);
		printf("TOUCH_PRESS:x=%d,y=%d,finger=%d\n", x, y, finger);
		if (lock == 1) // 2-finger
		{
			pinch_mode = 1;
			printf("pinch mode on\n");
		}
		fin_pos[finger].x = x;
		fin_pos[finger].y = y;
		break;
	case TOUCH_MOVE:
		// printf("TOUCH_MOVE：x=%d,y=%d,finger=%d\n", x, y, finger);
		// fin_pos[finger].x = x;
		// fin_pos[finger].y = y;
		break;
	case TOUCH_RELEASE:
		if (finger > lock)
			return;
		printf("lock: %d\n", --lock);
		printf("TOUCH_RELEASE:x=%d,y=%d,finger=%d\n", x, y, finger);
		if (!pinch_mode) // single-finger event
		{
			int event = get_single_event(fin_pos[finger].x, fin_pos[finger].y, x, y);
			switch (event)
			{
			case DRAG_B2T:
				printf("↑\n");
				move_image(img->x, img->y + mv_unit, img);
				display_image(img);
				break;
			case DRAG_T2B:
				printf("↓\n");
				move_image(img->x, img->y - mv_unit, img);
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
					fit_screen(img);
					printf("prev pic\n");
				}
				else
				{
					move_image(img->x - mv_unit, img->y, img);
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
					fit_screen(img);
					printf("next pic\n");
				}
				else
					move_image(img->x + mv_unit, img->y, img);
				display_image(img);
				break;
			default:
				break;
			}
		}
		else // pinch mode
		{
			// prev release finger
			static finger_pos prev_fp = {0, 0};
			static finger_pos fp = {0, 0};
			if (lock == 0) // still 1 finger in touch
			{
				prev_fp = fin_pos[finger];
				fp.x = x;
				fp.y = y;
				break;
			}
			else if (lock == -1)
			{
				finger_pos tmp = {x, y};
				int event = get_pinch_event(prev_fp, fp, fin_pos[finger], tmp);
				switch (event)
				{
				case ZOOM_IN:
					printf("zoom in\n");
					zoom_image(img->scale + zm_unit, img);
					display_image(img);
					break;
				case ZOOM_OUT:
					printf("zoom out");
					zoom_image(img->scale - zm_unit, img);
					display_image(img);
					break;
				default:
					break;
				}
				pinch_mode = 0;
				printf("pinch mode off\n");
			}
		}
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

	fit_screen(img);

	display_image(img);

	// touch utils
	touch_fd = touch_init("/dev/input/event1");
	task_add_file(touch_fd, touch_event_cb);

	task_loop(); // 进入任务循环

	return 0;
}