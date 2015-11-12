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
#define FRAME_RATE 16.6667

#define NUM_OF_KEYS 30
#define PANSPEEDMULTI 0.2

typedef struct GraphicsData GraphicsData;

enum Keybindings {ARROW_DOWN,
                  ARROW_UP,
				  ARROW_LEFT,
				  ARROW_RIGHT,
				  SPACE,
				  MAX_KEYS};

struct GraphicsData{
  SDL_Window *window;
  SDL_Surface *workerGraphic;
  SDL_Surface *nodeGraphic;
  SDL_Surface *hiveGraphic;
  SDL_Rect navigationOffset;
  SDL_Renderer *renderer;
  SDL_Texture *workerTexture;
  SDL_Texture *nodeTexture;
  SDL_Texture *hiveTexture;
  int keys[MAX_KEYS]; /*keybindings*/
};

typedef enum Keybindings Keybindings;

int isPointInRect(int point_x, int point_y, SDL_Rect rect);
int isRectEnclosedInRect(SDL_Rect rectA, SDL_Rect rectB);
void ensureRectEnclosed(SDL_Rect *ensure, SDL_Rect *against);
void blitGameObject(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Surface *graphic, SDL_Texture *texture);
int testRectIntersection(SDL_Rect rectA, SDL_Rect rectB);
