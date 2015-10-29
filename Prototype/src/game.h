#ifndef _GENERIC_INCLUDED
#include "generic.h"
#endif
#include "world_generation.h"
#include "AI.h"
/* Because we're working with numerous files that need to include game_objects.h
   having this #ifndef check here prevents things breaking from trying to
   include the same file several times */

typedef struct GameData GameData;
struct GameData{
  int number;
  SDL_Window *window;
  Uint32 gameStartTime;
  float gameRunTime;
  GameObjectData gameObjectData;
  SDL_Surface *nodeGraphic;
  SDL_Surface *workerGraphic;
  BlockFunctionRoot *blockFunctionRoots;
};

int gameStart(SDL_Window *window);
int gameLoop(GameData *gameData);
float calculateDt(float previousRunTime);
