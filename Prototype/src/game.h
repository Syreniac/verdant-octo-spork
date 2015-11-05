#include "UI.h"

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

int gameStart(SDL_Window *window);
int gameLoop(GameData *gameData);
float calculateDt(float previousRunTime);
