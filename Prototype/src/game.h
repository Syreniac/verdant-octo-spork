#include "controls.h"

typedef struct GameData GameData;
struct GameData{
  int number;
  SDL_Window *window;
  Uint32 gameStartTime;
  int gameRunTime;
  int running;
  int delta;
  UI_Element *element;

  AIData aiData;
  GraphicsData graphicsData;
  GameObjectData gameObjectData;
  UIData uiData;
  ControlsData controlsData;
  AudioData audioData;
  AnnouncementsData announcementsData;
};

int gameStart(GraphicsData graphicsData, AudioData audioData, ConfigData configData);
int gameLoop(GameData *gameData);
int calculateDt(int previousRunTime);
