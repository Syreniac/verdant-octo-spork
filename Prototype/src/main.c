#include "main.h"

int main(int argc, char *argv[]){
  /* int argc = the number of arguments passed to the exe through the command line
     char *argv[] = the strings passed to the .exe as arguments.

     This is a fairly generic main function, but the SDL has to be hidden
     from Windows (or something?), so the functionality is split between other
     functions from other modules */

  SDL_Window *window;

  window  = initialise();
  printf("\ninitialised\n");
  gameStart(window);
  uninitialise();
  gameEnd();
  exit(0);
}


