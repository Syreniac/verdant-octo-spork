#include <SDL_image.h>
#include "controls.h"

typedef struct GameData GameData;
struct GameData{
  int number;
  SDL_Window *window;
  Uint32 gameStartTime;
  float gameRunTime;

  AIData aiData;
  GraphicsData graphicsData;
  GameObjectData gameObjectData;
  UIData uiData;
};

void gameEnd();
int gameStart(SDL_Window *window);
char* absolutePathGenerator(char* filepath);
SDL_Surface* IMG_LoadFromAbsolutePath(char* filepath);
SDL_Surface* loadBMPFromAbsolutePath(char* filepath);
FILE* fopenFromAbsolutePath(char* filepath);
int gameLoop(GameData *gameData);
float calculateDt(float previousRunTime);

void IMG_LoadAndVerify(SDL_Surface **gameData, char *filepath);
void fopenAndVerify(FILE **file, char *filepath);
