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