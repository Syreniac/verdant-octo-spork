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
#define VERBOSE 1

#define NUMBER_OF_CELLS_IN_HIVE 6
#define SUGAR_COST_OF_WORKER 50
#define HIVE_CELL_SPAWN_DELAY 30*1000

#define HONEY_REQUIRED_FOR_WINTER 100
#define REQUIREMENT_YEAR_INCREASE_PERCENTAGE 10
#define DELAY_BEFORE_SUMMER 10

#define PROGRAM_NAME "Prototype"
#define X_SIZE_OF_SCREEN 960
#define Y_SIZE_OF_SCREEN 720

#define X_SIZE_OF_WORLD 21000
#define Y_SIZE_OF_WORLD 17000

#define X_INITIAL_SCREEN_OFFSET -(X_SIZE_OF_WORLD/2)+(X_SIZE_OF_SCREEN/2)
#define Y_INITIAL_SCREEN_OFFSET -(Y_SIZE_OF_WORLD/2)+(Y_SIZE_OF_SCREEN/2)

#define X_SIZE_OF_NODE 20
#define Y_SIZE_OF_NODE 20

#define GRASS_TILE_WIDTH 350
#define GRASS_TILE_HEIGHT 76

#define RAIN_TILE_WIDTH 9
#define RAIN_TILE_HEIGHT 9
#define RAIN_FRAME_DELAY 4


#define INITIAL_NUMBER_OF_WORKERS 700

/*lower values result in a higher chance of bees regaining flight when wet, with
each call to updateProgrammableWorker*/
#define CHANCE_OF_REGAINING_FLIGHT 1000

/*lower values result in a higher chance of bees falling during rain, with
each call to updateProgrammableWorker*/
#define CHANCE_OF_FALLING_IN_RAIN 1

/*how much smaller are the bees when they are on the ground (further away from screen*/
#define BEE_SHRINK_FACTOR_ON_GROUND 1.2

/*higher values result in lower levels of parallax*/
#define PARALLAX_INTENSITY 20

#define DROPPED_ICECREAM_HEIGHT 50
#define DROPPED_ICECREAM_WIDTH 50
#define MAX_DROPPED_ICECREAM_WIDTH 60
#define SUGAR_VALUE_OF_BLUE_FLOWER 1
#define SUGAR_VALUE_OF_RED_FLOWER 2
#define SUGAR_VALUE_OF_YELLOW_FLOWER 5
#define SUGAR_VALUE_OF_ICECREAM 50

#define MELT_TIME_THRESHOLD 1000
#define ICECREAM_PICKUP_RADIUS 20

#define SUGAR_VALUE_OF_FLOWER 1

#define PERSON_HEIGHT 54
#define PERSON_WIDTH 50
/*probability that the iceCreamPerson will appear each time that*/
/*the updateGameObjects function is called, if it is also sunny and*/
/*he is not already on screen */
#define ICE_CREAM_PERSON_PROB 100
#define STRIDE_FREQUENCY 0.75

#define STING_HIT_RADIUS PERSON_HEIGHT

#define SIZE_OF_TREE 200
#define SIZE_OF_TREESTUMP 70
#define NUMBER_OF_TREES 500
#define NUMBER_OF_FLOWER_PATCHES 50

#define SIZE_OF_FLOWER 20

#define TREE_SHELTER_RADIUS (SIZE_OF_TREE/2)

#define CARRYING_FLOWER_INDEX_OFFSET 2
#define CARRYING_ICECREAM_INDEX_OFFSET 4
#define MS_BETWEEN_FLAPPING 45


#define BENCHMARK_TEST 0

#define X_SIZE_OF_WORKER 40
#define Y_SIZE_OF_WORKER 40
#define WORKER_SPEED 0.3

#define WORKER_PERCEPTION_RADIUS 250
#define COLD_DEATH_THRESHOLD 200
#define STUNNED_AFTER_STING_DURATION (MELT_TIME_THRESHOLD + 200)


#define X_SIZE_OF_HIVE 64
#define Y_SIZE_OF_HIVE 80

#define HIVE_SHELTER_RADIUS (X_SIZE_OF_HIVE/2)

#define CELEBRATION_THRESHOLD 50

#define DEFAULT_RESOURCEUNITS 100
#define DEFAULT_SPAWNDELAY 300000
#define DEFAULT_SPAWNRADIUS 100.0
#define RANDOMISE_SPAWNRADIUS 2500
#define DEFAULT_MAXNODECOUNT 10
#define TICKSPERWEATHER 30000
#define SUN_LASTS_LONGER_FACTOR 5
#define FRAME_TIME 15

#define LENGTH_OF_STATUS_STRING 32

#define WINTER_COUNTDOWN_SPEED 0.01

#define MAX_DAYS_TO_WINTER 301
#define AUTUMN_THRESHOLD (MAX_DAYS_TO_WINTER / 3)
#define WINTER_THRESHOLD 1

#define DAYS_LABEL_WIDTH 155
#define DAYS_COUNTER_WIDTH 40

#define TOP_BAR_HEIGHT 30
#define SCORE_COUNTER_WIDTH 40
#define SCORE_LABEL_WIDTH 75

#define YEARS_COUNTER_WIDTH 40
#define YEARS_LABEL_WIDTH 145

#define NUM_OF_KEYS 30
#define PANSPEEDMULTI 0.5

#define PI 3.14159
#define DEGREESINCIRCLE 360.0
#define RADIANSTODEGREES 57.2958

#define SUMMER_INDEX 0
#define AUTUMN_INDEX 1
#define WINTER_INDEX 2

#define WORKER_SENSE_RANGE 500.0

#define PROGRAMMABLE_WORKER_CARGO_SPEED_FACTOR 0.7

#define FILE_TO_STRING_STEP_SIZE 10

typedef enum personGraphic {WITH_ICE_CREAM_STRIDE1, WITH_ICE_CREAM_STRIDE2} personGraphic;
#define NO_ICECREAM_INDEX_OFFSET 2

#define SPIDER 0
#define SPIDER2 1
#define SPIDER15 2
#define SPIDER15b 3
#define SPIDER_DEAD 4
#define SPIDER_FIGHTING 5
#define SPIDER_ATTACK_AREA 50
#define SPIDER_SPEED 0.25
#define SPIDER_STRIDE_FREQUENCY 1.0



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
void fitRectToWorld(SDL_Rect *rect);
double getAngleBetweenRects(SDL_Rect *rectA, SDL_Rect *rectB);
SDL_Point getCenterOfRect(SDL_Rect rect);
SDL_Rect getRectFromInvRect(SDL_Window *window, int from_left, int from_top, int from_right, int from_bot);
SDL_Rect getRectFromPercRect(SDL_Window *window, float from_left, float from_right, float from_top, float from_bot);
char *fileToString(FILE *file);
SDL_Point getPointFromInvPoint(SDL_Window *window, int x, int y);
SDL_Point getPointFromPerc(SDL_Window *window, float x, float y);
void shrinkRectToFit(SDL_Rect *toShrink, SDL_Rect *fitTo);
