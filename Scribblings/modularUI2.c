#include "modularUI2.h"

#define COLOUR 0x000000
#define NAME "bareMinimum"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define IMAGE_PATH "images/blueFlower.bmp"
#define IMAGE_X 100
#define IMAGE_Y 100
#define IMAGE_HEIGHT 10
#define IMAGE_WIDTH 10

void goSDL(void);

UI_Element *UICreate_Button(void){
  printf("creating button\n");
  UI_Element *element = malloc(sizeof(UI_Element));
  UI_Trigger trigger;

  element->rect.x = 0;
  element->rect.y = 0;
  element->rect.w = 100;
  element->rect.h = 100;
  element->actions = malloc(sizeof(UI_Action)*4);
  element->num_of_actions = 4;

  trigger = UIConfigure_Trigger(&element->actions[2],2,0,1,3,2);

  UIConfigure_FillRect(element,&element->actions[0],0,100,100);
  UIConfigure_TwoRectOverride(element,&element->actions[2],0,0,100,100,100,100,200,200,0,0);
  UIConfigure_ClickRect(element,&element->actions[1],1,trigger);
  return element;
}

UI_Element *UICreate_Button2(void){
  UI_Element *element = malloc(sizeof(UI_Element));
  UI_Trigger trigger;

  element->rect.x = 300;
  element->rect.y = 300;
  element->rect.w = 50;
  element->rect.h = 50;
  element->actions = malloc(sizeof(UI_Action)*4);
  element->num_of_actions = 4;

  trigger.action = &element->actions[3];
  trigger.states_from = calloc(2, sizeof(int));
  trigger.states_to = calloc(2, sizeof(int));
  trigger.states = 2;
  trigger.states_from[0] = 1;
  trigger.states_to[0] = 0;
  trigger.states_from[1] = 0;
  trigger.states_to[1] = 1;

  UIConfigure_FillRect(element,&element->actions[0],300,300,50);
  UIConfigure_RenderLine(element,&element->actions[2]);
  UIConfigure_ClickRect(element,&element->actions[1],1,trigger);
  UIConfigure_CalculateLine(element,&element->actions[3],&element->actions[2]);
  return element;
}

UI_Element *UICreate_Button3(void){
  UI_Element *element = malloc(sizeof(UI_Element));
  element->rect.x = 0;
  element->rect.y = 100;
  element->rect.w = 50;
  element->rect.h = 50;
  return element;
}

int main(int argc, char *argv[]){
  goSDL();
  return 0;
}

void goSDL(void){
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Event event;
  UI_Element *element = UICreate_Button();
  UI_Element *element2 = UICreate_Button2();

   if(SDL_Init(SDL_INIT_VIDEO) != 0){
     return(1);
   }

  window = SDL_CreateWindow(
    "An SDL2 window",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    640,
    480,
    SDL_WINDOW_OPENGL);

  renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

  while(1){

    /* UPDATE actions */
    element->actions[2].function(&element->actions[2],15);

     SDL_SetRenderDrawColor(renderer,0,255,0,255);
     SDL_RenderFillRect(renderer,NULL);

     /* RENDER actions */
     element->actions[0].function(&element->actions[0],renderer);
     element2->actions[0].function(&element2->actions[0],renderer);
     element2->actions[2].function(&element2->actions[2],renderer);


     while(SDL_PollEvent(&event)){
          switch(event.type){
            case SDL_MOUSEBUTTONDOWN:
              /* CLICK actions */
              element->actions[1].function(&element->actions[1],&event);
              element2->actions[1].function(&element2->actions[1],&event);
              break;
            case SDL_MOUSEMOTION:
              element2->actions[3].function(&element2->actions[3],&event,renderer);
              break;
            case SDL_QUIT:
            case SDL_KEYDOWN:
              SDL_Quit();
              freeUIElement(element);
              freeUIElement(element2);
              exit(0);
              break;
          }
     }
     SDL_Delay(1);


     SDL_RenderPresent(renderer);

  }

  return(0);

}
