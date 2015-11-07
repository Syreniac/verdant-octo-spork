#include <sys/errno.h>
#include "game.h"
#include "SDL_image.h"

char *workingDirectory;

float calculateDt(float previousRunTime){
  /* float previousRunTime = the milliseconds you want to calculate from
   * This returns the difference in milliseconds between now and another time
   * in the past. It's used here to help keep the main loop running
   * consistently. */
  return (float) SDL_GetTicks() - previousRunTime;
}

int gameStart(SDL_Window *window){
  /* SDL_Window *window = the pointer to the program's window
   * This function will start the game looping.
   * The return should be 0 unless there's an error we need to catch.*/

  /* By creating a new struct to hold our game data, we can more easily
   * pass data between functions */
  workingDirectory = SDL_GetBasePath();
  printf("\nworking directory:%s\n", workingDirectory);
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

  /* Load in the BMPs for our ResourceNodes and ProgrammableWorkers.
     Now processes only absolute paths for cross-system compatibility. */
  IMG_LoadAndVerify(&gameData.graphicsData.nodeGraphic, "images/blueFlower.png");
  IMG_LoadAndVerify(&gameData.graphicsData.workerGraphic, "images/bee.png");
  IMG_LoadAndVerify(&gameData.graphicsData.hiveGraphic, "images/beehive.png");

  gameData.aiData.blockFunctionRoots = calloc(1, sizeof(BlockFunctionRoot));
  fopenAndVerify(&file,"ai/GenericWorkerAI.txt");
  makeBlockFunctionRootFromFile(&(gameData.aiData.blockFunctionRoots[0]), file);
  fclose(file);

  /* Then run the gameLoop until it returns 0 or exits */
  printf("gameStarted\n");
  while(gameLoopReturn){
    gameLoopReturn = gameLoop(&gameData);
  }
  return(0);
}

/* A wrapper for IMG_Load that also verifies loading success. */
void IMG_LoadAndVerify(SDL_Surface **surface, char *filepath) {
  *surface = IMG_LoadFromAbsolutePath(filepath);
  if(*surface == NULL ) {
    printf( "Unable to load %s! Consider rebuilding. SDL_image Error: %s\n", filepath, IMG_GetError() );
    exit(1);
  }
}

/* A wrapper for fopen that also verifies loading success. */
void fopenAndVerify(FILE **file, char *filepath) {
  *file = fopenFromAbsolutePath(filepath);
  if(*file == NULL ) {
    printf( "Unable to load %s! Consider rebuilding. errno reports: %d.\n"\
                "See errno.h to look up key of error values.", filepath, errno);
    exit(1);
  }
}

/* Generates an absolute path, given a relative one.
 * Necessary for cross-platform compatibility. */
char* absolutePathGenerator(char* filepath){
  char *absolutePath = malloc( sizeof('\0') + sizeof(char) * (strlen(workingDirectory) + strlen(filepath)));
  asprintf(&absolutePath, "%s%s", workingDirectory, filepath);
  printf("Absolute path to asset used:%s\n", absolutePath);
  return absolutePath;
}

/* A wrapper for loadBMP which accepts absolute paths. Only needed available
 * whilst coders are having problems installing SDL2_image.*/
SDL_Surface* loadBMPFromAbsolutePath(char* filepath){
  char* absolutePath = absolutePathGenerator(filepath);
  SDL_Surface* surface = SDL_LoadBMP(absolutePath);
  free(absolutePath);
  return surface;
}

/* A wrapper for IMG_Load which accepts absolute paths. */
SDL_Surface* IMG_LoadFromAbsolutePath(char* filepath){
  char* absolutePath = absolutePathGenerator(filepath);
  SDL_Surface* surface = IMG_Load(absolutePath);
  free(absolutePath);
  return surface;
}

/* A wrapper for fopen which accepts absolute paths. Currently read-only. */
FILE* fopenFromAbsolutePath(char* filepath){
  char* absolutePath = absolutePathGenerator(filepath);
  FILE* file = fopen(absolutePath, "r");
  free(absolutePath);
  return file;
}


int gameLoop(GameData *gameData){
  /* GameData *gameData = the pointer to the gameData struct that we're using
   * to save on the arguments passed to functions.
   * This function should be called every frame */

  /* delta_t is the time in milliseconds elapsed since the last time this
   * function ran */
  float delta_t = calculateDt(gameData->gameRunTime);
  SDL_Event event;
  int i = 0, j = 0;

  /* Storing the number of milliseconds since the program was run helps keep it
   * moving smoothly by calculating delta_t */
  gameData->gameRunTime = (float) SDL_GetTicks();

  /* Filling the background black helps get rid of things drawn onto the screen
   * that shouldn't be there anymore */
  SDL_FillRect(SDL_GetWindowSurface(gameData->graphicsData.window),NULL,0x1B8D2E);

  updateGameObjects(&gameData->gameObjectData, &gameData->graphicsData, delta_t);

  runAI(&gameData->aiData,&gameData->gameObjectData);

  updateUI(&gameData->uiData, &gameData->graphicsData, delta_t);

  /* At the end of the loop we need to update the main application window to
   * reflect the changes we've made to the graphics */
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
		}
	}
  SDL_Delay(16);
  return(1);
}

/* Free up any memory allocated during gameStart. */
void gameEnd(){
  SDL_free(workingDirectory);
  /*SDL_FreeSurface(gameData.graphicsData.nodeGraphic);
  SDL_FreeSurface(gameData.graphicsData.workerGraphic);
  SDL_FreeSurface(gameData.graphicsData.hiveGraphic); */
}