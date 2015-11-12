#include "game.h"

float calculateDt(float previousRunTime){
  /* float previousRunTime = the milliseconds you want to calculate from
     This returns the difference in milliseconds between now and another time
     in the past. It's used here to help keep the main loop running
     consistently. */
  return (float) SDL_GetTicks() - previousRunTime;
}

int gameStart(SDL_Window *window){
  /* SDL_Window *window = the pointer to the program's window
     This function will start the game looping.
     The return should be 0 unless there's an error we need to catch.*/

  /* By creating a new struct to hold our game data, we can more easily
     pass data between functions */
  GameData gameData;
  FILE *file;
  int gameLoopReturn = 1;
  SDL_Rect rect;
  SDL_Rect rect2;

  gameData.gameObjectData.pause_status = 0;
  
  memset(gameData.graphicsData.keys,0,sizeof(int) * NUM_OF_KEYS);
  
  gameData.graphicsData.renderer = SDL_CreateRenderer(window,
                                                      -1,
                                                      SDL_RENDERER_TARGETTEXTURE);

  
  /* We will need the window pointer for later, so we should store that. */
  gameData.graphicsData.window = window;

  /* initialise navigationOffset values */
  gameData.graphicsData.navigationOffset.x = -(X_SIZE_OF_WORLD/2)+(X_SIZE_OF_SCREEN/2); /*setting initial x offset to center of world*/
  gameData.graphicsData.navigationOffset.y = -(Y_SIZE_OF_WORLD/2)+(Y_SIZE_OF_SCREEN/2); /*setting initial y offset ot center of world*/
  gameData.graphicsData.navigationOffset.w = X_SIZE_OF_WORLD;
  gameData.graphicsData.navigationOffset.h = Y_SIZE_OF_WORLD;


  /* We also need some time information to make things run smoothly */
  gameData.gameStartTime = SDL_GetTicks();
  gameData.gameRunTime = (float) gameData.gameStartTime;

  rect.x = 0;
  rect.y = 0;
  rect.w = 40;
  rect.h = 40;

  gameData.uiData.UIElements[0] = createUI_Clickable(rect, "Hello", 50,50,150);

  rect.x = 100;
  rect.y = 100;
  rect.w = 40;
  rect.h = 40;

  rect2.x = 100;
  rect2.y = 100;
  rect2.w = 160;
  rect2.h = 160;

  gameData.uiData.UIElements[1] = createUI_Expandable(rect,rect2,1000,1000, 0, 0, 200);

  rect.x = 100;
  rect.y = 100;
  rect.w = 20;
  rect.h = 20;

  gameData.uiData.UIElements[2] = createUI_Draggable(rect,&gameData.uiData.UIElements[1],0,136,64);
  gameData.uiData.numberOfUIElements = 3;
  
  
  /* Create some ResourceNodeSpawners to fill our world with ResourceNodes */
  generateResourceNodeSpawners(&gameData.gameObjectData);

  /* Create some ProgammableWorkers to carry out our tasks */
  generateProgrammableWorkers(&gameData.gameObjectData);

  /* This doesn't actually do much, but it lets us give the workers somewhere
     to return home to */
  generateHive(&gameData.gameObjectData);

  /* Load in the BMPs for our ResourceNodes and ProgrammableWorkers */
  gameData.graphicsData.nodeGraphic = SDL_LoadBMP("images/blueFlower.bmp");
  gameData.graphicsData.workerGraphic = SDL_LoadBMP("images/bee.bmp");
  gameData.graphicsData.hiveGraphic = SDL_LoadBMP("images/beehive.bmp");
  
  if(gameData.graphicsData.nodeGraphic == NULL ||
     gameData.graphicsData.workerGraphic == NULL ||
     gameData.graphicsData.hiveGraphic == NULL){
    printf("IMAGE CANNOT BE FOUND\n");
  }
  


  SDL_SetColorKey(gameData.graphicsData.nodeGraphic, SDL_TRUE,
                  SDL_MapRGB(gameData.graphicsData.nodeGraphic->format, 255, 255, 255));
  SDL_SetColorKey(gameData.graphicsData.workerGraphic, SDL_TRUE,
                  SDL_MapRGB(gameData.graphicsData.workerGraphic->format, 255,255,255));
  SDL_SetColorKey(gameData.graphicsData.hiveGraphic, SDL_TRUE,
                  SDL_MapRGB(gameData.graphicsData.hiveGraphic->format, 255,255,255)); 
  
  
  
  gameData.graphicsData.nodeTexture = SDL_CreateTextureFromSurface(gameData.graphicsData.renderer,
                                                                   gameData.graphicsData.nodeGraphic);
                                                                   
  gameData.graphicsData.workerTexture = SDL_CreateTextureFromSurface(gameData.graphicsData.renderer,
                                                                   gameData.graphicsData.workerGraphic);
                                                                   
  gameData.graphicsData.hiveTexture = SDL_CreateTextureFromSurface(gameData.graphicsData.renderer,
                                                                   gameData.graphicsData.hiveGraphic);


    
    
  gameData.aiData.blockFunctionRoots = calloc(1, sizeof(BlockFunctionRoot));
  file = fopen("GenericWorkerAI.txt","r");
  makeBlockFunctionRootFromFile(&(gameData.aiData.blockFunctionRoots[0]), file);
  fclose(file);

  /* Then run the gameLoop until it returns 0 or exits */
  printf("gameStarted\n");
  while(gameLoopReturn){
    gameLoopReturn = gameLoop(&gameData);
  }
  return(0);
}


