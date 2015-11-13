#include "init.h"

GraphicsData initialise(void){
  /* This function will initialise the SDL library and create a blank window.
     I'll line by line comment what I'm doing here. */
  GraphicsData graphicsData;


  /* Initialise SDL library (must be called before any other SDL_function),
  SDL_INIT_VIDEO flag initialise only the video SDL subsystem*/
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    /* If the SDL_Init function doesn't return -1, then something has gone wrong
    and we should probably close (after giving a helpful error message!) */
  	printf("Could not initialize SDL: %s\n", SDL_GetError());
  	exit(1);
  }

  /* Now that SDL has been initialised by call SDL_Init, we can make the window
     The arguments are:
        - The window's name
        - X coords to open the window at (we don't need to specify something)
        - Y coords like above
        - The X size of the window in pixels
        - The Y size like above
        - SDL configuration options which can be found online in the API documentation*/

	graphicsData.window = SDL_CreateWindow(PROGRAM_NAME,
										   SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           X_SIZE_OF_SCREEN, Y_SIZE_OF_SCREEN,
                                           SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
										   
    graphicsData.renderer = SDL_CreateRenderer(graphicsData.window,
                                               -1,
                                               SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_SOFTWARE);
	assert(graphicsData.renderer != NULL);
	
  return graphicsData;
}

void uninitialise(void){
  /* This function should cleanup the SDL library for us */
  SDL_Quit();
}
