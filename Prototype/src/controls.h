#include "UI.h"

/* I know that there are technically more than two, but we only need to know
   about two of them */

#define NUM_OF_MOUSE_BUTTONS 2
#define RIGHT_CLICK_BUTTON 1
#define LEFT_CLICK_BUTTON 0

enum Keybindings {ARROW_DOWN,
                  ARROW_UP,
				  ARROW_LEFT,
				  ARROW_RIGHT,
				  SPACE,
				  DELETE,
				  MAX_KEYS};

typedef enum Keybindings Keybindings;

struct ControlsData{
  /* THIS HAS TO STAY EXACTLY HERE */
  int objectSelection;
	int keys[MAX_KEYS];
	int mouseButtons[NUM_OF_MOUSE_BUTTONS];
};
typedef struct ControlsData ControlsData;

int keydown(ControlsData *controlsData, GameObjectData *gameObjectData, GraphicsData *graphicsData, UIData *uiData, SDL_Event *event);
int keyup(ControlsData *controlsData, GameObjectData *gameObjectData, UIData *uiData, SDL_Event *event);
void panScreen(GraphicsData *graphicsData, ControlsData *controlsData, int delta_t);
void initControlData(ControlsData *controlsData);
int handleEvent(SDL_Event *event, GameObjectData *gameObjectData, UIData *uiData, ControlsData *controlsData, GraphicsData *graphicsData);