int gameLoop(GameData *gameData){
  /* GameData *gameData = the pointer to the gameData struct that we're using
     to save on the arguments passed to functions.
     This function should be called every frame */

  /* delta_t is the time in milliseconds elapsed since the last time this
     function ran */
  float delta_t = calculateDt(gameData->gameRunTime);
  SDL_Event event;
  
  /* Storing the number of milliseconds since the program was run helps keep it
     moving smoothly by calculating delta_t */
  gameData->gameRunTime = (float) SDL_GetTicks();
  
  
  /*clear helps get rid of things on the screen that shouldn't be there anymore
  and is also essential due to the way that render buffers can change unpredictably
  after the function SDL_RenderPresent*/
  SDL_RenderClear(gameData->graphicsData.renderer);

  updateGameObjects(&gameData->gameObjectData, &gameData->graphicsData, delta_t);

  runAI(&gameData->aiData,&gameData->gameObjectData);

  renderUI(&gameData->uiData, &gameData->graphicsData);

  panScreen(&gameData->graphicsData, delta_t);
  
  
  /*This function is like the blit function, putting pixels to the screen.
  but it needs to be called after all of the graphicall changes have been made,
  including those in the renderUI() function. After the call to render present
  the pixel buffer becomes unpredictable and should be followed by SDL_RenderClear
  (as above in this loop)*/
  SDL_RenderPresent(gameData->graphicsData.renderer);
				
  /* At the end of the loop we need to update the main application window to
     reflect the changes we've made to the graphics */
  /*SDL_UpdateWindowSurface(gameData->graphicsData.window);*/

  /* This bit makes sure our application keeps responding and doesn't crash */
  /* Don't worry too much about this for now */
	while (SDL_PollEvent(&event))
	{

		switch (event.type)
		{
			/* Closing the Window will exit the program */
      case SDL_MOUSEMOTION:

        moveMouseOnUi(&gameData->uiData,&event);
        break;
      case SDL_MOUSEBUTTONUP:
        clickupOnUI(&gameData->uiData, &event);
        break;
      case SDL_MOUSEBUTTONDOWN:
        clickOnUI(&gameData->uiData, &event);
        break;
      case SDL_KEYDOWN:
        keydown(&gameData->graphicsData, &gameData->gameObjectData, &event);
        break;
	  case SDL_KEYUP:
		keyup(&gameData->graphicsData, &gameData->gameObjectData, &event);
		break;
	  case SDL_QUIT:
		exit(0);
	    break;
		}
	}
  SDL_Delay(16);
  return(1);
}
