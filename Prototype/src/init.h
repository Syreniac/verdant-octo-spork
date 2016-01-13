#include "game.h"


struct InitData{
	AudioData audioData;
	GraphicsData graphicsData;
  UIData uiData;
	ConfigurationData configData;
};

typedef struct InitData InitData;


InitData initialise(void);
void uninitialise(void);
void audioSystem(AudioData *AudioSettings);
int game_welcome_page(GraphicsData graphicsData, AudioData audioData, ConfigurationData configData);
