#include "controls.h"

void keydown(ControlsData *controlsData, GameObjectData *gameObjectData, SDL_Event *event){
    switch (event->key.keysym.scancode){
        case (SDL_SCANCODE_DOWN):
			controlsData->keys[ARROW_DOWN] = 1;
            break;
        case (SDL_SCANCODE_UP):
			controlsData->keys[ARROW_UP] = 1;
            break;
        case (SDL_SCANCODE_RIGHT):
			controlsData->keys[ARROW_RIGHT] = 1;
            break;
        case (SDL_SCANCODE_LEFT):
			controlsData->keys[ARROW_LEFT] = 1;
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

void keyup(ControlsData *controlsData, GameObjectData *gameObjectData, SDL_Event *event){
    switch (event->key.keysym.scancode){
        case (SDL_SCANCODE_DOWN):
			controlsData->keys[ARROW_DOWN] = 0;
            break;
        case (SDL_SCANCODE_UP):
			controlsData->keys[ARROW_UP] = 0;
            break;
		case (SDL_SCANCODE_RIGHT):
			controlsData->keys[ARROW_RIGHT] = 0;
            break;
        case (SDL_SCANCODE_LEFT):
			controlsData->keys[ARROW_LEFT] = 0;
            break;
        default:
            return;
    }
}

void panScreen(GraphicsData *graphicsData, ControlsData *controlsData, int delta_t)
{
  int window_x,window_y;
  SDL_GetWindowSize(graphicsData->window,&window_x,&window_y);

  if(window_x > X_SIZE_OF_WORLD){
    graphicsData->navigationOffset.x = -(X_SIZE_OF_WORLD - window_x)/2;
  }
  else{

    if(controlsData->keys[ARROW_RIGHT]){
    	if(graphicsData->navigationOffset.x > -X_SIZE_OF_WORLD + window_x){
    		graphicsData->navigationOffset.x -= floor(delta_t * PANSPEEDMULTI);
    	}
    }

    if(controlsData->keys[ARROW_LEFT]){
      if(graphicsData->navigationOffset.x < 0){
         graphicsData->navigationOffset.x += floor(delta_t * PANSPEEDMULTI);
      }
    }

  }

  if(window_y > Y_SIZE_OF_WORLD){
    graphicsData->navigationOffset.y = -(Y_SIZE_OF_WORLD - window_y)/2;
  }
  else{

    if(controlsData->keys[ARROW_DOWN]){
    	if(graphicsData->navigationOffset.y > -Y_SIZE_OF_WORLD + window_y){
    		graphicsData->navigationOffset.y -= floor(delta_t * PANSPEEDMULTI);
    	}
    }

    if(controlsData->keys[ARROW_UP]){
      if(graphicsData->navigationOffset.y < 0){
  		    graphicsData->navigationOffset.y += floor(delta_t * PANSPEEDMULTI);
  	  }
    }

  }

}

void zeroControlKeys(ControlsData *controlsData){
	int i = 0;
	while(i < MAX_KEYS){
		controlsData->keys[i] = 0;
		i++;
	}
}
