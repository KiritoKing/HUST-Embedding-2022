#include "lab.h"

int get_single_event(int x1, int y1, int x2, int y2)
{
  int un_th = 100; // threhold of movement
  int en_th = 50;
  if (abs(x1 - x2) < un_th) // 判定为纵向
  {
    if (y1 - y2 > en_th)
      return DRAG_B2T;
    else if (y2 - y1 > en_th)
      return DRAG_T2B;
  }
  else if (abs(y1 - y2) < un_th)
  {
    if (x1 - x2 > en_th)
      return DRAG_R2L;
    else if (x2 - x1 > en_th)
      return DRAG_L2R;
  }
  return INVALID;
}

int get_pinch_event(finger_pos prevA, finger_pos A, finger_pos prevB, finger_pos B)
{
  int dist_prev_x = abs(prevA.x - prevB.x);
  int dist_prev_y = abs(prevA.y - prevB.y);
  int dist_now_x = abs(A.x - B.x);
  int dist_now_y = abs(A.y - B.y);
  unsigned long dist_prev = dist_prev_x * dist_prev_x + dist_prev_y * dist_prev_y;
  unsigned long dist = dist_now_x * dist_now_x + dist_now_y * dist_now_y;
  if (dist_prev < dist)
    return ZOOM_IN;
  else if (dist_prev > dist)
    return ZOOM_OUT;
  else
    return INVALID;
}