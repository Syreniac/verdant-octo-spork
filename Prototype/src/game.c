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
  gameData.gameObjectData.gameOver = 0;
  gameData.gameObjectData.gameOverBoxVisible = 0;
  gameData.gameObjectData.gameOverEventNum = SDL_RegisterEvents(1);
  gameData.gameObjectData.gameRestart = 0;

  initControlData(&gameData.controlsData);

  /* initialise navigationOffset values */
  gameData.graphicsData.navigationOffset.x = X_INITIAL_SCREEN_OFFSET; /*setting initial x offset to center of world*/
  gameData.graphicsData.navigationOffset.y = Y_INITIAL_SCREEN_OFFSET; /*setting initial y offset ot center of world*/
  gameData.graphicsData.navigationOffset.w = 0;
  gameData.graphicsData.navigationOffset.h = 0;

  gameData.graphicsData.trackingMode = 0;



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
  generateDroppedIceCream(&gameData.gameObjectData);


  gameData.graphicsData.grassTexture = loadTextureFromFile("images/grass/grass4.bmp",&gameData.graphicsData, 0);
  gameData.graphicsData.treeStumpTexture = loadTextureFromFile("images/stump.bmp",&gameData.graphicsData, 1);
  gameData.graphicsData.nodeTexture = loadTextureFromFile("images/blueFlower.bmp",
														  &gameData.graphicsData, 1);

  gameData.graphicsData.shelter = malloc(sizeof(struct Shelter));

  gameData.graphicsData.shelter->graphic[SUMMER_INDEX] =
  loadTextureFromFile("images/tree1.bmp",
					  &gameData.graphicsData, 1);

  gameData.graphicsData.shelter->graphic[AUTUMN_INDEX] =
  loadTextureFromFile("images/treeAutumn1.bmp",
					  &gameData.graphicsData, 1);

  gameData.graphicsData.shelter->graphic[WINTER_INDEX] =
  loadTextureFromFile("images/treeWinter.bmp",
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

  gameData.graphicsData.droppedIceCreamTexture = loadTextureFromFile("images/person/droppedIceCream.bmp", &gameData.graphicsData, 1);
  gameData.graphicsData.meltedIceCreamTexture = loadTextureFromFile("images/person/meltedIceCream.bmp", &gameData.graphicsData, 1);

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

  gameData.graphicsData.bee->graphic[BEE_FLAP_GRAPHIC_1 + CARRYING_FLOWER_INDEX_OFFSET] = loadTextureFromFile("images/beeWithFlower.bmp",
														  &gameData.graphicsData, 1);
  gameData.graphicsData.bee->graphic[BEE_FLAP_GRAPHIC_2 + CARRYING_FLOWER_INDEX_OFFSET] = loadTextureFromFile("images/beeWithFlower2.bmp",
														  &gameData.graphicsData, 1);

  gameData.graphicsData.bee->graphic[BEE_FLAP_GRAPHIC_1 + CARRYING_ICECREAM_INDEX_OFFSET] =
  loadTextureFromFile("images/beeWithIcecream.bmp", &gameData.graphicsData, 1);
  gameData.graphicsData.bee->graphic[BEE_FLAP_GRAPHIC_2 + CARRYING_ICECREAM_INDEX_OFFSET] =
  loadTextureFromFile("images/beeWithIcecream2.bmp",&gameData.graphicsData, 1);

  gameData.graphicsData.hiveTexture = loadTextureFromFile("images/beehive.bmp",
														  &gameData.graphicsData, 1);

  gameData.aiData = initAIData();

  initUIData(&gameData.uiData);
  gameData.uiData.graphicsData = &gameData.graphicsData;
  gameData.uiData.gameObjectData = &gameData.gameObjectData;
  gameData.uiData.aiData = &gameData.aiData;
  gameData.uiData.announcementsData = &gameData.announcementsData;
  gameData.uiData.ticks = &gameData.delta;
  createGameUI(&gameData);

  /* Then run the gameLoop until it returns 0 or exits */
  announce_init(&gameData.announcementsData);
  playMusic(&gameData.audioData,1);

  while(gameLoopReturn){
    gameLoopReturn = gameLoop(&gameData);
  }

  if(gameData.gameObjectData.gameRestart){
  	return(1);
  }else{
  	return(0);
  }
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

  /*get dimensions of the display monitor*/
  SDL_GetCurrentDisplayMode(0, &dm);

  /* top information bar */
  element = UIElement_Create(0,0,dm.w,31,4);
  UIConfigure_FillAndBorderRect(element,&element->actions[0],248,221,35,0,0,0);
  UIConfigure_PercPosition(element, &element->actions[1],1.0,0.0,-dm.w,0,0);
  UIConfigure_DisplayString(element, &element->actions[2], "Hello",0,UISTRING_ALIGN_LEFT);
  UIConfigure_GetAnnouncement(element, &element->actions[3], &element->actions[2]);
  UIElement_Reparent(element,gameData->uiData.root);
  
  /*game Over box*/
  element = UIElement_Create(0,0,win_x/2,win_y/2,6);
  UIConfigure_Auto(element, &element->actions[0], GAME_OVER);
    UITrigger_Bind(&element->actions[0],&element->actions[0],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[1],0,2);
    UITrigger_Bind(&element->actions[0],&element->actions[2],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[3],-1,0);
	  quickSetStatus(&element->actions[0],0);
  UIConfigure_Auto(element, &element->actions[1], GAME_OVER);
    UITrigger_Bind(&element->actions[1],&element->actions[0],0,2);
    UITrigger_Bind(&element->actions[1],&element->actions[1],1,0);
    UITrigger_Bind(&element->actions[1],&element->actions[2],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[3],0,1);
  	quickSetStatus(&element->actions[1],1)
  UIConfigure_FillAndBorderRect(element, &element->actions[2],255,255,255,0,0,0);
	  quickSetStatus(&element->actions[2],0)
  UIConfigure_DisplayString(element, &element->actions[3],"GAME OVER",0, UISTRING_ALIGN_CENTER);
	  quickSetStatus(&element->actions[3],0)
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
  	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
  	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIConfigure_PercOffsetRect(element, &element->actions[5], 0.25, 0.25, 0.75,0.75, 0,0,0,0,0);
  UIElement_Reparent(element,gameData->uiData.root);

 /*press enter to restart (part of game over box)*/
   element = UIElement_Create(0,0,200,25,6);
  UIConfigure_Auto(element, &element->actions[0], GAME_OVER);
    UITrigger_Bind(&element->actions[0],&element->actions[0],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[1],0,2);
    UITrigger_Bind(&element->actions[0],&element->actions[2],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[3],-1,0);
	  element->actions[0].status = 0;
	  element->actions[0].new_status = 0;
  UIConfigure_Auto(element, &element->actions[1], GAME_OVER);
    UITrigger_Bind(&element->actions[1],&element->actions[0],0,2);
    UITrigger_Bind(&element->actions[1],&element->actions[1],1,0);
    UITrigger_Bind(&element->actions[1],&element->actions[2],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[3],0,1);
	  element->actions[1].status = 1;
	  element->actions[1].new_status = 1;
  UIConfigure_FillAndBorderRect(element, &element->actions[2],255,255,255,255,255,255);
	  element->actions[2].status = 0;
	  element->actions[2].new_status = 0;
  UIConfigure_DisplayString(element, &element->actions[3],"PRESS < ENTER > TO PLAY AGAIN",0, UISTRING_ALIGN_CENTER);
	  element->actions[3].status = 0;
	  element->actions[3].new_status = 0;
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIConfigure_PercPosition(element, &element->actions[5], 0.5, 1.0, -100, -215,0);
  UIElement_Reparent(element,gameData->uiData.root);



      /*YEARS label*/
  element2 = UIElement_Create(0,0,YEARS_LABEL_WIDTH,TOP_BAR_HEIGHT,3);
  UIConfigure_FillRect(element2,&element2->actions[0],147,147, 170);
  UIConfigure_DisplayString(element2, &element2->actions[1],"  YEARS SURVIVED: ",0,UISTRING_ALIGN_LEFT);
  UIConfigure_PercPosition(element2, &element2->actions[2],1.0,0.0,
  -(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIElement_Reparent(element2,element);


  /* YEARS counter*/
  element2 = UIElement_Create(0,0,YEARS_COUNTER_WIDTH,TOP_BAR_HEIGHT,4);
  UIConfigure_FillRect(element2,&element2->actions[0],249,252,220);
  UIConfigure_DisplayNumber(element2, &element2->actions[1], 0,0,UISTRING_ALIGN_CENTER);
  UIConfigure_YearsCounter(element2, &element2->actions[2],1,&element2->actions[1]);
  UIConfigure_PercPosition(element2, &element2->actions[3],1.0,0.0,-YEARS_COUNTER_WIDTH,0,0);
  UIElement_Reparent(element2,element);

  /*score counter label*/
  element2 = UIElement_Create(0,0,SCORE_LABEL_WIDTH,TOP_BAR_HEIGHT,3);
  UIConfigure_FillRect(element2,&element2->actions[0],127,127, 150);
  UIConfigure_DisplayString(element2, &element2->actions[1],"  SUGAR: ",0,UISTRING_ALIGN_LEFT);
  UIConfigure_PercPosition(element2, &element2->actions[2],1.0,0.0,
  -(SCORE_COUNTER_WIDTH + SCORE_LABEL_WIDTH)-(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIElement_Reparent(element2,element);


  /* Score counter */
  element2 = UIElement_Create(0,0,SCORE_COUNTER_WIDTH,TOP_BAR_HEIGHT,4);
  UIConfigure_FillRect(element2,&element2->actions[0],249,252,124);
  UIConfigure_DisplayNumber(element2, &element2->actions[1], 0,0,UISTRING_ALIGN_CENTER);
  UIConfigure_ResourceCounter(element2, &element2->actions[2],1,&element2->actions[1]);
  UIConfigure_PercPosition(element2, &element2->actions[3],1.0,0.0,-SCORE_COUNTER_WIDTH -(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIElement_Reparent(element2,element);

    /*day counter label*/
  element2 = UIElement_Create(0,0,DAYS_LABEL_WIDTH,TOP_BAR_HEIGHT,3);
  UIConfigure_FillRect(element2,&element2->actions[0],147,147, 170);
  UIConfigure_DisplayString(element2, &element2->actions[1],"  DAYS TO WINTER: ",0,UISTRING_ALIGN_LEFT);
  UIConfigure_PercPosition(element2, &element2->actions[2],1.0,0.0,
  -(SCORE_COUNTER_WIDTH + SCORE_LABEL_WIDTH)-(DAYS_LABEL_WIDTH + DAYS_COUNTER_WIDTH) -(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIElement_Reparent(element2,element);


  /* day counter */
  element2 = UIElement_Create(0,0,DAYS_COUNTER_WIDTH,TOP_BAR_HEIGHT,4);
  UIConfigure_FillRect(element2,&element2->actions[0],249,252,124);
  UIConfigure_DisplayNumber(element2, &element2->actions[1], 0,0,UISTRING_ALIGN_CENTER);
  UIConfigure_DaysCounter(element2, &element2->actions[2],1,&element2->actions[1]);
  UIConfigure_PercPosition(element2, &element2->actions[3],1.0,0.0,
  -(SCORE_COUNTER_WIDTH + SCORE_LABEL_WIDTH)-DAYS_COUNTER_WIDTH -(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIElement_Reparent(element2,element);

  /* Minimap */
  element = UIElement_Create(0,0,100,100,10);
  UIConfigure_FillAndBorderRect(element,&element->actions[0],0x5F,0xB4,0x04,0,0,0);
  UIConfigure_PercPosition(element,&element->actions[1],1.0,1.0,-99,-99,0);
  UIConfigure_Minimap(element,&element->actions[2]);
	UIConfigure_LeftClickRect(element,&element->actions[3]);
		UITrigger_Bind(&element->actions[3],&element->actions[4],0,1);
		UITrigger_Bind(&element->actions[3],&element->actions[4],3,2);
    UITrigger_Bind(&element->actions[3],&element->actions[6],0,1);
	UIConfigure_TwoRectOverride(element,&element->actions[4],0,  win_y - 100, 100,         100,
                                                           50, 50,          win_x - 100, win_y - 200,200);
  UIConfigure_UpdateTwoRectOverrideOnWindowResize(element, &element->actions[5],&element->actions[4],
                                                                                  50,50,0.0,0.0,
                                                                                  -100,-150,1.0,1.0,
                                                                                  -99,-99,1.0,1.0,
                                                                                  100,100,0.0,0.0);
  UIConfigure_ToggleInteger(element,&element->actions[6],&gameData->controlsData.objectSelection);
  UIConfigure_MinimapMouseMove(element,&element->actions[7]);
	  quickSetStatus(&element->actions[7],0);
  UIConfigure_RightClickRect(element,&element->actions[8]);
    UITrigger_Bind(&element->actions[8],&element->actions[7],0,1);
  UIConfigure_RightReleaseAnywhere(element,&element->actions[9]);
    UITrigger_Bind(&element->actions[9],&element->actions[7],1,0);
  UIElement_Reparent(element,gameData->uiData.root);


  element = UIElement_Create(win_x/2,win_y - 50,200,25,6);
  UIConfigure_Auto(element, &element->actions[0], RESPONSE_PAUSE);
    UITrigger_Bind(&element->actions[0],&element->actions[0],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[1],0,1);
    UITrigger_Bind(&element->actions[0],&element->actions[2],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[3],-1,0);
  UIConfigure_Auto(element, &element->actions[1], RESPONSE_PAUSE);
    UITrigger_Bind(&element->actions[1],&element->actions[0],0,2);
    UITrigger_Bind(&element->actions[1],&element->actions[1],1,0);
    UITrigger_Bind(&element->actions[1],&element->actions[2],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[3],0,1);
  	  quickSetStatus(&element->actions[1],0);
  UIConfigure_FillAndBorderRect(element, &element->actions[2],255,0,0,0,0,0);
	  quickSetStatus(&element->actions[2],0);
  UIConfigure_DisplayString(element, &element->actions[3],"PAUSED",0, UISTRING_ALIGN_CENTER);
	  quickSetStatus(&element->actions[3],0);
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIConfigure_PercPosition(element, &element->actions[5], 0.5, 1.0, -100, -25,0);
  UIElement_Reparent(element,gameData->uiData.root);


  element2 = UIElement_Create(0, win_y - 100, 100,100,6);
	UIConfigure_FillAndBorderRect(element2,&element2->actions[0],248,221,35,0,0,0);
	UIConfigure_LeftClickRect(element2,&element2->actions[1]);
		UITrigger_Bind(&element2->actions[1],&element2->actions[2],0,1);
    UITrigger_Bind(&element2->actions[1],&element2->actions[1],1,0);
    UITrigger_Bind(&element2->actions[1],&element2->actions[4],0,1);
    UIConfigure_DisplayString(element2, &element2->actions[5],"     B + +     ^",0, UISTRING_ALIGN_CENTER);
	UIConfigure_TwoRectOverride(element2,&element2->actions[2],0,win_y - 100, 100, 100,
                                                               50, 50, win_x - 100, win_y - 200,
                                                               200);
  UIConfigure_UpdateTwoRectOverrideOnWindowResize(element2, &element2->actions[3],&element2->actions[2],
                                                                                  50,50,0.0,0.0,
                                                                                  -100,-150,1.0,1.0,
                                                                                  -1,-99,0.0,1.0,
                                                                                  100,100,0.0,0.0);
  UIConfigure_ToggleInteger(element2,&element2->actions[4],&gameData->controlsData.objectSelection);
  UIElement_Reparent(element2,gameData->uiData.root);

  /* Minimize button */
  element = UIElement_Create(50 + win_x - 150, 50, 50, 50,6);
  UIConfigure_FillAndBorderRect(element, &element->actions[0],185,122,87,0,0,0);
  UIConfigure_ShrinkFitToParent(element, &element->actions[1]);
  UIConfigure_LeftClickRect(element, &element->actions[2]);
    UITrigger_Bind(&element->actions[2],&element->actions[3],0,1);
    UITrigger_Bind(&element->actions[2],&element->actions[5],0,1);
  UIConfigure_Auto(element, &element->actions[3],UPDATE);
    UITrigger_Bind(&element->actions[3], &element2->actions[2], 3,2);
    UITrigger_Bind(&element->actions[3], &element2->actions[1], 0,1);
    UITrigger_Bind(&element->actions[3], &element->actions[3], 1,0);
  UIConfigure_PercPosition(element, &element->actions[4],1.0,0.0,-100,50,1,&element->actions[1]);
  UIConfigure_ToggleInteger(element,&element->actions[5],&gameData->controlsData.objectSelection);
  UIElement_Reparent(element,element2);

  /* This is the output from the B++ compiler */
  /* I'm making it a little bigger so the border doesn't doublepixel */
  element4 = UIElement_Create(0,0,0,0,4);
  UIConfigure_FillAndBorderRect(element4,&element4->actions[0],222,0,0,0,0,0);
  UIConfigure_ShrinkFitToParent(element4,&element4->actions[1]);
  UIConfigure_DisplayString(element4,&element4->actions[2],"                                                                                                                          ",0,UISTRING_ALIGN_LEFT);
	UIConfigure_PercOffsetRect(element4,&element4->actions[3],0.0,1.0,1.0,1.0,50,-125,-269,-100,1,&element4->actions[1]);
  UIElement_Reparent(element4,element2);

  /* The big panel holding all the AI blocks */
  /* I'm making it a little bigger along the bottom and right hand side because otherwise there's some double lapping of pixels
     when drawing the border */
  element3 = UIElement_Create(50,50,win_x - 270,win_y - 200,4);
  UIConfigure_FillAndBorderRect(element3, &element3->actions[0],249,252,124,0,0,0);
  UIConfigure_ShrinkFitToParent(element3, &element3->actions[1]);
  UIConfigure_ReadAiBlocks(element3,&element3->actions[2],&element4->actions[2]);
  UIConfigure_PercOffsetRect(element3,&element3->actions[3],0.0,0.0,1.0,1.0,50,50,-269,-124,1,&element3->actions[1]);
  UIElement_Reparent(element3,element2);

  /* add block button - DEPRECATED
  element = UIElement_Create(50 + win_x - 150, 150,50,50,4);
  UIConfigure_FillRect(element, &element->actions[0],255,255,0);
  UIConfigure_ShrinkFitToParent(element, &element->actions[1]);
  UIConfigure_LeftClickRect(element, &element->actions[2]);
	UITrigger_Bind(&element->actions[2],&element->actions[3],0,1);
  UIConfigure_AddAiBlock(element,&element->actions[3],element3);
  UIElement_Reparent(element,element2);*/

  /* Calculate AI button */
  element = UIElement_Create(0, 0, 50, 50, 5);
  UIConfigure_FillAndBorderRect(element, &element->actions[0],249,252,124,0,0,0);
  UIConfigure_ShrinkFitToParent(element, &element->actions[1]);
  UIConfigure_LeftClickRect(element, &element->actions[2]);
	UITrigger_Bind(&element->actions[2],&element->actions[3],0,1);
  UIConfigure_Auto(element, &element->actions[3],UPDATE);
    quickSetStatus(&element->actions[3],0);
    UITrigger_Bind(&element->actions[3], &element3->actions[2], 0,1);
    UITrigger_Bind(&element->actions[3], &element->actions[3],1,0);
  UIConfigure_PercPosition(element,&element->actions[4],1.0,0.0,-100,110,1,&element->actions[1]);
  UIElement_Reparent(element,element2);

  /* Nullify AI button */

  makeAIResetButton(-100,170,element2);

  makeAITemplateScrollList(270,230,&gameData->aiData,element2,element3);

  element4 = makeStartBlock(topX,topY,element3);
  topX+=210;

  if(gameData->aiData.blockFunctionRoots[0].numOfBlockFunctions != 0){
	  array[i] = &gameData->aiData.blockFunctionRoots[0].blockFunctions[i];
	  element2 = makeAIBlock(topX,topY,&array[i]->name[0],element3);
	  array2[i] = element2;
	  topX+=210;
  }
  else{
	  return;
  }

  i = 1;
  while(i < gameData->aiData.blockFunctionRoots[0].numOfBlockFunctions){
    array[i] = &gameData->aiData.blockFunctionRoots[0].blockFunctions[i];
    element2 = makeAIBlock(topX,topY,&array[i]->name[0],element3);
    array2[i] = element2;
    topX+=210;
    if(topX > win_y - 350){
      topX -= win_y -350;
      topY+= 60;
    }
    i++;
  }
  if(gameData->aiData.blockFunctionRoots[0].numOfBlockFunctions > 0){
	element4->actions[3].external = array2[0];
	element4->actions[3].status = 2;
	element4->actions[3].new_status = 2;
  }
  i = 0;

    if(gameData->aiData.blockFunctionRoots[0].blockFunctions[i].primary != NULL){
      j = 0;
      while(array[j] != gameData->aiData.blockFunctionRoots[0].blockFunctions[i].primary){
        j++;
      }
      assert(array[j] != NULL);
      element = array2[i];
      element->actions[6].external = array2[j];
      element->actions[6].status = 2;
	  element->actions[6].new_status = 2;
    }
    if(gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary != NULL){
      j = 0;
      while(array[j] != gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary){
        j++;
      }
      assert(array[j] != NULL);
      element = array2[i];
      element->actions[11].external = array2[j];
      element->actions[11].status = 2;
	  element->actions[11].new_status = 2;
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
      element->actions[6].external = array2[j];
      element->actions[6].status = 2;
	  element->actions[6].new_status = 2;
    }
    if(gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary != NULL){
      j = 0;
      while(array[j] != gameData->aiData.blockFunctionRoots[0].blockFunctions[i].secondary){
        j++;
      }
      assert(array[j] != NULL);
      element = array2[i];
      element->actions[11].external = array2[j];
      element->actions[11].status = 2;
	  element->actions[11].new_status = 2;
    }
    i++;
  }
  gameData->element = array2[0];
  UIRoot_Pack(&gameData->uiData,&gameData->graphicsData);
}



int gameLoop(GameData *gameData){
  /* GameData *gameData = the pointer to the gameData struct that we're using
     to save on the arguments passed to functions.
     This function should be called every frame */

  /* delta_t is the time in milliseconds elapsed since the last time this
     function ran */
  SDL_Event event;

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

  if(!gameData->graphicsData.trackingMode){
  	panScreen(&gameData->graphicsData, &gameData->controlsData, gameData->delta);
  }
  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  #endif

  if(SDL_RenderClear(gameData->graphicsData.renderer) == -1){
	   printf("Error clearing renderer: %s\n", SDL_GetError());
  }
  paintBackground(&gameData->graphicsData,0,200,100);
  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  #endif

  updateGameObjects(&gameData->gameObjectData, &gameData->graphicsData, &gameData->announcementsData, gameData->delta);
  UIRoot_Execute(&gameData->uiData,UPDATE,0);
  runAI(&gameData->aiData,&gameData->gameObjectData);

  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  #endif

  SDL_RenderPresent(gameData->graphicsData.renderer);
  announce_update(&gameData->announcementsData, gameData->delta);

  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  #endif

  while (SDL_PollEvent(&event)){
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
		      handleEvent(&event,&gameData->gameObjectData,&gameData->uiData,&gameData->controlsData, &gameData->graphicsData);
  	  }

  }
  while (SDL_PollEvent(&event)){
	   handleEvent(&event,&gameData->gameObjectData,&gameData->uiData,&gameData->controlsData, &gameData->graphicsData);
  }
  if (Mix_Playing(1) == 0) {
 	  playMusic(&gameData->audioData,1);
  }
  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  #endif

  UIRoot_ExecuteUpwards(&gameData->uiData,DISPOSAL,0);

  #if BENCHMARK_TEST==1
  printf("t @ benchmark %d: %d\n", testMarker++,SDL_GetTicks() - gameData->gameRunTime);
  printf("--------------------------\n");
  #endif
  if(gameData->delta<FRAME_TIME){
	   SDL_Delay(FRAME_TIME-gameData->delta);
  }
  if(gameData->delta<FRAME_TIME){
	   SDL_Delay(FRAME_TIME-gameData->delta);
  }

  if(gameData->gameObjectData.gameRestart){
     return(0);
  }
  else{
     return(1);
  }
}
