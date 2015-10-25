#include "game.h"

float calculateDt(float previousRunTime){
  return (float) SDL_GetTicks() - previousRunTime;
}

int gameStart(SDL_Window *window){
  /* SDL_Window *window = the pointer to the program's window
     This function will start the game looping.
     The return should be 0 unless there's an error we need to catch.*/

  /* By creating a new struct to hold our game data, we can more easily
     pass data between functions */
  GameData gameData;
  int gameLoopReturn = 1;

  printf("gameStart\n");

  /* We will need the window pointer for later, so we should store that. */
  gameData.window = window;

  /* We also need some time information to make things run smoothly */
  gameData.gameStartTime = SDL_GetTicks();
  gameData.gameRunTime = (float) gameData.gameStartTime;


  gameData.gameObjectData.resourceNodeSpawners[0] = createResourceNodeSpawner(10,340.0,280.0,200.0);
  gameData.gameObjectData.resourceNodeSpawnerCount = 1;
  gameData.gameObjectData.programmableWorkers[0] = createProgrammableWorker();
  gameData.gameObjectData.programmableWorkerCount = 1;

  gameData.nodeGraphic = SDL_LoadBMP("src/images/blueFlower.bmp");
  SDL_SetColorKey(gameData.nodeGraphic, SDL_TRUE, SDL_MapRGB(gameData.nodeGraphic->format, 255,255,255));

  gameData.workerGraphic = SDL_LoadBMP("src/images/bee.bmp");
  SDL_SetColorKey(gameData.workerGraphic, SDL_TRUE, SDL_MapRGB(gameData.workerGraphic->format, 255,255,255));

  while(gameLoopReturn){
    gameLoopReturn = gameLoop(&gameData);
  }
  return(0);
}

int gameLoop(GameData *gameData){
  /* GameData *gameData = the pointer to the gameData struct that we're using
     to save on the arguments passed to functions.
     This function should be called every frame */

  /* delta_t is the time elapsed since the last time this function ran */
  float delta_t = calculateDt(gameData->gameRunTime);
  SDL_Event event;
  SDL_Rect rect1,rect2;
  int i = 0, j = 0;


  rect1.x = 0;
  rect1.y = 0;
  rect1.w = 20;
  rect1.h = 20;
  rect2.w = 40;
  rect2.h = 40;

  if(delta_t<1.0){
    return(1);
  }

  gameData->gameRunTime = (float) SDL_GetTicks();

  SDL_FillRect(SDL_GetWindowSurface(gameData->window),NULL,0x000000);


  while(i<gameData->gameObjectData.resourceNodeSpawnerCount){
    j = 0;

    updateResourceNodeSpawner(&gameData->gameObjectData.resourceNodeSpawners[i],delta_t);

    while(j<gameData->gameObjectData.resourceNodeSpawners[i].maximumNodeCount){
      if(gameData->gameObjectData.resourceNodeSpawners[i].resourceNodes[j].alive){
        rect1.x = gameData->gameObjectData.resourceNodeSpawners[i].resourceNodes[j].xPosition - 10;
        rect1.y = gameData->gameObjectData.resourceNodeSpawners[i].resourceNodes[j].yPosition - 10;
        SDL_BlitSurface(gameData->nodeGraphic,NULL,SDL_GetWindowSurface(gameData->window),&rect1);

      }
      j++;
    }
    i++;
  }

  i = 0;
  while(i<1){
    updateProgrammableWorker(&gameData->gameObjectData.programmableWorkers[0],&gameData->gameObjectData,delta_t);
    rect2.x = gameData->gameObjectData.programmableWorkers[0].xPosition - 20;
    rect2.y = gameData->gameObjectData.programmableWorkers[0].yPosition - 20;
    SDL_BlitSurface(gameData->workerGraphic,NULL,SDL_GetWindowSurface(gameData->window),&rect2);
    i++;
  }


  SDL_UpdateWindowSurface( gameData->window );

	while (SDL_PollEvent(&event))
	{

		switch (event.type)
		{
			/* Closing the Window or pressing Escape will exit the program */

			case SDL_QUIT:
				exit(0);
			break;

		}

	}
  return(1);
}
