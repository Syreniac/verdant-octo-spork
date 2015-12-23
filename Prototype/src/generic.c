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



float randPi(void){
  /* This returns a random value between PI and 0. It's very useful for working
     trigonometric randomness. */
  return (float)(PI * (float) rand() / (float) RAND_MAX);
}

float square(float f){
  /* float f = the float to square

     Simply returns f*f to simulate squaring a number */
  return f*f;
}

double getDistance2BetweenPoints(float p1X, float p1Y, float p2X, float p2Y){
  /* float p1X = the x position of the first point
     float p1Y = the y position of the first point
     float p2X = the x position of the second point
     float p2Y = the y position of the second point

     Distance2 is the non-square rooted distance value between two points. If
     all we want is to check whether a point is within a radius of another,
     doing Distance2 < radius^2 is just as good as Distance < radius */
  return (square(p1X - p2X) + square(p1Y - p2Y));
}

void fitRectToWorld(SDL_Rect *rect){
  if(rect->x + rect->w > X_SIZE_OF_WORLD){
	rect->x = X_SIZE_OF_WORLD - rect->w;
  }
  else if(rect->x < 0){
	rect->x = 0;
  }
  if(rect->y + rect->h > Y_SIZE_OF_WORLD){
	rect->y = Y_SIZE_OF_WORLD - rect->h;
  }
  else if(rect->y < 0){
	rect->y = 0;
  }
}

float generateRandomCoordOffset(float radius){
  /* float radius = the range to generate random numbers within

     This function just generates a random x or y offset for a coordinate within
     a given range. It's the cheap hack I'm using to make randomly spaced nodes
     around a spawner. */
  float return_value = (radius * ( (float) rand() - (float) RAND_MAX * 0.5 )/(float) RAND_MAX );
  return(return_value);
}

double getDistance2BetweenRects(SDL_Rect rectA, SDL_Rect rectB){
  int rectAXC = rectA.x + rectA.w/2;
  int rectAYC = rectA.y + rectA.h/2;
  int rectBXC = rectB.x + rectB.w/2;
  int rectBYC = rectB.y + rectB.h/2;
  double dist = getDistance2BetweenPoints(rectAXC,rectAYC,rectBXC,rectBYC);
  return dist;
}

double getAngleBetweenRects(SDL_Rect *rectA, SDL_Rect *rectB){
  return atan2((double)(rectA->x + rectA->w/2 - rectB->w/2 - rectB->x),
               (double)(rectA->y + rectA->h/2 - rectB->h/2 - rectB->y));
}

SDL_Point getCenterOfRect(SDL_Rect rect){
  SDL_Point point;
  point.x = rect.x + rect.w/2;
  point.y = rect.y + rect.h/2;
  return point;
}

SDL_Rect getRectFromInvRect(SDL_Window *window, int from_left, int from_top, int from_right, int from_bot){
	int win_x = X_SIZE_OF_SCREEN, win_y = Y_SIZE_OF_SCREEN;
	SDL_Rect rect;
	SDL_GetWindowSize(window, &win_x, &win_y);
	if(from_left < 0){
		from_left = win_x + from_left;
		from_right -= from_left;
	}
	if(from_top < 0){
		from_top = win_y + from_top;
		from_bot -= from_top;
	}
	rect.x = from_left;
	rect.y = from_top;
	rect.w = (win_x - from_right) - rect.x;
	rect.h = (win_y - from_bot) - rect.y;
	return rect;
}

SDL_Rect getRectFromPercRect(SDL_Window *window, float from_left, float from_top, float from_right, float from_bot){
	int win_x = X_SIZE_OF_SCREEN, win_y = Y_SIZE_OF_SCREEN;
	SDL_Rect rect;
	SDL_GetWindowSize(window, &win_x, &win_y);
	rect.x = (int)((float) win_x * from_left);
	rect.y = (int)((float) win_y * from_top);
	rect.w = (int)((float) win_x * from_right);
	rect.h = (int)((float) win_y * from_bot);
	return rect;
}

char *fileToString(FILE *file){
	/* This string is malloc'd, and needs to be freed */
	char *s = NULL;
	int i = 0;
	int i2 = 0;
	char c;
	while((c = fgetc(file)) != EOF){
		i2++;
		if(i2 > i){
			i += FILE_TO_STRING_STEP_SIZE;
			s = realloc(s,i);
		}
		s[i2 - 1] = c;
	}
	s[i2] = '\0';
	return s;
}

#if DEBUGGING==1
#undef calloc
#undef malloc
#undef realloc
#undef free

void *debug_calloc(int line, char *filename, int itemCount, int itemSize){
  char *p;
  size_t vp;
  printf("callocing @ %s:%d ",filename,line);
  fprintf(DEBUGGING_FILE_ALLOC,"callocing @ %s:%d ",filename,line);
  p = calloc(itemCount,itemSize);
  vp = p;
  printf("getting block %p of size %d (%p)\n",vp,itemCount*itemSize,p+itemCount*itemSize*8);
  fprintf(DEBUGGING_FILE_ALLOC,"getting block %p of size %d (%p)\n",vp,itemCount*itemSize,p+itemCount*itemSize*8);
  fflush(DEBUGGING_FILE_ALLOC);
  return p;
}

void *debug_malloc(int line, char *filename, int totalSize){
  char *p;
  size_t vp;
  printf("mAlLoCiNg @ %s:%d ",filename,line);
  fprintf(DEBUGGING_FILE_ALLOC,"callocing @ %s:%d ",filename,line);
  p = malloc(totalSize);
  vp = p;
  printf("getting block %p of size %d (%p)\n",p,totalSize,vp+totalSize*8);
  fprintf(DEBUGGING_FILE_ALLOC,"getting block %p of size %d (%p)\n",p,totalSize,vp+totalSize*8);
  fflush(DEBUGGING_FILE_ALLOC);
  return p;
}


void *debug_realloc(int line, char *filename, void* oldPointer, int newSize){
  char *p;
  size_t vp;
  printf("reallocing @ %s:%d from %p to ",filename,line,oldPointer);
  fprintf(DEBUGGING_FILE_ALLOC,"reallocing @ %s:%d from %p to ",filename,line,oldPointer);
  p = realloc(oldPointer,newSize);
  vp = p;
  printf("getting block %p of size %d (%p)\n",vp,newSize,p+newSize);
  fprintf(DEBUGGING_FILE_ALLOC,"getting block %p of size %d (%p)\n",vp,newSize,vp+newSize*8);
  fflush(DEBUGGING_FILE_ALLOC);
  return p;
}

void debug_free(int line, char *filename, void* pointer){
  fprintf(DEBUGGING_FILE_FREE,"freeing @ %s:%d pointer %p\n",filename,line,pointer);
  fflush(DEBUGGING_FILE_FREE);
  printf("freeing @ %s:%d pointer %p",filename,line,pointer);
  free(pointer);
  printf("\n");
}
#define calloc(x,y) debug_calloc(__LINE__,__FILE__,x,y)
#define malloc(x) debug_malloc(__LINE__,__FILE__,x)
#define realloc(x,y) debug_realloc(__LINE__,__FILE__,x,y)
#define free(x) debug_free(__LINE__,__FILE__,x)
#endif
