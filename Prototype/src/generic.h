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

#define VERBOSE 1

#define PROGRAM_NAME "Prototype"
#define X_SIZE_OF_SCREEN 640
#define Y_SIZE_OF_SCREEN 480

#define X_SIZE_OF_WORLD 2100
#define Y_SIZE_OF_WORLD 1700

#define X_INITIAL_SCREEN_OFFSET -(X_SIZE_OF_WORLD/2)+(X_SIZE_OF_SCREEN/2)
#define Y_INITIAL_SCREEN_OFFSET -(Y_SIZE_OF_WORLD/2)+(Y_SIZE_OF_SCREEN/2)

#define X_SIZE_OF_NODE 20
#define Y_SIZE_OF_NODE 20

#define GRASS_TILE_WIDTH 350
#define GRASS_TILE_HEIGHT 76

#define RAIN_TILE_WIDTH 9
#define RAIN_TILE_HEIGHT 9
#define RAIN_FRAME_DELAY 4

/*lower values result in a higher chance of bees regaining flight when wet, with
each call to updateProgrammableWorker*/
#define CHANCE_OF_REGAINING_FLIGHT 1000

/*lower values result in a higher chance of bees falling during rain, with
each call to updateProgrammableWorker*/
#define CHANCE_OF_FALLING_IN_RAIN 100

/*higher values result in lower levels of parallax*/
#define PARALLAX_INTENSITY 10

#define PERSON_HEIGHT 54
#define PERSON_WIDTH 50
/*probability that the iceCreamPerson will appear each time that*/
/*the updateGameObjects function is called, if it is also sunny and*/
/*he is not already on screen */
#define ICE_CREAM_PERSON_PROB 100
#define STRIDE_FREQUENCY 0.75

#define SIZE_OF_TREE 200
#define NUMBER_OF_TREES 50
#define NUMBER_OF_FLOWER_PATCHES 50


#define X_SIZE_OF_WORKER 40
#define Y_SIZE_OF_WORKER 40
#define WORKER_SPEED 0.4

#define DEFAULT_RESOURCEUNITS 100
#define DEFAULT_SPAWNDELAY 100*60
#define DEFAULT_SPAWNRADIUS 200.0
#define DEFAULT_MAXNODECOUNT 10
#define TICKSPERWEATHER 2500
#define FRAME_TIME 20

#define NUM_OF_KEYS 30
#define PANSPEEDMULTI 0.5

#define PI 3.14159
#define DEGREESINCIRCLE 360.0
#define RADIANSTODEGREES 57.2958

typedef enum personGraphic {WITH_ICE_CREAM_STRIDE1, WITH_ICE_CREAM_STRIDE2} personGraphic;

int isPointInRect(int point_x, int point_y, SDL_Rect rect);
int isRectEnclosedInRect(SDL_Rect rectA, SDL_Rect rectB);
void ensureRectEnclosed(SDL_Rect *ensure, SDL_Rect *against);
int testRectIntersection(SDL_Rect rectA, SDL_Rect rectB);
float generateRandomCoordOffset(float radius);
double getDistance2BetweenPoints(float p1X, float p1Y, float p2X, float p2Y);
double getDistance2BetweenRects(SDL_Rect rectA, SDL_Rect rectB);
float square(float f);
float randPi(void);
void fitRectToWorld(SDL_Rect *rect);
double getAngleBetweenRects(SDL_Rect *rectA, SDL_Rect *rectB);
SDL_Point getCenterOfRect(SDL_Rect rect);
