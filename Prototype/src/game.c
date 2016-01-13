#include "game.h"

#define OBJECT_SELECTION_CONTROL &gameData->controlsData.objectSelection

static void createGameUI(GameData *gameData);
static void cleanUpGameData(GameData *gameData);

int calculateDt(int previousRunTime){
  /* float previousRunTime = the milliseconds you want to calculate from
     This returns the difference in milliseconds between now and another time
     in the past. It's used here to help keep the main loop running
     consistently. */
  return SDL_GetTicks() - previousRunTime;
}



int gameStart(GraphicsData graphicsData, AudioData audioData, ConfigurationData configData){
  /* SDL_Window *window = the pointer to the program's window
     This function will start the game looping.
     The return should be 0 unless there's an error we need to catch.*/

  /* Only things that can only be done when the game is started should go here.
     Things like graphics & audio initialisation should go in init.c instead.
	 Ideally, we also want this to be mostly function calls rather than actually doing
	 any work in this function */

  /* By creating a new struct to hold our game data, we can more easily
     pass data between functions */
  GameData gameData;
  FILE *file;
  int gameLoopReturn;

  gameLoopReturn = 1;

  gameData.graphicsData = graphicsData;
  gameData.audioData = audioData;


  gameData.uiData.root = UIElement_Create(0,0,0,0,0);
  gameData.running = 1;

  initAudio(&gameData.gameObjectData, gameData.audioData);

  gameData.gameObjectData.pause_status = 0;
  gameData.gameObjectData.first_programmable_worker = NULL;
  gameData.gameObjectData.gameOver = 0;
  gameData.gameObjectData.gameOverBoxVisible = 0;
  gameData.gameObjectData.gameOverEventNum = SDL_RegisterEvents(1);
  gameData.gameObjectData.objectDisplayEventNum = SDL_RegisterEvents(1);
  gameData.gameObjectData.gameRestart = 0;

  initControlsData(&gameData.controlsData,&configData);
  initGameObjectData(&gameData.gameObjectData,&configData);
  /* initialise navigationOffset values */

  gameData.graphicsData.trackingMode = 0;

  printf(">>>>>> %p\n",graphicsData.fonts[0]);


  /* We also need some time information to make things run smoothly */
  gameData.gameStartTime = SDL_GetTicks();
  gameData.gameRunTime = gameData.gameStartTime;


  /* Create some ResourceNodeSpawners to fill our world with ResourceNodes */
  generateResourceNodeSpawners(&gameData.gameObjectData);

  /* Create some ProgammableWorkers to carry out our tasks */

  generateProgrammableWorkers(&gameData.gameObjectData);

  /*hive must be generated before trees, as this allows the last tree to be placed near the hive*/
  generateTrees(&gameData.gameObjectData);
  generateWeatherLayer(&gameData.gameObjectData);

  generateIceCreamPerson(&gameData.gameObjectData);
  generateDroppedIceCream(&gameData.gameObjectData);

  generateRoamingSpider(&gameData.gameObjectData);

  memset(&gameData.gameObjectData.droppedIceCream->rect.x,256,sizeof(int));
  memset(&gameData.gameObjectData.droppedIceCream->rect.y,256,sizeof(int));
  gameData.aiData = initAIData();

  initUIData(&gameData.uiData);
  gameData.uiData.graphicsData = &gameData.graphicsData;
  gameData.uiData.gameObjectData = &gameData.gameObjectData;
  gameData.uiData.aiData = &gameData.aiData;
  gameData.uiData.announcementsData = &gameData.announcementsData;
  gameData.uiData.gameOverData = &gameData.announcementsData.gameOverData;
  gameData.uiData.objectInfoDisplay = &gameData.announcementsData.objectInfoDisplay;
  gameData.uiData.ticks = &gameData.delta;
  gameData.uiData.audioData = &gameData.audioData;
  createGameUI(&gameData);

  /* Then run the gameLoop until it returns 0 or exits */
  announce_init(&gameData.announcementsData);
  gameOverInfo_init(&gameData.announcementsData.gameOverData);
  objectInfoDisplay_init(&gameData.announcementsData.objectInfoDisplay);
  playMusic(&gameData.audioData,1);

  centerViewOnHive(&gameData.graphicsData,&gameData.gameObjectData);
  gameLoopReturn = 1;
  while(gameLoopReturn == 1){
    gameLoopReturn = gameLoop(&gameData);
  }

  cleanUpGameData(&gameData);
  return gameLoopReturn;
}

static void cleanUpGameData(GameData *gameData){
  UIRoot_Destroy(&gameData->uiData);
  destroyGameObjectData(&gameData->gameObjectData);
}

