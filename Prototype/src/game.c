#include "game.h"

static void createGameUI(GameData *gameData);
static void cleanUpGameData(GameData *gameData);

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
  /*hive must be generated before trees, as this allows the last tree to be placed near the hive*/
  generateTrees(&gameData.gameObjectData);
  generateWeatherLayer(&gameData.gameObjectData);

  generateIceCreamPerson(&gameData.gameObjectData);
  generateDroppedIceCream(&gameData.gameObjectData);

  generateRoamingSpider(&gameData.gameObjectData);

  memset(&gameData.gameObjectData.droppedIceCream->rect.x,256,sizeof(int));
  memset(&gameData.gameObjectData.droppedIceCream->rect.y,256,sizeof(int));

  gameData.graphicsData.grassTexture = loadTextureFromFile("images/grass/grass4.bmp",&gameData.graphicsData, 0);
  gameData.graphicsData.treeStumpTexture = loadTextureFromFile("images/stump.bmp",&gameData.graphicsData, 1);
  gameData.graphicsData.nodeTexture[0] = loadTextureFromFile("images/blueFlower.bmp",
														                                 &gameData.graphicsData, 1);
  gameData.graphicsData.nodeTexture[1] = loadTextureFromFile("images/redFlower.bmp",&gameData.graphicsData, 1);
  gameData.graphicsData.nodeTexture[2] = loadTextureFromFile("images/yellowFlower.bmp",&gameData.graphicsData, 1);

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

  gameData.graphicsData.roamingArachnid = (RoamingArachnid*) malloc(sizeof(RoamingArachnid));

  gameData.graphicsData.roamingArachnid->graphic[SPIDER] =
  loadTextureFromFile("images/spider.bmp", &gameData.graphicsData, 1);

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

  gameData.graphicsData.uiEle = (UIEle*) malloc(sizeof(UIEle));
//  gameData.graphicsData.ui->graphic[SCROLLHANDLE_GRAPHIC] = loadTextureFromFile("images/UI/scrollhandle.bmp",
//														  &gameData.graphicsData, 1);

  gameData.graphicsData.uiEle->graphic[SCROLLHANDLE_GRAPHIC] = loadTextureFromFile("images/UI/scrollhandle.bmp",
                                                                                  &gameData.graphicsData, 1);

  gameData.graphicsData.uiEle->graphic[CROSSBOX_GRAPHIC] = loadTextureFromFile("images/UI/crossbox.bmp",
                                                                                  &gameData.graphicsData, 1);
  gameData.graphicsData.uiEle->graphic[COMPILEBOX_GRAPHIC] = loadTextureFromFile("images/UI/compilebox.bmp",
                                                                                  &gameData.graphicsData, 1);
  gameData.graphicsData.uiEle->graphic[STOP_GRAPHIC] = loadTextureFromFile("images/UI/stop.bmp",
                                                                                  &gameData.graphicsData, 1);
  gameData.graphicsData.uiEle->graphic[BLOCK_GRAPHIC] = loadTextureFromFile("images/UI/block.bmp",
                                                                                  &gameData.graphicsData, 1);
  gameData.graphicsData.uiEle->graphic[HIVECELL_GRAPHIC] = loadTextureFromFile("images/UI/hivecell.bmp",
                                                                                  &gameData.graphicsData, 1);
  gameData.graphicsData.uiEle->graphic[HIVECELLMASK_GRAPHIC] = loadTextureFromFile("images/UI/hivecellmask.bmp",
                                                                                  &gameData.graphicsData, 1);

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

  gameLoopReturn = 1;
  while(gameLoopReturn == 1){
    gameLoopReturn = gameLoop(&gameData);
  }

  cleanUpGameData(&gameData);
  return gameLoopReturn;
}

