#include "controls.h"

typedef struct GameData GameData;
struct GameData{
  int number;
  SDL_Window *window;
  Uint32 gameStartTime;
  int gameRunTime;

  AIData aiData;
  GraphicsData graphicsData;
  GameObjectData gameObjectData;
  UIData uiData;
  ControlsData controlsData;
};

int game_welcome_page(GraphicsData graphicsData);
int gameStart(GraphicsData graphicsData);
int gameLoop(GameData *gameData);
int calculateDt(int previousRunTime);