static void createGameUI(GameData *gameData){
  UI_Element *element;
  UI_Element *element2;
  UI_Element *element3;
  UI_Element *element4;
  int win_x, win_y;

  BlockFunction *array[255] = {NULL};
  UI_Element *array2[255];
  int i = 0;
  int j = 0;
  int topX = 60;
  int topY = 60;

  SDL_DisplayMode dm;


  SDL_GetWindowSize(gameData->graphicsData.window,&win_x, &win_y);

  element = makeAnnouncementBar(&gameData->uiData);

  element = makeGameOverBox(&gameData->uiData);

  element = makeMuteButton(&gameData->uiData);




      /*objectInfoDisplay box*/
  element = makeObjectDisplayInfo(&gameData->uiData);
        /*objectInfoDisplay box - status*/
  element = makeObjectDisplayLabel(&gameData->uiData);


    /*gameOverInformation*/
  element = makeGameOverInfomation(&gameData->uiData);

  /*press enter to restart (part of game over box)*/
  element = makeGameOverPanel(&gameData->uiData);

  /* Minimap */
  element = makeMinimap(&gameData->uiData,OBJECT_SELECTION_CONTROL);

  element = makePauseBox(&gameData->uiData);


	/*hive button*/
  element2 = makeHiveButton(&gameData->uiData,&gameData->gameObjectData,&gameData->graphicsData,OBJECT_SELECTION_CONTROL);

  /*b++ editor button*/

  element2 = makeBPPEditor(&gameData->uiData,&gameData->aiData,&gameData->graphicsData,OBJECT_SELECTION_CONTROL);
  /* Nullify AI button */
  UIRoot_Pack(&gameData->uiData,&gameData->graphicsData);
}



int gameLoop(GameData *gameData){
  /* GameData *gameData = the pointer to the gameData struct that we're using
     to save on the arguments passed to functions.
     This function should be called every frame */

  /* delta_t is the time in milliseconds elapsed since the last time this
     function ran */
  SDL_Event event;
  int continuing = 1;

  #if BENCHMARK_TEST==1
  int testMarker = 0;
  #endif


  /* Storing the number of milliseconds since the program was run helps keep it
     moving smoothly by calculating delta_t */
  gameData->delta = calculateDt(gameData->gameRunTime);

  gameData->gameRunTime = SDL_GetTicks();

  /*clear helps get rid of things on the screen that shouldn't be there anymore
  and is also essential due to the way that render buffers can change unpredictably
  after the function SDL_RenderPresent*/

  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  #endif

  while (SDL_PollEvent(&event)){
    continuing = handleEvent(&event,&gameData->gameObjectData,&gameData->uiData,&gameData->controlsData, &gameData->graphicsData);
	  if(continuing != 1){
      printf("escape\n");
      return continuing;
    }
  }

  if(!gameData->graphicsData.trackingMode){
  	panScreen(&gameData->graphicsData, &gameData->controlsData, gameData->delta);
  }

  if(SDL_RenderClear(gameData->graphicsData.renderer) == -1){
	   printf("Error clearing renderer: %s\n", SDL_GetError());
  }

  paintBackground(&gameData->graphicsData,0,200,100);

  updateGameObjects(&gameData->gameObjectData, &gameData->audioData, &gameData->graphicsData, &gameData->announcementsData, gameData->delta);


	  UIRoot_Execute(&gameData->uiData,UPDATE,0);
	  runAI(&gameData->aiData,&gameData->gameObjectData,gameData->delta);

	  SDL_RenderPresent(gameData->graphicsData.renderer);
	  announce_update(&gameData->announcementsData, gameData->delta);
	while (SDL_PollEvent(&event))
	{
		handleEvent(&event,&gameData->gameObjectData,&gameData->uiData,&gameData->controlsData, &gameData->graphicsData);
	}
  if (Mix_Playing(1) == 0) {
 	  playMusic(&gameData->audioData,1);
  }

  if(gameData->gameObjectData.gameOver){
  	  if(gameData->gameObjectData.gameOverCause == STARVATION){
  		    gameData->gameObjectData.hive.flowers_collected = 0;
  	  }
  	  if(!gameData->gameObjectData.gameOverBoxVisible){
		    SDL_Event gameOverEvent;
		    gameOverEvent.type = gameData->gameObjectData.gameOverEventNum;
  	  	gameData->gameObjectData.gameOverBoxVisible = 1;
	      SDL_PushEvent(&gameOverEvent);
  	  }
   	  while (SDL_PollEvent(&event)){
        continuing = handleEvent(&event,&gameData->gameObjectData,&gameData->uiData,&gameData->controlsData, &gameData->graphicsData);
    	  if(continuing != 1){
          printf("escape\n");
          return continuing;
        }
      }
  }
  if (Mix_Playing(1) == 0) {
 	  playMusic(&gameData->audioData,1);
  }

  UIRoot_ExecuteUpwards(&gameData->uiData,DISPOSAL,0);

  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  printf("--------------------------\n");
  #endif
  return 1;
}
