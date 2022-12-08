#include "../common/common.h"
#define BLACK FB_COLOR(0,0,0)

int main() {
  fb_init("/dev/fb0");
	fb_draw_rect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,BLACK);
	fb_update();
	printf("\n========== ScreenCleared ===========\n");
}