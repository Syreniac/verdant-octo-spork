/* A bunch of things used all over the place are put in here. */

#include "generic.h"

int isPointInRect(int point_x, int point_y, SDL_Rect rect){
  /* int point_x   = the x coordinate of the point we want to test
     int point_y   = the y coordinate fo the point we want to test
     SDL_Rect rect = the rectangle we want to test against. It holds x,y,w,h.

     This checks whether a point is inside a given rectangle.
     returns 1 if it is or 0 if it isn't.*/
  if(point_x < rect.x || point_x > rect.x + rect.w){
    return 0;
  }
  if(point_y < rect.y || point_y > rect.y + rect.h){
    return 0;
  }
  return 1;
}

int testRectIntersection(SDL_Rect rectA, SDL_Rect rectB){
	if (rectA.x+rectA.w<rectB.x || rectB.x+rectB.w<rectA.x || rectA.y+rectA.h<rectB.y || rectB.y+rectB.h<rectA.y){
		return 0;
	}
	return 1; /*There is intersection!*/
}

int isRectEnclosedInRect(SDL_Rect rectA, SDL_Rect rectB){
  /* SDL_Rect rectA = the rectangle that we want to test is enclosed.
     SDL_Rect rectB = the rectangle that we want to see if it encloses rectA.

     This checks to see whether a given rectangle is entirely inside another. */
  if(rectA.x < rectB.x || rectA.x+rectA.w > rectB.x+rectB.w){
    return 0;
  }
  if(rectA.y < rectB.y || rectA.y+rectA.h > rectB.y+rectB.h){
    return 0;
  }
  return 1;
}

void ensureRectEnclosed(SDL_Rect *ensure, SDL_Rect *limit){
  /* SDL_Rect *ensure = the pointer to the rectangle that we want to ensure is
                        inside another
     SDL_Rect *limit  = the pointer to the rectangle that we want to use as the
                        limit for the other

     This function will ensure that the first rectangle is entirely within the
     other by moving it until there is no collision */
  int xOffset = 0, yOffset = 0;
  if(ensure->x < limit->x){
    xOffset = limit->x - ensure->x;
  }
  else if(ensure->x + ensure->w > limit->x + limit->w){
    xOffset = (limit->x+limit->w) - (ensure->x+ensure->w);
  }

  if(ensure->y < limit->y){
    yOffset = limit->y - ensure->y;
  }
  else if(ensure->y + ensure->h > limit->y + limit->h){
    yOffset = (limit->y+limit->h) - (ensure->y+ensure->h);
  }
  ensure->x += xOffset;
  ensure->y += yOffset;
}

void blitGameObject(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Surface *graphic, SDL_Texture *texture){
  SDL_Rect tempRect;


  
  tempRect = objectRect;
  tempRect.x += graphicsData->navigationOffset.x;
  tempRect.y += graphicsData->navigationOffset.y;
  


    /*this function copies the texture into the renderer, but is not put on screen until*/
    /*SDL_RenderPresent is called in the gameloop of game.c*/
    /*SDL_RenderPresent function must be called after all graphical changes have been made, including those*/
    /*made by UIrender, also found in the gameloop*/
  SDL_RenderCopy(graphicsData->renderer, texture, &graphic->clip_rect, &tempRect);





 /* SDL_BlitSurface(surface,
                  NULL,
                  SDL_GetWindowSurface(graphicsData->window),
                  &tempRect);*/

}
