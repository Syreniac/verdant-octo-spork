#include "game.h"

static void createGameUI(GameData *gameData);

int calculateDt(int previousRunTime){
  /* float previousRunTime = the milliseconds you want to calculate from
     This returns the difference in milliseconds between now and another time
     in the past. It's used here to help keep the main loop running
     consistently. */
  return SDL_GetTicks() - previousRunTime;
}



int gameStart(GraphicsData graphicsData, AudioData audioData){
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

  gameData.uiData.root = calloc(1, sizeof(UI_Element));
  gameData.running = 1;

  initAudio(&gameData.gameObjectData, gameData.audioData);

  gameData.gameObjectData.pause_status = 0;
  gameData.gameObjectData.first_programmable_worker = NULL;

  initControlData(&gameData.controlsData);

  /* initialise navigationOffset values */
  gameData.graphicsData.navigationOffset.x = X_INITIAL_SCREEN_OFFSET; /*setting initial x offset to center of world*/
  gameData.graphicsData.navigationOffset.y = Y_INITIAL_SCREEN_OFFSET; /*setting initial y offset ot center of world*/
  gameData.graphicsData.navigationOffset.w = X_SIZE_OF_WORLD;
  gameData.graphicsData.navigationOffset.h = Y_SIZE_OF_WORLD;



  /* We also need some time information to make things run smoothly */
  gameData.gameStartTime = SDL_GetTicks();
  gameData.gameRunTime = gameData.gameStartTime;


  /* Create some ResourceNodeSpawners to fill our world with ResourceNodes */
  generateResourceNodeSpawners(&gameData.gameObjectData);

  /* Create some ProgammableWorkers to carry out our tasks */

  generateProgrammableWorkers(&gameData.gameObjectData);

  /* This doesn't actually do much, but it lets us give the workers somewhere
     to return home to */
  generateHive(&gameData.gameObjectData);
  generateTrees(&gameData.gameObjectData);
  generateWeatherLayer(&gameData.gameObjectData);

  generateIceCreamPerson(&gameData.gameObjectData);


  gameData.graphicsData.grassTexture = loadTextureFromFile("images/grass/grass4.bmp",&gameData.graphicsData, 0);
  gameData.graphicsData.treeTexture = loadTextureFromFile("images/tree.bmp",&gameData.graphicsData, 1);
  gameData.graphicsData.nodeTexture = loadTextureFromFile("images/blueFlower.bmp",
														  &gameData.graphicsData, 1);

  gameData.graphicsData.person = (Person*) malloc(sizeof(Person));

  gameData.graphicsData.person->graphic[WITH_ICE_CREAM_STRIDE1] =
  loadTextureFromFile("images/person/withIceCream1.bmp",
					  &gameData.graphicsData, 1);

  gameData.graphicsData.person->graphic[WITH_ICE_CREAM_STRIDE2] =
  loadTextureFromFile("images/person/withIceCream2.bmp",
					  &gameData.graphicsData, 1);
					  
  gameData.graphicsData.person->graphic[WITH_ICE_CREAM_STRIDE1 + NO_ICECREAM_INDEX_OFFSET] =
  loadTextureFromFile("images/person/withoutIceCream1.bmp",
					  &gameData.graphicsData, 1);

  gameData.graphicsData.person->graphic[WITH_ICE_CREAM_STRIDE2 + NO_ICECREAM_INDEX_OFFSET] =
  loadTextureFromFile("images/person/withoutIceCream2.bmp",
					  &gameData.graphicsData, 1);

  gameData.graphicsData.rainy = (Rainy*) malloc(sizeof(Rainy));

  gameData.graphicsData.rainy->graphic[0] = loadTextureFromFile("images/rain/rain1.bmp", &gameData.graphicsData, 1);
  gameData.graphicsData.rainy->graphic[1] = loadTextureFromFile("images/rain/rain2.bmp", &gameData.graphicsData, 1);
  gameData.graphicsData.rainy->graphic[2] = loadTextureFromFile("images/rain/rain3.bmp", &gameData.graphicsData, 1);
  gameData.graphicsData.rainy->graphic[3] = loadTextureFromFile("images/rain/rain4.bmp", &gameData.graphicsData, 1);
  gameData.graphicsData.rainy->graphic[4] = loadTextureFromFile("images/rain/rain5.bmp", &gameData.graphicsData, 1);
  gameData.graphicsData.rainy->graphic[5] = loadTextureFromFile("images/rain/rain6.bmp", &gameData.graphicsData, 1);

  gameData.graphicsData.bee = (Bee*) malloc(sizeof(Bee));


  gameData.graphicsData.bee->graphic[BEE_FLAP_GRAPHIC_1] = loadTextureFromFile("images/bee.bmp",
														  &gameData.graphicsData, 1);
  gameData.graphicsData.bee->graphic[BEE_FLAP_GRAPHIC_2] = loadTextureFromFile("images/bee2.bmp",
														  &gameData.graphicsData, 1);

  gameData.graphicsData.hiveTexture = loadTextureFromFile("images/beehive.bmp",
														  &gameData.graphicsData, 1);

  gameData.aiData.blockFunctionRoots = calloc(1, sizeof(BlockFunctionRoot));
  file = fopenAndVerify("GenericWorkerAI.txt", "r");
  makeBlockFunctionRootFromFile(&(gameData.aiData.blockFunctionRoots[0]), file);
  fclose(file);

  createGameUI(&gameData);

  SDL_Delay(10);
  /* Then run the gameLoop until it returns 0 or exits */
  printf("gameStarted %d\n",gameLoopReturn);

  playMusic(&gameData.audioData,1);
  while(gameLoopReturn){
    gameLoopReturn = gameLoop(&gameData);
  }
  return(0);
}

