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
		uiData->event = event;
		switch (event->type)
		{
			/* Closing the Window will exit the program */
			case SDL_WINDOWEVENT:
				switch (event->window.event){
					case SDL_WINDOWEVENT_RESIZED:
						UIRoot_Execute(uiData,WINDOW_RESIZE,0);
						break;
				}
				break;
			case SDL_MOUSEMOTION:
				UIRoot_Execute(uiData,MOTION,0);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(isMouseFree(controlsData)){
					/* We only want to catch new mouse clicks if the mouse is not having
					   a button held down */
					/* trust me */
					if(event->button.button == SDL_BUTTON_LEFT){
					  UIRoot_ExecuteUpwards(uiData,LEFT_CLICK,1);
						selectGameObject(gameObjectData,graphicsData,event);

					}
					else if(event->button.button == SDL_BUTTON_RIGHT){
					  UIRoot_ExecuteUpwards(uiData,RIGHT_CLICK,1);
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event->button.button == SDL_BUTTON_LEFT){
				  UIRoot_Execute(uiData,LEFT_RELEASE,0);
							controlsData->mouseButtons[LEFT_CLICK_BUTTON] = 0;
				}
				else if(event->button.button == SDL_BUTTON_RIGHT){
				  UIRoot_Execute(uiData,RIGHT_RELEASE,0);
							controlsData->mouseButtons[RIGHT_CLICK_BUTTON] = 0;
				}
				break;
			case SDL_KEYDOWN:
				return keydown(controlsData,gameObjectData, graphicsData, uiData,event);
				break;
			case SDL_KEYUP:
				return keyup(controlsData,gameObjectData, uiData,event);
				break;
			case SDL_QUIT:
				return 0;
			case SDL_MOUSEWHEEL:
				UIRoot_Execute(uiData,MOUSEWHEEL,0);
				break;
		    default:
		    	if(gameObjectData->gameOverEventNum == event->type){
		      		UIRoot_Execute(uiData,GAME_OVER,0);
		      	}else if(gameObjectData->objectDisplayEventNum == event->type){
		      		UIRoot_Execute(uiData,OBJECT_DISPLAY,0);
		      	}
		    	break;
		}
		return 1;
}

int keydown(ControlsData *controlsData, GameObjectData *gameObjectData, GraphicsData *graphicsData, UIData *uiData, SDL_Event *event){
		(void)gameObjectData;
    switch (event->key.keysym.scancode){
      case (SDL_SCANCODE_DOWN):
      	if(!gameObjectData->gameOver){
					controlsData->keys[ARROW_DOWN] = 1;
					graphicsData->trackingMode = 0;
				}
        break;
      case (SDL_SCANCODE_UP):
      	if(!gameObjectData->gameOver){
					controlsData->keys[ARROW_UP] = 1;
					graphicsData->trackingMode = 0;
				}
        break;
      case (SDL_SCANCODE_RIGHT):
        if(!gameObjectData->gameOver){
					controlsData->keys[ARROW_RIGHT] = 1;
					graphicsData->trackingMode = 0;
				}
        break;
      case (SDL_SCANCODE_LEFT):
        if(!gameObjectData->gameOver){
					controlsData->keys[ARROW_LEFT] = 1;
					graphicsData->trackingMode = 0;
				}
        break;
			case (SDL_SCANCODE_P):
        gameObjectData->pause_status = 1 - gameObjectData->pause_status; /* 1 for pause, 0 for go on */
				UIRoot_Execute(uiData,RESPONSE_PAUSE,0);
        break;
			case (SDL_SCANCODE_Q):
				controlsData->keys[DELETE] = 1;
				UIRoot_ExecuteUpwards(uiData,RESPONSE_DELETE,1);
				break;
			case (SDL_SCANCODE_RETURN):
			case (SDL_SCANCODE_KP_ENTER):
				if(gameObjectData->gameOver){
					return 2;
				}
				break;
			case (SDL_SCANCODE_H):
				centerViewOnHive(graphicsData,gameObjectData);
				break;

			#if DEV_HACKS==1
			case (SDL_SCANCODE_S):
				printf("DEV HACKING TOGGLING WEATHER TO SUN\n");
				gameObjectData->environment.weather.tickCount = -1;
				gameObjectData->environment.weather.present_weather = Sun;
				break;
			case (SDL_SCANCODE_R):
				printf("DEV HACKING TOGGLING WEATHER TO RAIN\n");
				gameObjectData->environment.weather.tickCount = -1;
				gameObjectData->environment.weather.present_weather = Rain;
				break;
			case (SDL_SCANCODE_W):
				printf("DEV HACKING TOGGLING SEASON TO WINTER\n");
				gameObjectData->environment.winterCountdownFloat = WINTER_THRESHOLD+1;
				break;
			case (SDL_SCANCODE_A):
				printf("DEV HACKING TOGGLING SEASON TO AUTUMN\n");
				gameObjectData->environment.winterCountdownFloat = AUTUMN_THRESHOLD;
				break;
			case (SDL_SCANCODE_M):
				printf("DEV HACKING 1000 DAYS OF SUMMER\n");
				gameObjectData->environment.season = SUMMER;
				gameObjectData->environment.winterCountdownFloat = 1000.0;
				break;
			case SDL_SCANCODE_G:
				printf("DEV HACKING A LOT OF HONEY\n");
				gameObjectData->hive.flowers_collected += 50000;
				break;
			case SDL_SCANCODE_N:
				printf("DEV HACKING TO GO TO NEXT WEATHER\n");
				gameObjectData->environment.weather.tickCount = TICKSPERWEATHER;
				break;
			case SDL_SCANCODE_J:
				printf("DEV HACKING TO STOP SEASONS PROGRESSING\n");
				gameObjectData->environment.winterCountdownSpeed = (!(gameObjectData->environment.winterCountdownSpeed)) * WINTER_COUNTDOWN_SPEED;
				break;
			case SDL_SCANCODE_L:
				printf("DEV HACKING ANOTHER BEE INTO EXISTENCE\n");
				createProgrammableWorker(gameObjectData);
				break;
			case SDL_SCANCODE_K:
				printf("DEV HACKING A BEE TO DEATH\n");
				ProgrammableWorker *p = gameObjectData->first_programmable_worker;
				killProgrammableWorker(gameObjectData,&p);
				break;
			#endif
	    default:
				return 1;
    }
		return 1;
}

int keyup(ControlsData *controlsData, GameObjectData *gameObjectData, UIData *uiData, SDL_Event *event){
		(void)gameObjectData;
		(void)uiData;
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
		      return 1;
    }
		return 1;
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
	controlsData->objectSelection = 1;
}
