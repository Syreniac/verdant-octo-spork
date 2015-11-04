/* A bunch of things used all over the place are put in here. */

#include "generic.h"

int isPointInRect(int point_x, int point_y, SDL_Rect rect){
  if(point_x < rect.x || point_x > rect.x + rect.w){
    return 0;
  }
  if(point_y < rect.y || point_y > rect.y + rect.h){
    return 0;
  }
  return 1;
}

int isRectEnclosedInRect(SDL_Rect rectA, SDL_Rect rectB){
  if(rectA.x < rectB.x || rectA.x+rectA.w > rectB.x+rectB.w){
    return 0;
  }
  if(rectA.y < rectB.y || rectA.y+rectA.h > rectB.y+rectB.h){
    return 0;
  }
  return 1;
}

void ensureRectEnclosed(SDL_Rect *ensure, SDL_Rect *against){
  /* This function will ensure that the first rectangle is entirely within the
     other by moving it until there is no collision */
  int xOffset = 0, yOffset = 0;
  if(ensure->x < against->x){
    xOffset = against->x - ensure->x;
  }
  else if(ensure->x + ensure->w > against->x + against->w){
    xOffset = (against->x+against->w) - (ensure->x+ensure->w);
  }

  if(ensure->y < against->y){
    yOffset = against->y - ensure->y;
  }
  else if(ensure->y + ensure->h > against->y + against->h){
    yOffset = (against->y+against->h) - (ensure->y+ensure->h);
  }
  ensure->x += xOffset;
  ensure->y += yOffset;
}