static void createGameUI(GameData *gameData){
  UI_Element *element;
  UI_Element *element2;
  int win_x, win_y;

  BlockFunction *array[255] = {NULL};
  UI_Element *array2[255];
  int i = 0;
  int j = 0;
  int topX = 60;
  int topY = 60;

  SDL_GetWindowSize(gameData->graphicsData.window,&win_x, &win_y);

  element = UIElement_Create(win_x-150,0,150,50,3);
  UIConfigure_FillRect(element,&element->actions[0],255,255,255);
  UIConfigure_DisplayNumber(element, &element->actions[1], 0,0);
  UIConfigure_ResourceCounter(element, &element->actions[2],1,&element->actions[1]);
  UIElement_Reparent(element,gameData->uiData.root);

  element = UIElement_Create(win_x/2,win_y - 50,200,50,5);
  UIConfigure_Auto(element, &element->actions[0], RESPONSE_PAUSE);
    UITrigger_Bind(&element->actions[0],&element->actions[0],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[1],0,2);
    UITrigger_Bind(&element->actions[0],&element->actions[2],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[3],-1,0);
  element->actions[0].status = 0;
  UIConfigure_Auto(element, &element->actions[1], RESPONSE_PAUSE);
    UITrigger_Bind(&element->actions[1],&element->actions[0],0,2);
    UITrigger_Bind(&element->actions[1],&element->actions[1],1,0);
    UITrigger_Bind(&element->actions[1],&element->actions[2],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[3],0,1);
  element->actions[1].status = 1;
  UIConfigure_FillRect(element, &element->actions[2],255,0,0);
  element->actions[2].status = 0;
  UIConfigure_DisplayString(element, &element->actions[3],"PAUSED",0);
  element->actions[3].status = 0;
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIElement_Reparent(element,gameData->uiData.root);


  element2 = UIElement_Create(0, win_y - 100, 100,100,3);
	UIConfigure_FillRect(element2,&element2->actions[0],0,100,100);
	UIConfigure_LeftClickRect(element2,&element2->actions[1]);
		UITrigger_Bind(&element2->actions[1],&element2->actions[2],0,1);
    UITrigger_Bind(&element2->actions[1],&element2->actions[1],1,0);
	UIConfigure_TwoRectOverride(element2,&element2->actions[2],0,win_y - 100, 100, 100,
                                                               50, 50, win_x - 100, win_y - 200,
                                                               200, 0, 0);
  UIElement_Reparent(element2,gameData->uiData.root);

  element = UIElement_Create(50 + win_x - 150, 50, 50, 50,4);
  UIConfigure_FillRect(element, &element->actions[0],222,0,0);
  UIConfigure_ShrinkFitToParent(element, &element->actions[1]);
  UIConfigure_LeftClickRect(element, &element->actions[2]);
    UITrigger_Bind(&element->actions[2],&element->actions[3],0,1);
  UIConfigure_External(element, &element->actions[3],element2);
    UITrigger_Bind(&element->actions[3], &element2->actions[2], 3,2);
    UITrigger_Bind(&element->actions[3], &element2->actions[1], 0,1);
  UIElement_Reparent(element,element2);

  element = UIElement_Create(50,50,win_x - 150,win_y - 200,2);
  UIConfigure_FillRect(element, &element->actions[0],0,222,0);
  UIConfigure_ShrinkFitToParent(element, &element->actions[1]);
  UIElement_Reparent(element,element2);

  array[i] = &gameData->aiData.blockFunctionRoots[0].blockFunctions[i];
  element2 = UIElement_Create(topX,topY,200,50,5);
  UIConfigure_FillRect(element2,&element2->actions[0],123,321,123);
  UIConfigure_ShrinkFitToParent(element2,&element2->actions[1]);
  UIConfigure_RenderLine(element2,&element2->actions[2],BR_CORNER,NULL);
  UIConfigure_RenderLine(element2,&element2->actions[3],BL_CORNER,NULL);
  UIConfigure_DisplayString(element2, &element2->actions[4],&gameData->aiData.blockFunctionRoots[0].blockFunctions[i].name[0],1);
  UIElement_Reparent(element2,element);
  topX+=210;
  array2[i] = element2;

  i = 1;
  while(i < gameData->aiData.blockFunctionRoots[0].numOfBlockFunctions){
    printf("making something\n");
    array[i] = &gameData->aiData.blockFunctionRoots[0].blockFunctions[i];
    element2 = UIElement_Create(topX,topY,200,50,8);
    UIConfigure_FillRect(element2,&element2->actions[0],123,321,123);
    UIConfigure_ShrinkFitToParent(element2,&element2->actions[1]);
  	UIConfigure_RightClickRect(element2, &element2->actions[2]);
		printf("#\n");
  		UITrigger_Bind(&element2->actions[2],&element2->actions[3],0,1);
		printf("#\n");
  		UITrigger_Bind(&element2->actions[2],&element2->actions[4],0,1);
		printf("#\n");
  		UITrigger_Bind(&element2->actions[2],&element2->actions[2],1,0);
		printf("#\n");
  	UIConfigure_RightReleaseAnywhere(element2, &element2->actions[3]);
  		UITrigger_Bind(&element2->actions[3],&element2->actions[3],1,0);
  		UITrigger_Bind(&element2->actions[3],&element2->actions[4],1,0);
  		UITrigger_Bind(&element2->actions[3],&element2->actions[2],0,1);
  	UIConfigure_DraggableRectOverride(element2, &element2->actions[4],1,&element2->actions[1]);
    UIConfigure_RenderLine(element2,&element2->actions[5],BR_CORNER,NULL);
    UIConfigure_RenderLine(element2,&element2->actions[6],BL_CORNER,NULL);
    UIConfigure_DisplayString(element2, &element2->actions[7],&gameData->aiData.blockFunctionRoots[0].blockFunctions[i].name[0],1);
    UIElement_Reparent(element2,element);
    array2[i] = element2;
    topX+=210;
    if(topX > win_y - 350){
      topX -= win_y -350;
      topY+= 60;
    }
    i++;
  }
  printf("part 1 done\n");

  i = 0;

    if(gameData->aiData.blockFunctionRoots[0].blockFunctions[i].primary != NULL){
      j = 0;
      while(array[j] != gameData->aiData.blockFunctionRoots[0].blockFunctions[i].primary){
        j++;
      }
      assert(array[j] != NULL);
      element = array2[i];
      element->actions[2].external = array2[j];
      element->actions[2].status = 2;
    }
    if(gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary != NULL){
      j = 0;
      while(array[j] != gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary){
        j++;
      }
      assert(array[j] != NULL);
      element = array2[i];
      element->actions[3].external = array2[j];
      element->actions[3].status = 2;
    }
  i++;


  while(i < gameData->aiData.blockFunctionRoots[0].numOfBlockFunctions){
    if(gameData->aiData.blockFunctionRoots[0].blockFunctions[i].primary != NULL){
      j = 0;
      while(array[j] != gameData->aiData.blockFunctionRoots[0].blockFunctions[i].primary){
        j++;
      }
      assert(array[j] != NULL);
      element = array2[i];
      element->actions[5].external = array2[j];
      element->actions[5].status = 2;
    }
    if(gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary != NULL){
      j = 0;
      while(array[j] != gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary){
        j++;
      }
      assert(array[j] != NULL);
      element = array2[i];
      element->actions[6].external = array2[j];
      element->actions[5].status = 2;
    }
    i++;
  }
}

