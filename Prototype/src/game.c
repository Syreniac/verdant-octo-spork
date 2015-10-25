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
  int gameLoopReturn = 1;

  /* We will need the window pointer for later, so we should store that. */
  gameData.window = window;

  /* We also need some time information to make things run smoothly */
  gameData.gameStartTime = SDL_GetTicks();
  gameData.gameRunTime = (float) gameData.gameStartTime;

  /* Create some ResourceNodeSpawners to fill our world with ResourceNodes */
  gameData.gameObjectData.resourceNodeSpawners[0] = createResourceNodeSpawner(DEFAULT_MAXNODECOUNT,
                                                                              X_SIZE_OF_SCREEN/2,
                                                                              Y_SIZE_OF_SCREEN/2,
                                                                              DEFAULT_SPAWNRADIUS);

  gameData.gameObjectData.resourceNodeSpawners[1] = createResourceNodeSpawner(DEFAULT_MAXNODECOUNT,
                                                                              X_SIZE_OF_SCREEN/4,
                                                                              Y_SIZE_OF_SCREEN/4,
                                                                              DEFAULT_SPAWNRADIUS);
  /* We need to keep a record of how many resourceNodeSpawners we have active
     so that we can loop through them later */
  gameData.gameObjectData.resourceNodeSpawnerCount = 2;

  /* Create some ProgammableWorkers to carry out our tasks */
  gameData.gameObjectData.programmableWorkers[0] = createProgrammableWorker();
  gameData.gameObjectData.programmableWorkers[1] = createProgrammableWorker();
  /* Make sure the count is correct */
  gameData.gameObjectData.programmableWorkerCount = 2;

  /* This doesn't actually do much, but it lets us give the workers somewhere
     to return home to */
  gameData.gameObjectData.hive = createHive();

  /* Load in the BMPs for our ResourceNodes and ProgrammableWorkers */
  gameData.nodeGraphic = SDL_LoadBMP("src/images/blueFlower.bmp");
  gameData.workerGraphic = SDL_LoadBMP("src/images/bee.bmp");

  /* SDL_SetColorKey makes the program treat all pixels of a given colour as
     transparent (in these cases, white)*/
  SDL_SetColorKey(gameData.nodeGraphic, SDL_TRUE, SDL_MapRGB(gameData.nodeGraphic->format, 255,255,255));
  SDL_SetColorKey(gameData.workerGraphic, SDL_TRUE, SDL_MapRGB(gameData.workerGraphic->format, 255,255,255));

  /* Then run the gameLoop until it returns 0 or exits */
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
  SDL_Rect rect1,rect2;
  int i = 0, j = 0;

  /* because rect1 is used for drawing ResourceNodes, we'll set its height and
     width to the globally defined values for ResourceNodes */

  rect1.w = X_SIZE_OF_NODE;
  rect1.h = Y_SIZE_OF_NODE;

  /* similarly, rect2 is used for drawing ProgrammableWorkers, so we'll use the
     globally defined values again. */

  rect2.w = X_SIZE_OF_WORKER;
  rect2.h = Y_SIZE_OF_WORKER;

  /* There were weird things happening if the delta_t was too low, so putting
     this in makes sure that the program behaves itself */
  if(delta_t<FRAME_RATE){
    return(1);
  }

  /* Storing the number of milliseconds since the program was run helps keep it
     moving smoothly by calculating delta_t */
  gameData->gameRunTime = (float) SDL_GetTicks();

  /* Filling the background black helps get rid of things drawn onto the screen
     that shoudln't be there anymore */
  SDL_FillRect(SDL_GetWindowSurface(gameData->window),NULL,0x000000);


  /* Loop through all the spawners so we can update and draw them all in turn */
  while(i<gameData->gameObjectData.resourceNodeSpawnerCount){
    /* j is the index for individual resourceNodes within the
       resourceNodeSpawner so we need to rezero it for every resourceNodeSpawner */
    j = 0;

    /* This just updates the ResourceNodeSpawner */
    updateResourceNodeSpawner(&(gameData->gameObjectData.resourceNodeSpawners[i]), delta_t);

    /* This draws the resourceNodes onto the screen by iterating through the
       array attached to the ResourceNodeSpawner*/
    while(j<gameData->gameObjectData.resourceNodeSpawners[i].maximumNodeCount){
      if(gameData->gameObjectData.resourceNodeSpawners[i].resourceNodes[j].alive){
        /* This gets the x or y coordinate from the ResourceNode attached to the
           ResourceNodeSpawner selected from gameObjectData in gameData and
           moves the x or y coordinate of rect1 to that position*/
        rect1.x = gameData->gameObjectData.resourceNodeSpawners[i].resourceNodes[j].xPosition - X_SIZE_OF_NODE/2;
        rect1.y = gameData->gameObjectData.resourceNodeSpawners[i].resourceNodes[j].yPosition - Y_SIZE_OF_NODE/2;
        /* Draw the nodeGraphic from gameData on to the screen at rect1's new
           coordinates */
        SDL_BlitSurface(gameData->nodeGraphic,NULL,SDL_GetWindowSurface(gameData->window),&rect1);
      }

      j++;
    }

    i++;
  }

  i = 0;
  /* Loop through all the workers so we can update and draw them all in turn */
  while(i<gameData->gameObjectData.programmableWorkerCount){
    /* Because the ProgrammableWorkers need to know the positions of objects in
       the world, we pass the gameObjectData to it, which holds the arrays of
       game objects */
    updateProgrammableWorker(&gameData->gameObjectData.programmableWorkers[i],&gameData->gameObjectData,delta_t);
    /* Just like with the ResourceNodes above, we move rect2 to the position
       indicated by the ProgrammableWorker */
    rect2.x = gameData->gameObjectData.programmableWorkers[i].xPosition - X_SIZE_OF_WORKER/2;
    rect2.y = gameData->gameObjectData.programmableWorkers[i].yPosition - Y_SIZE_OF_WORKER/2;
    /* Then we draw the workerGraphic to the position of rect2 */
    SDL_BlitSurface(gameData->workerGraphic,NULL,SDL_GetWindowSurface(gameData->window),&rect2);
    i++;
  }

  /* At the end of the loop we need to update the main application window to
     reflect the changes we've made to the graphics */
  SDL_UpdateWindowSurface( gameData->window );

  /* This bit makes sure our application keeps responding and doesn't crash */
  /* Don't worry too much about this for now */
	while (SDL_PollEvent(&event))
	{

		switch (event.type)
		{
			/* Closing the Window will exit the program */

			case SDL_QUIT:
				exit(0);
			break;

		}

	}
  return(1);
}
