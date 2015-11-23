/* These are the generic things that every module in the project needs. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <assert.h>

#define PROGRAM_NAME "Prototype"
#define X_SIZE_OF_SCREEN 640
#define Y_SIZE_OF_SCREEN 480

#define X_SIZE_OF_WORLD 840
#define Y_SIZE_OF_WORLD 680

#define X_SIZE_OF_NODE 20
#define Y_SIZE_OF_NODE 20

#define X_SIZE_OF_WORKER 40
#define Y_SIZE_OF_WORKER 40

#define DEFAULT_RESOURCEUNITS 100
#define DEFAULT_SPAWNDELAY 3000
#define DEFAULT_SPAWNRADIUS 200.0
#define DEFAULT_MAXNODECOUNT 10
#define FRAME_TIME 20

#define NUM_OF_KEYS 30
#define PANSPEEDMULTI 0.2

#define PI 3.14159
#define DEGREESINCIRCLE 360.0

int isPointInRect(int point_x, int point_y, SDL_Rect rect);
int isRectEnclosedInRect(SDL_Rect rectA, SDL_Rect rectB);
void ensureRectEnclosed(SDL_Rect *ensure, SDL_Rect *against);
int testRectIntersection(SDL_Rect rectA, SDL_Rect rectB);
float generateRandomCoordOffset(float radius);
double getDistance2BetweenPoints(float p1X, float p1Y, float p2X, float p2Y);
float square(float f);
float randPi(void);
