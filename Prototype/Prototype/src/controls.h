#include "UI.h"

enum Keybindings {ARROW_DOWN,
                  ARROW_UP,
				  ARROW_LEFT,
				  ARROW_RIGHT,
				  SPACE,
				  MAX_KEYS};

typedef enum Keybindings Keybindings;

struct ControlsData{
	int keys[MAX_KEYS];
};
typedef struct ControlsData ControlsData;

void keydown(ControlsData *controlsData, GameObjectData *gameObjectData, SDL_Event *event);
void keyup(ControlsData *controlsData, GameObjectData *gameObjectData, SDL_Event *event);
void panScreen(GraphicsData *graphicsData, ControlsData *controlsData, int delta_t);
void zeroControlKeys(ControlsData *controlsData);