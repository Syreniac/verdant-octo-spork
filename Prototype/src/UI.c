#include "UI.h"



UI_Element createUI_Panel(SDL_Rect rect, Uint32 colour){
  UI_Element element;

  element.panel.type = PANEL;
  element.panel.rect = rect;
  element.panel.colour = colour;

  return element;
}
UI_Element createUI_Clickable(SDL_Rect rect, char *message, Uint32 colour){
  UI_Element element;
  element.clickable.type = CLICKABLE;
  element.clickable.rect = rect;
  element.clickable.colour = colour;
  element.clickable.message = message;
  element.clickable.parent = NULL;
  return element;
}
UI_Element createUI_Expandable(SDL_Rect s_rect, SDL_Rect b_rect,
                                     int msToBig, int msToSmall, Uint32 colour){
  UI_Element element;
  element.expandable.type = EXPANDABLE;
  element.expandable.small_rect = s_rect;
  element.expandable.big_rect = b_rect;
  element.expandable.rect = s_rect;
  element.expandable.msToBig = msToBig;
  element.expandable.msToSmall = msToSmall;
  element.expandable.msTimer = 0;
  element.expandable.colour = colour;
  return element;
}

UI_Element createUI_Draggable(SDL_Rect rect, UI_Element *parent, Uint32 colour){
  UI_Element element;
  element.draggable.type = DRAGGABLE;
  element.draggable.rect = rect;
  element.draggable.status = ROOTED;
  element.draggable.parent = parent;
  element.draggable.colour = colour;
  return element;
}

SDL_Rect *getUIElementRect(UI_Element *element){
  switch(element->type){
    case CLICKABLE:
      return &element->clickable.rect;
    case EXPANDABLE:
      return &element->expandable.rect;
    case DRAGGABLE:
      return &element->draggable.rect;
    case PANEL:
      return &element->panel.rect;
    case GENERIC:
      return NULL;
  }
  return NULL;
}

void renderUIElement(GraphicsData *graphicsData, UI_Element *element){
  switch(element->type){
    case CLICKABLE:
    case DRAGGABLE:
    case PANEL:
    case EXPANDABLE:
      SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                   getUIElementRect(element),
                   element->panel.colour);
      break;
    case GENERIC:
      break;
  }
}
void clickOnUIElement(UI_Element *element, SDL_Event *event){
  switch(element->type){
    case CLICKABLE:
    {
      if(isPointInRect(event->button.x, event->button.y,*getUIElementRect(element))){
        printf("clicked on clickable\n");
      }
    }
    case DRAGGABLE:
    {
      if(isPointInRect(event->button.x, event->button.y,*getUIElementRect(element))){
        element->draggable.status = GRABBED;
        printf("clicked on draggable\n");
      }
    }
  }
}
void clickupOnUIElement(UI_Element *element, SDL_Event *event){
  switch(element->type){
    case DRAGGABLE:
      if(element->draggable.status == GRABBED){
        element->draggable.status = ROOTED;
      }
  }
}
void mousemoveOnUIElement(UI_Element *element, SDL_Event *event){
  SDL_Rect *rect;
  switch(element->type){
    case DRAGGABLE:
      if(element->draggable.status == GRABBED){
        rect = getUIElementRect(element);
        rect->x = event->motion.x;
        rect->y = event->motion.y;
        ensureRectEnclosed(rect,getUIElementRect(element->draggable.parent));
      }
  }
}

void clickOnUI(UIData *uiData, SDL_Event *event){
  int i = 0;
  while(i < uiData->numberOfUIElements){
    clickOnUIElement(&uiData->UIElements[i],event);
    i++;
  }
}
void clickupOnUI(UIData *uiData, SDL_Event *event){
  int i = 0;
  while(i < uiData->numberOfUIElements){
    clickupOnUIElement(&uiData->UIElements[i],event);
    i++;
  }
}

void moveMouseOnUi(UIData *uiData, SDL_Event *event){
  int i = 0;
  while(i < uiData->numberOfUIElements){
    mousemoveOnUIElement(&uiData->UIElements[i],event);
    i++;
  }
}

void renderUI(UIData *uiData, GraphicsData *graphicsData){
  int i = 0;
  while(i < uiData->numberOfUIElements){
    renderUIElement(graphicsData,&uiData->UIElements[i]);
    i++;
  }
}

void keydown(GraphicsData *graphicsData, GameObjectData *gameObjectData, SDL_Event *event){
    switch (event->key.keysym.scancode){
        case (SDL_SCANCODE_DOWN):
            printf("you pressed down key\n");
            /*if not at edge of world, keep going*/

            if(!(graphicsData->navigationOffset->y <= -Y_SIZE_OF_WORLD + Y_SIZE_OF_SCREEN)){
                graphicsData->navigationOffset->y -= 20;
            }
            break;
        case (SDL_SCANCODE_UP):
            printf("you pressed up key\n");
            /*if not at edge of world, keep going*/
            if(!(graphicsData->navigationOffset->y >= Y_SIZE_OF_WORLD - Y_SIZE_OF_SCREEN)){
                graphicsData->navigationOffset->y += 20;
            }
            break;
        case (SDL_SCANCODE_RIGHT):
            printf("you pressed right key\n");
            /*if not at edge of world, keep going*/
            if(!(graphicsData->navigationOffset->x <= -X_SIZE_OF_WORLD + X_SIZE_OF_SCREEN)){
                graphicsData->navigationOffset->x -= 20;
            }
            break;
        case (SDL_SCANCODE_LEFT):
            printf("you pressed left key\n");
            /*if not at edge of world, keep going*/
            if(!(graphicsData->navigationOffset->x >= X_SIZE_OF_WORLD - X_SIZE_OF_SCREEN)){
                graphicsData->navigationOffset->x += 20;
            }
            break;
        case (SDL_SCANCODE_P):
            printf("you pressed P (pause) key\n");
            gameObjectData->pause_status = 1 - gameObjectData->pause_status; /* 1 for pause, 0 for go on */

            if (gameObjectData->pause_status){
               /*put a big recentage in the middle of the screen, with a big PAUSE*/
               printf("Test1: %d\n", gameObjectData->pause_status);
               /*output pause_status to other part of program*/

            }
            else{
               /*remove the big recentage in the middle of the screen, and continue the clock*/
               printf("Test2: %d\n", gameObjectData->pause_status);
               /*output pause_status to other part of program*/
            }
            break;
        default:
            printf("redundant key pressed\n");
    }
}