int gameLoop(GameData *gameData){
  /* GameData *gameData = the pointer to the gameData struct that we're using
     to save on the arguments passed to functions.
     This function should be called every frame */

  /* delta_t is the time in milliseconds elapsed since the last time this
     function ran */
  int delta_t;
  SDL_Event event;


  /* Storing the number of milliseconds since the program was run helps keep it
     moving smoothly by calculating delta_t */
  delta_t = calculateDt(gameData->gameRunTime);
  gameData->gameRunTime = SDL_GetTicks();


  /*clear helps get rid of things on the screen that shouldn't be there anymore
  and is also essential due to the way that render buffers can change unpredictably
  after the function SDL_RenderPresent*/


  UIRoot_Execute(&gameData->uiData,UPDATE,delta_t);

  panScreen(&gameData->graphicsData, &gameData->controlsData, delta_t);
  if(SDL_RenderClear(gameData->graphicsData.renderer) == -1){
	   printf("Error clearing renderer: %s\n", SDL_GetError());
  }
  paintBackground(&gameData->graphicsData,0,200,100);
  updateGameObjects(&gameData->gameObjectData, &gameData->graphicsData, delta_t);
  UIRoot_Execute(&gameData->uiData,RENDER,&gameData->graphicsData);
  runAI(&gameData->aiData,&gameData->gameObjectData);


  /*This function is like the blit function, putting pixels to the screen.
  but it needs to be called after all of the graphicall changes have been made,
  including those in the renderUI() function. After the call to render present
  the pixel buffer becomes unpredictable and should be followed by SDL_RenderClear
  (as above in this loop)*/
  SDL_RenderPresent(gameData->graphicsData.renderer);
  UIRoot_Execute(&gameData->uiData,GAME_OBJECT_UPDATE,&gameData->gameObjectData);
  UIRoot_Execute(&gameData->uiData,EXTERNAL);

  /* At the end of the loop we need to update the main application window to
     reflect the changes we've made to the graphics */
  /*SDL_UpdateWindowSurface(gameData->graphicsData.window);*/

  /* This bit makes sure our application keeps responding and doesn't crash */
  /* Don't worry too much about this for now */
	while (SDL_PollEvent(&event))
	{
    handleEvent(&event,&gameData->gameObjectData,&gameData->uiData,&gameData->controlsData);
	}
  delta_t = calculateDt(gameData->gameRunTime);
  gameData->gameRunTime = SDL_GetTicks();
  if (Mix_Playing(1) == 0) {
	 playMusic(&gameData->audioData,1);
  }
  if(delta_t<FRAME_TIME){
	SDL_Delay(FRAME_TIME-delta_t);
  }
  return(1);
}
