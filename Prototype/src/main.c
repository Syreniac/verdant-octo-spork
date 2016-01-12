#include "main.h"

int main(int argc, char *argv[]){
  /* int argc = the number of arguments passed to the exe through the command line
     char *argv[] = the strings passed to the .exe as arguments.

     This is a fairly generic main function, but the SDL has to be hidden
     from Windows (or something?), so the functionality is split between other
     functions from other modules */

  InitData initData;
  int game_return = 2;

  #if DEBUGGING==1
  DEBUGGING_FILE_ALLOC= fopen("debug_output_alloc.txt","w");
  DEBUGGING_FILE_FREE= fopen("debug_output_free.txt","w");
  #endif
  initData  = initialise();
	game_welcome_page(initData.graphicsData,initData.audioData);
  while(game_return == 2){
    game_return = gameStart(initData.graphicsData,initData.audioData);
  }
  uninitialise();
  exit(0);
}
