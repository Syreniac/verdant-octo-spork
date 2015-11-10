#include "controls.h"

void keydown(GraphicsData *graphicsData, GameObjectData *gameObjectData, SDL_Event *event){
    switch (event->key.keysym.scancode){
        case (SDL_SCANCODE_DOWN):
			graphicsData->keys[ARROW_DOWN] = 1;
            break;
        case (SDL_SCANCODE_UP):
			graphicsData->keys[ARROW_UP] = 1;
            break;
        case (SDL_SCANCODE_RIGHT):
			graphicsData->keys[ARROW_RIGHT] = 1;
            break;
        case (SDL_SCANCODE_LEFT):
			graphicsData->keys[ARROW_LEFT] = 1;
            break;
		case (SDL_SCANCODE_P):
            gameObjectData->pause_status = 1 - gameObjectData->pause_status; /* 1 for pause, 0 for go on */
            if (gameObjectData->pause_status){
               /*put a big recentage in the middle of the screen, with a big PAUSE*/
               printf("Test1: %d\n", gameObjectData->pause_status);
               /*output pause_status to other part of program*/

            }
            else{
               /*remove the big recentage in the middle of the screen, and continue the clock*/
               printf("Test2: %d\n", gameObjectData->pause_status);
               /*output pause_status to other part of program*/
            }
            break;
        default:
			return;
    }
}

void keyup(GraphicsData *graphicsData, GameObjectData *gameObjectData, SDL_Event *event){
    switch (event->key.keysym.scancode){
        case (SDL_SCANCODE_DOWN):
			graphicsData->keys[ARROW_DOWN] = 0;
            break;
        case (SDL_SCANCODE_UP):
			graphicsData->keys[ARROW_UP] = 0;
            break;
		case (SDL_SCANCODE_RIGHT):
			graphicsData->keys[ARROW_RIGHT] = 0;
            break;
        case (SDL_SCANCODE_LEFT):
			graphicsData->keys[ARROW_LEFT] = 0;
            break;
        default:
            return;
    }
}

void panScreen(GraphicsData *graphicsData, float delta_t)
{
  if(graphicsData->keys[ARROW_DOWN]){
	if(!(graphicsData->navigationOffset.y <= -Y_SIZE_OF_WORLD + Y_SIZE_OF_SCREEN)){
		graphicsData->navigationOffset.y -= floor(delta_t * PANSPEEDMULTI);
	}
  }
  
  if(graphicsData->keys[ARROW_UP]){
    if(!(graphicsData->navigationOffset.y >= Y_SIZE_OF_WORLD - Y_SIZE_OF_SCREEN)){
		graphicsData->navigationOffset.y += floor(delta_t * PANSPEEDMULTI);
	}
  }

if(graphicsData->keys[ARROW_RIGHT]){
	if(!(graphicsData->navigationOffset.x <= -X_SIZE_OF_WORLD + X_SIZE_OF_SCREEN)){
		graphicsData->navigationOffset.x -= floor(delta_t * PANSPEEDMULTI);
	}
  }
  
  if(graphicsData->keys[ARROW_LEFT]){
    if(!(graphicsData->navigationOffset.x >= X_SIZE_OF_WORLD - X_SIZE_OF_SCREEN)){
         graphicsData->navigationOffset.x += floor(delta_t * PANSPEEDMULTI);
	}
  }
		
}
