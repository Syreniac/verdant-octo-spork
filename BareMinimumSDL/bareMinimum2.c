#include <SDL2/SDL.h>
#include <stdio.h>

#define COLOUR 0x000000
#define NAME "bareMinimum"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[]){
  SDL_Window *window;
  SDL_Event event;

   if(SDL_Init(SDL_INIT_VIDEO) != 0){
     return(1);
   }
   
  window = SDL_CreateWindow(NAME,
							SDL_WINDOWPOS_UNDEFINED,
							SDL_WINDOWPOS_UNDEFINED,
							WINDOW_WIDTH,
							WINDOW_HEIGHT,
							SDL_WINDOW_OPENGL);

   SDL_FillRect(SDL_GetWindowSurface(window),
                NULL,
                COLOUR);

  while(1){
     SDL_FillRect(SDL_GetWindowSurface(window),
                  NULL,
                  COLOUR);
				  
     SDL_UpdateWindowSurface(window);
	 
     while(SDL_PollEvent(&event)){
          switch(event.type){
            case SDL_QUIT:
            case SDL_KEYDOWN:
              SDL_Quit();
              exit(0);
              break;
          }
     }
  }
  return(0);
}
