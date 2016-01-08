#include "controls.h"

static int isMouseFree(ControlsData *controlsData);

static int isMouseFree(ControlsData *controlsData){
	int i = 0;
	while(i < NUM_OF_MOUSE_BUTTONS){
		if(controlsData->mouseButtons[i]){return 0;}
		i++;
	}
	return 1;
}

int handleEvent(SDL_Event *event, GameObjectData *gameObjectData, UIData *uiData, ControlsData *controlsData, GraphicsData *graphicsData){

		switch (event->type)
		{
			/* Closing the Window will exit the program */
			case SDL_WINDOWEVENT:
				switch (event->window.event){
					case SDL_WINDOWEVENT_RESIZED:
						printf("window resized\n");
						UIRoot_Execute(uiData,WINDOW_RESIZE,0,event);
						break;
				}
				break;
			case SDL_MOUSEMOTION:
				UIRoot_Execute(uiData,MOTION,0,event);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(isMouseFree(controlsData)){
					/* We only want to catch new mouse clicks if the mouse is not having
					   a button held down */
					/* trust me */
					if(event->button.button == SDL_BUTTON_LEFT){
					  UIRoot_ExecuteUpwards(uiData,LEFT_CLICK,1,event);
					  if(!controlsData->objectSelectionOff){
					  	objectInfoDisplay(gameObjectData, graphicsData, &event->button);
								controlsData->mouseButtons[LEFT_CLICK_BUTTON] = 1;
					  }
					  
					}
					else if(event->button.button == SDL_BUTTON_RIGHT){
					  UIRoot_ExecuteUpwards(uiData,RIGHT_CLICK,1,event);
					  if(!controlsData->objectSelectionOff){					 
					  	objectInfoDisplay(gameObjectData, graphicsData, &event->button);
								controlsData->mouseButtons[RIGHT_CLICK_BUTTON] = 1;
					  }
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event->button.button == SDL_BUTTON_LEFT){
				  UIRoot_Execute(uiData,LEFT_RELEASE,0,event);
							controlsData->mouseButtons[LEFT_CLICK_BUTTON] = 0;
				}
				else if(event->button.button == SDL_BUTTON_RIGHT){
				  UIRoot_Execute(uiData,RIGHT_RELEASE,0,event);
							controlsData->mouseButtons[RIGHT_CLICK_BUTTON] = 0;
				}
				break;
			case SDL_KEYDOWN:
				keydown(controlsData,gameObjectData, graphicsData, uiData,event);
				break;
			case SDL_KEYUP:
				keyup(controlsData,gameObjectData, uiData,event);
				break;
			case SDL_QUIT:
				printf("exiting\n");
				exit(0);
				break;
		}
		return 1;
}

void keydown(ControlsData *controlsData, GameObjectData *gameObjectData, GraphicsData *graphicsData, UIData *uiData, SDL_Event *event){
    switch (event->key.keysym.scancode){
        case (SDL_SCANCODE_DOWN):
			controlsData->keys[ARROW_DOWN] = 1;
			graphicsData->trackingMode = 0;
            break;
        case (SDL_SCANCODE_UP):
			controlsData->keys[ARROW_UP] = 1;
			graphicsData->trackingMode = 0;
            break;
        case (SDL_SCANCODE_RIGHT):
			controlsData->keys[ARROW_RIGHT] = 1;
			graphicsData->trackingMode = 0;
            break;
        case (SDL_SCANCODE_LEFT):
			controlsData->keys[ARROW_LEFT] = 1;
			graphicsData->trackingMode = 0;
            break;
		case (SDL_SCANCODE_P):
            gameObjectData->pause_status = 1 - gameObjectData->pause_status; /* 1 for pause, 0 for go on */
			UIRoot_Execute(uiData,RESPONSE_PAUSE,0);
            break;
		case (SDL_SCANCODE_Q):
			printf("DELETE key pressed\n");
			controlsData->keys[DELETE] = 1;
			UIRoot_ExecuteUpwards(uiData,RESPONSE_DELETE,1);
			break;
        default:
			return;
    }
}

void keyup(ControlsData *controlsData, GameObjectData *gameObjectData, UIData *uiData, SDL_Event *event){
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
		case (SDL_SCANCODE_DELETE):
			controlsData->keys[DELETE] = 0;
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

void initControlData(ControlsData *controlsData){
	int i = 0;
	while(i < MAX_KEYS){
		controlsData->keys[i] = 0;
		i++;
	}
	i = 0;
	while(i < NUM_OF_MOUSE_BUTTONS){
		controlsData->mouseButtons[i] = 0;
		i++;
	}
	controlsData->objectSelectionOff = 0;
}
