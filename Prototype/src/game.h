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
  AudioData audioData;
};

int gameStart(GraphicsData graphicsData, AudioData audioData);
int gameLoop(GameData *gameData);
int calculateDt(int previousRunTime);