static void cleanUpGameData(GameData *gameData){
  UIRoot_Destroy(&gameData->uiData);
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
  element = UIElement_Create(-1,0,dm.w+1,TOP_BAR_HEIGHT,4);
  UIConfigure_FillAndBorderRect(element,&element->actions[0],248,221,35,0,0,0,FILLRECT);
  UIConfigure_DisplayString(element, &element->actions[1], " ",0,UISTRING_ALIGN_LEFT);
  UIConfigure_GetAnnouncement(element, &element->actions[2], &element->actions[1]);
  UIConfigure_PercOffsetRect(element, &element->actions[3], 0.0, 0.0, 1.0, 0.0, -1, 0, 100, 31,0);
  UIElement_Reparent(element,gameData->uiData.root);


    /*game Over box*/
  /*element = UIElement_Create(0,0,win_x/2,win_y/2,6);
  printf("game over box is %p\n",element);
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
  	quickSetStatus(&element->actions[1],1);
  UIConfigure_FillAndBorderRect(element, &element->actions[2],255,255,255,0,0,0,FILLRECT);
	  quickSetStatus(&element->actions[2],0);
  UIConfigure_DisplayString(element, &element->actions[3],"GAME OVER",0, UISTRING_ALIGN_CENTER);
	  quickSetStatus(&element->actions[3],0);
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
  	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
  	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIConfigure_PercOffsetRect(element, &element->actions[5], 0.25, 0.25, 0.75,0.75, 0,0,0,0,0);
  UIElement_Reparent(element,gameData->uiData.root);*/


  /* Mute button */
      element = UIElement_Create((win_x - 30), win_y - 30, 30, 30, 6);
    UIConfigure_FillRect(element, &element->actions[0],228,240,3);
	UIConfigure_PercPosition(element, &element->actions[1],1.0,1.0,-30,-30,0);
	UIConfigure_LeftClickRect(element, &element->actions[2]);
		UITrigger_Bind(&element->actions[2],&element->actions[3],0,1);
	UIConfigure_MuteSound(element,&element->actions[3]);
	UIConfigure_RightClickRect(element, &element->actions[4]);
		UITrigger_Bind(&element->actions[4],&element->actions[5],0,1);
	UIConfigure_MuteSoundFX(element,&element->actions[5]);
    UIElement_Reparent(element,gameData->uiData.root);
	UIRoot_Pack(&gameData->uiData,&gameData->graphicsData);




      /*objectInfoDisplay box*/
  element = UIElement_Create(49,30,250,52,8);
  UIConfigure_Auto(element, &element->actions[0], OBJECT_DISPLAY);
    UITrigger_Bind(&element->actions[0],&element->actions[0],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[1],0,2);
    UITrigger_Bind(&element->actions[0],&element->actions[2],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[3],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[6],-1,0);
	  quickSetStatus(&element->actions[0],0);
  UIConfigure_Auto(element, &element->actions[1], OBJECT_DISPLAY);
    UITrigger_Bind(&element->actions[1],&element->actions[0],0,2);
    UITrigger_Bind(&element->actions[1],&element->actions[1],1,0);
    UITrigger_Bind(&element->actions[1],&element->actions[2],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[3],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[6],0,1);
  	quickSetStatus(&element->actions[1],1);
  UIConfigure_FillAndBorderRect(element,&element->actions[2],248,221,35,0,0,0,FILLRECT);
	  quickSetStatus(&element->actions[2],0);
  UIConfigure_DisplayString(element, &element->actions[3]," ",0, UISTRING_ALIGN_CENTER);
	  quickSetStatus(&element->actions[3],0);
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
  	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
  	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIConfigure_GetInfoDisplayString(element, &element->actions[5], &element->actions[3]);
  UIConfigure_DisplayStringSubrect(element, &element->actions[6]," ",0,0,0,250,52);
    quickSetStatus(&element->actions[6],0);
  UIConfigure_GetObjectStatusString(element, &element->actions[7], &element->actions[6]);
  UIElement_Reparent(element,gameData->uiData.root);

    /*gameOverInformation*/
  element = UIElement_Create(0,0,400,100,8);
  UIConfigure_Auto(element, &element->actions[0], GAME_OVER);
  UIConfigure_Auto(element, &element->actions[1], GAME_OVER);
    UITrigger_Bind(&element->actions[1],&element->actions[0],0,2);
    UITrigger_Bind(&element->actions[1],&element->actions[1],1,0);
    UITrigger_Bind(&element->actions[1],&element->actions[2],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[3],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[7],0,1);
  	quickSetStatus(&element->actions[1],1);
  UIConfigure_FillAndBorderRect(element,&element->actions[2],255,255,255,0,0,0,FILLRECT);
	  quickSetStatus(&element->actions[2],0);
  UIConfigure_DisplayString(element, &element->actions[3]," ",0, UISTRING_ALIGN_CENTER);
	  quickSetStatus(&element->actions[3],0);
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
  	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
  	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIConfigure_PercPosition(element, &element->actions[5],0.5,0.5,-200,-50,0);
  UIConfigure_GetGameOverString(element, &element->actions[6], &element->actions[3]);
  UIConfigure_DisplayStringSubrect(element, &element->actions[7],"PRESS < ENTER > TO PLAY AGAIN",0,0,50,400,50);
    quickSetStatus(&element->actions[7],0);
  UIElement_Reparent(element,gameData->uiData.root);

  // 249,252,220

      /*YEARS label*/
  element2 = UIElement_Create(0,0,YEARS_LABEL_WIDTH+YEARS_COUNTER_WIDTH,TOP_BAR_HEIGHT+1,6);
  UIConfigure_FillAndBorderRect(element2,&element2->actions[0],147,147,170,0,0,0,FILLRECT);
  UIConfigure_DisplayStringSubrect(element2, &element2->actions[1],"YEARS SURVIVED:",0,0,0,YEARS_LABEL_WIDTH,TOP_BAR_HEIGHT-1);
  UIConfigure_PercPosition(element2, &element2->actions[2],1.0,0.0,-(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIConfigure_FillSubrect(element2,&element2->actions[3],YEARS_LABEL_WIDTH,1,YEARS_COUNTER_WIDTH-1,TOP_BAR_HEIGHT-1,249,252,220);
  UIConfigure_DisplayNumberSubrect(element2, &element2->actions[4], 0,0,YEARS_LABEL_WIDTH,1,YEARS_COUNTER_WIDTH-1,TOP_BAR_HEIGHT-1);
  UIConfigure_YearsCounter(element2, &element2->actions[5],1,&element2->actions[4]);
  UIElement_Reparent(element2,element);

  element2 = UIElement_Create(0,0,SCORE_LABEL_WIDTH+SCORE_COUNTER_WIDTH-1,TOP_BAR_HEIGHT,6);
  UIConfigure_FillRect(element2,&element2->actions[0],147,147, 170);
  UIConfigure_DisplayStringSubrect(element2, &element2->actions[1],"SUGAR:",0,0,0,SCORE_LABEL_WIDTH,TOP_BAR_HEIGHT-1);
  UIConfigure_PercPosition(element2, &element2->actions[2],1.0,0.0,-(SCORE_COUNTER_WIDTH + SCORE_LABEL_WIDTH)-(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIConfigure_FillSubrect(element2,&element2->actions[3],SCORE_LABEL_WIDTH,1,SCORE_COUNTER_WIDTH-1,TOP_BAR_HEIGHT-1,249,252,220);
  UIConfigure_DisplayNumberSubrect(element2, &element2->actions[4], 0,0,SCORE_LABEL_WIDTH,0,SCORE_COUNTER_WIDTH-1,TOP_BAR_HEIGHT-1);
  UIConfigure_ResourceCounter(element2, &element2->actions[5],1,&element2->actions[4]);
  UIElement_Reparent(element2,element);

  element2 = UIElement_Create(0,0,DAYS_LABEL_WIDTH+DAYS_COUNTER_WIDTH,TOP_BAR_HEIGHT+1,6);
  UIConfigure_FillAndBorderRect(element2,&element2->actions[0],147,147,170,0,0,0,FILLRECT);
  UIConfigure_DisplayStringSubrect(element2, &element2->actions[1],"DAYS TO WINTER:",0,0,0,DAYS_LABEL_WIDTH,TOP_BAR_HEIGHT-1);
  UIConfigure_PercPosition(element2, &element2->actions[2],1.0,0.0,-(SCORE_COUNTER_WIDTH + SCORE_LABEL_WIDTH)-(DAYS_LABEL_WIDTH + DAYS_COUNTER_WIDTH) -(YEARS_LABEL_WIDTH + YEARS_COUNTER_WIDTH),0,0);
  UIConfigure_FillSubrect(element2,&element2->actions[3],DAYS_LABEL_WIDTH,1,DAYS_COUNTER_WIDTH-1,TOP_BAR_HEIGHT-1,249,252,220);
  UIConfigure_DisplayNumberSubrect(element2, &element2->actions[4], 0,0,DAYS_LABEL_WIDTH,0,DAYS_COUNTER_WIDTH-1,TOP_BAR_HEIGHT-1);
  UIConfigure_DaysCounter(element2, &element2->actions[5],1,&element2->actions[4]);
  UIElement_Reparent(element2,element);

  /* Minimap */
  element = UIElement_Create(0,0,100,100,10);
  printf("minimap is %p\n",element);
  UIConfigure_FillAndBorderRect(element,&element->actions[0],0x5F,0xB4,0x04,0,0,0,FILLRECT);
  UIConfigure_PercPosition(element,&element->actions[1],1.0,1.0,-99,-99,0);
  UIConfigure_Minimap(element,&element->actions[2]);
	UIConfigure_LeftClickRect(element,&element->actions[3]);
		UITrigger_Bind(&element->actions[3],&element->actions[4],0,1);
		UITrigger_Bind(&element->actions[3],&element->actions[4],3,2);
    UITrigger_Bind(&element->actions[3],&element->actions[6],0,1);
	UIConfigure_TwoRectOverride(element,&element->actions[4],0,  win_y - 100, 100,         100,
                                                           50, 50,          win_x - 100, win_y - 200,200);
  UIConfigure_UpdateTwoRectOverrideOnWindowResize(element, &element->actions[5],&element->actions[4],
                                                                                  55,55,0.0,0.0,
                                                                                  -105,-155,1.0,1.0,
                                                                                  -1,-199,0.0,1.0,
                                                                                  200,200,0.0,0.0);
  UIConfigure_ToggleInteger(element,&element->actions[6],&gameData->controlsData.objectSelection);
  UIConfigure_MinimapMouseMove(element,&element->actions[7]);
	  quickSetStatus(&element->actions[7],0);
  UIConfigure_RightClickRect(element,&element->actions[8]);
    UITrigger_Bind(&element->actions[8],&element->actions[7],0,1);
  UIConfigure_RightReleaseAnywhere(element,&element->actions[9]);
    UITrigger_Bind(&element->actions[9],&element->actions[7],1,0);
  UIElement_Reparent(element,gameData->uiData.root);


  element = UIElement_Create(win_x/2,win_y - 50,200,25,6);
  printf("pause box is %p\n",element);
  UIConfigure_Auto(element, &element->actions[0], RESPONSE_PAUSE);
    UITrigger_Bind(&element->actions[0],&element->actions[0],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[1],0,1);
    UITrigger_Bind(&element->actions[0],&element->actions[2],-1,0);
    UITrigger_Bind(&element->actions[0],&element->actions[3],-1,0);
  	  quickSetStatus(&element->actions[0],0);
  UIConfigure_Auto(element, &element->actions[1], RESPONSE_PAUSE);
    UITrigger_Bind(&element->actions[1],&element->actions[0],-1,1);
    UITrigger_Bind(&element->actions[1],&element->actions[1],1,0);
    UITrigger_Bind(&element->actions[1],&element->actions[2],0,1);
    UITrigger_Bind(&element->actions[1],&element->actions[3],0,1);
  UIConfigure_FillAndBorderRect(element, &element->actions[2],255,0,0,0,0,0,FILLRECT);
	  quickSetStatus(&element->actions[2],0);
  UIConfigure_DisplayString(element, &element->actions[3],"PAUSED",0, UISTRING_ALIGN_CENTER);
	  quickSetStatus(&element->actions[3],0);
  UIConfigure_Auto(element,&element->actions[4],UPDATE);
	UITrigger_Bind(&element->actions[4],&element->actions[0],2,1);
	UITrigger_Bind(&element->actions[4],&element->actions[1],2,1);
  UIConfigure_PercPosition(element, &element->actions[5], 0.5, 1.0, -100, -25,0);
  UIElement_Reparent(element,gameData->uiData.root);


  /*b++ editor button*/
  element2 = UIElement_Create(0, win_y - 100, 100,100,6);
  printf("BPP editor panel is %p\n",element2);
	UIConfigure_FillAndBorderRect(element2,&element2->actions[0],248,221,35,0,0,0,FILLRECT);
	UIConfigure_LeftClickRect(element2,&element2->actions[1]);
		UITrigger_Bind(&element2->actions[1],&element2->actions[2],0,1);
    UITrigger_Bind(&element2->actions[1],&element2->actions[1],1,0);
    UITrigger_Bind(&element2->actions[1],&element2->actions[4],0,1);
    UIConfigure_DisplayString(element2, &element2->actions[5],"B + +",0, UISTRING_ALIGN_CENTER);
	UIConfigure_TwoRectOverride(element2,&element2->actions[2],0,win_y - 100, 100, 100,
                                                               50, 50, win_x - 100, win_y - 200,
                                                               200);
  UIConfigure_UpdateTwoRectOverrideOnWindowResize(element2, &element2->actions[3],&element2->actions[2],
                                                                                  55,55,0.0,0.0,
                                                                                  -105,-155,1.0,1.0,
                                                                                  -1,-248,0.0,1.0,
                                                                                  51,50,0.0,0.0);
  UIConfigure_ToggleInteger(element2,&element2->actions[4],&gameData->controlsData.objectSelection);
  UIElement_Reparent(element2,gameData->uiData.root);

  /* Minimize button */
  element = UIElement_Create(50 + win_x - 150, 50, 50, 50,6);
  printf("minimise button is %p\n",element);
  UIConfigure_FillAndBorderRect(element, &element->actions[0],185,122,87,0,0,0,CROSSBOX);
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
  printf("compiler output bar is %p\n",element4);
  UIConfigure_FillAndBorderRect(element4,&element4->actions[0],255,255,255,0,0,0,FILLRECT);
  UIConfigure_ShrinkFitToParent(element4,&element4->actions[1]);
  UIConfigure_DisplayString(element4,&element4->actions[2],"                                                                                                                          ",0,UISTRING_ALIGN_LEFT);
	UIConfigure_PercOffsetRect(element4,&element4->actions[3],0.0,1.0,1.0,1.0,50,-125,-269,-100,1,&element4->actions[1]);
  UIElement_Reparent(element4,element2);

  /* The big panel holding all the AI blocks */
  /* I'm making it a little bigger along the bottom and right hand side because otherwise there's some double lapping of pixels
     when drawing the border */
  element3 = UIElement_Create(50,50,win_x - 270,win_y - 200,4);
  printf("block panel is %p\n",element3);
  UIConfigure_FillAndBorderRect(element3, &element3->actions[0],249,252,124,0,0,0,FILLRECT);
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
  printf("compile button is %p\n",element);
  UIConfigure_FillAndBorderRect(element, &element->actions[0],249,252,124,0,0,0,COMPILEBOX);
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

  makeAITemplateScrollList(265,225,&gameData->aiData,element2,element3);

  element4 = makeStartBlock(60,60,element3);

	/*hive button*/
  element2 = UIElement_Create(0, win_y - 100, 100,100,3);
  printf("Hive internals panel is %p\n",element2);
	UIConfigure_FillAndBorderRect(element2,&element2->actions[0],249,252,124,0,0,0,FILLRECT);
  UIConfigure_DisplayString(element2, &element2->actions[1],"Hive",0, UISTRING_ALIGN_CENTER);
  UIConfigure_PercOffsetRect(element2, &element2->actions[2],0.0,0.0,0.0,1.0,-1,TOP_BAR_HEIGHT,50,-247,0);
  UIElement_Reparent(element2,gameData->uiData.root);

  i = 0;
  topY = TOP_BAR_HEIGHT + 25;
  topX = -1;
  while(i < NUMBER_OF_CELLS_IN_HIVE){
    makeHiveCellBlock(topX,topY,element2,&gameData->gameObjectData.hive.hiveCells[i]);
      topY += 60;
    i++;
  }
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
  if(gameData->gameRunTime % 50 == 0){
    printf("t @ benchmark %s: %d\n", "START",SDL_GetTicks() - gameData->gameRunTime);
  }
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
  announce_update(&gameData->announcementsData, gameData->delta);

  #if BENCHMARK_TEST==1
  if(gameData->gameRunTime % 50 == 0){
    printf("t @ benchmark %s: %d\n", "GAME",SDL_GetTicks() - gameData->gameRunTime);
  }
  #endif

  SDL_RenderPresent(gameData->graphicsData.renderer);

  #if BENCHMARK_TEST==1
  if(gameData->gameRunTime % 50 == 0){
    printf("t @ benchmark %s: %d\n", "RENDER",SDL_GetTicks() - gameData->gameRunTime);
  }
  #endif
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
  }
  if (Mix_Playing(1) == 0) {
 	  playMusic(&gameData->audioData,1);
  }

  UIRoot_ExecuteUpwards(&gameData->uiData,DISPOSAL,0);

  #if BENCHMARK_TEST==1
  if(gameData->gameRunTime % 50 == 0){
    printf("t @ benchmark %s: %d\n", "FINAL",SDL_GetTicks() - gameData->gameRunTime);
    printf("--------------------------\n");
  }
  #endif
  return 1;
}
