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

  /* We will need the window pointer for later, so we should store that. */
  gameData.graphicsData.window = window;

  /* We also need some time information to make things run smoothly */
  gameData.gameStartTime = SDL_GetTicks();
  gameData.gameRunTime = (float) gameData.gameStartTime;

  gameData.uiData.numberOfSimpleButtons = 1;
  gameData.uiData.simpleButtons[0] = createUISimpleButton(0, 0, 40, 40, "Hello", 0xb00000);
  gameData.uiData.numberOfExpandablePanels = 1;
  gameData.uiData.expandablePanels[0] = createExpandingPanel(100, 100, 40, 40,
                                                             100, 100, 160, 160,
                                                             1000,1000,0x00b000);
  gameData.uiData.numberOfDraggableBlocks = 1;
  gameData.uiData.draggableBlocks[0] = createDraggableBlock(0,0,20,20,&gameData.uiData.expandablePanels[0],0x0000b0);


  /* Create some ResourceNodeSpawners to fill our world with ResourceNodes */
  generateResourceNodeSpawners(&gameData.gameObjectData);

  /* Create some ProgammableWorkers to carry out our tasks */
  generateProgrammableWorkers(&gameData.gameObjectData);

  /* This doesn't actually do much, but it lets us give the workers somewhere
     to return home to */
  generateHive(&gameData.gameObjectData);

    SDL_ClearError();
    /* Load in the BMPs for our ResourceNodes and ProgrammableWorkers */
    SDL_RWops * theFile = SDL_RWFromFile("images/bee.bmp", "rb");
    char* theError = SDL_GetError();
    printf(theError);
    SDL_Surface * theBmp = SDL_LoadBMP_RW(theFile, 1);
    theError = SDL_GetError();
    printf(theError);
  gameData.graphicsData.nodeGraphic = theBmp;
  gameData.graphicsData.workerGraphic = SDL_LoadBMP_RW(SDL_RWFromFile("images/bee.bmp", "rb"), 1);
  gameData.graphicsData.hiveGraphic = SDL_LoadBMP_RW(SDL_RWFromFile("images/beehive.bmp", "rb"), 1);

  /* SDL_SetColorKey makes the program treat all pixels of a given colour as
     transparent (in these cases, white)*/
  SDL_SetColorKey(gameData.graphicsData.nodeGraphic, SDL_TRUE, SDL_MapRGB(gameData.graphicsData.nodeGraphic->format, 255,255,255));
  SDL_SetColorKey(gameData.graphicsData.workerGraphic, SDL_TRUE, SDL_MapRGB(gameData.graphicsData.workerGraphic->format, 255,255,255));
  SDL_SetColorKey(gameData.graphicsData.hiveGraphic, SDL_TRUE, SDL_MapRGB(gameData.graphicsData.hiveGraphic->format, 255,255,255));

  gameData.aiData.blockFunctionRoots = calloc(1, sizeof(BlockFunctionRoot));
  file = fopen("ai/GenericWorkerAI.txt","r");
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
  int i = 0, j = 0;

  /* Storing the number of milliseconds since the program was run helps keep it
     moving smoothly by calculating delta_t */
  gameData->gameRunTime = (float) SDL_GetTicks();

  /* Filling the background black helps get rid of things drawn onto the screen
     that shoudln't be there anymore */
  SDL_FillRect(SDL_GetWindowSurface(gameData->graphicsData.window),NULL,0x1B8D2E);

  updateGameObjects(&gameData->gameObjectData, &gameData->graphicsData, delta_t);

  runAI(&gameData->aiData,&gameData->gameObjectData);

  updateUI(&gameData->uiData, &gameData->graphicsData, delta_t);

  /* At the end of the loop we need to update the main application window to
     reflect the changes we've made to the graphics */
  SDL_UpdateWindowSurface(gameData->graphicsData.window);

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
        clickUpOnUI(&gameData->uiData, &event);
        break;
      case SDL_MOUSEBUTTONDOWN:
        clickDownOnUI(&gameData->uiData, &event);
        break;
			case SDL_QUIT:
				exit(0);
			  break;
		}
	}
  SDL_Delay(16);
  return(1);
}
