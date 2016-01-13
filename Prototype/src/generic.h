/* These are the generic things that every module in the project needs. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <SDL_mixer.h>
#include <time.h>
#include <SDL_ttf.h>
#include <stdarg.h>

#define DEBUGGING 0
#if DEBUGGING==1
#define calloc(x,y) debug_calloc(__LINE__,__FILE__,x,y)
#define malloc(x) debug_malloc(__LINE__,__FILE__,x)
#define realloc(x,y) debug_realloc(__LINE__,__FILE__,x,y)
#define free(x) debug_free(__LINE__,__FILE__,x)

FILE *DEBUGGING_FILE_ALLOC;
FILE *DEBUGGING_FILE_FREE;
#endif

#define BENCHMARK_TEST 0

#define PI 3.14159
#define DEGREESINCIRCLE 360.0
#define RADIANSTODEGREES 57.2958

#define SUMMER_INDEX 0
#define AUTUMN_INDEX 1
#define WINTER_INDEX 2

#define FILE_TO_STRING_STEP_SIZE 10

typedef enum personGraphic {WITH_ICE_CREAM_STRIDE1, WITH_ICE_CREAM_STRIDE2} personGraphic;
#define NO_ICECREAM_INDEX_OFFSET 2

#if DEBUGGING==1
void *debug_calloc(int line, char *filename, int itemCount, int itemSize);
void *debug_malloc(int line, char *filename, int totalSize);
void *debug_realloc(int line, char *filename, void* oldPointer, int newSize);
void debug_free(int line, char *filename, void* pointer);
#endif

int isPointInRect(int point_x, int point_y, SDL_Rect rect);
int isRectEnclosedInRect(SDL_Rect rectA, SDL_Rect rectB);
void ensureRectEnclosed(SDL_Rect *ensure, SDL_Rect *against);
int testRectIntersection(SDL_Rect rectA, SDL_Rect rectB);
float generateRandomCoordOffset(float radius);
double getDistance2BetweenPoints(float p1X, float p1Y, float p2X, float p2Y);
double getDistance2BetweenRects(SDL_Rect rectA, SDL_Rect rectB);
float square(float f);
float randPi(void);
double getAngleBetweenRects(SDL_Rect *rectA, SDL_Rect *rectB);
SDL_Point getCenterOfRect(SDL_Rect rect);
SDL_Rect getRectFromInvRect(SDL_Window *window, int from_left, int from_top, int from_right, int from_bot);
SDL_Rect getRectFromPercRect(SDL_Window *window, float from_left, float from_right, float from_top, float from_bot);
char *fileToString(FILE *file);
SDL_Point getPointFromInvPoint(SDL_Window *window, int x, int y);
SDL_Point getPointFromPerc(SDL_Window *window, float x, float y);
void shrinkRectToFit(SDL_Rect *toShrink, SDL_Rect *fitTo);
