#include "UI.h"

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
