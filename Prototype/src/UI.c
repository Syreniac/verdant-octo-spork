#include "UI.h"



UI_Element createUI_Panel(SDL_Rect rect, int r, int g, int b){
  UI_Element element;

  element.panel.type = PANEL;
  element.panel.rect = rect;
  /*element.panel.colour = colour*/
  element.panel.r = r;
  element.panel.g = g;
  element.panel.b = b;

  return element;
}
UI_Element createUI_Clickable(SDL_Rect rect, char *message,int r, int g, int b){
  UI_Element element;
  element.clickable.type = CLICKABLE;
  element.clickable.rect = rect;
  /*element.clickable.colour = colour;*/
  element.clickable.message = message;
  element.clickable.parent = NULL;

  element.clickable.r = r;
  element.clickable.g = g;
  element.clickable.b = b;
  return element;
}
UI_Element createUI_Expandable(SDL_Rect s_rect, SDL_Rect b_rect,
                                     int msToBig, int msToSmall, int r, int g, int b){
  UI_Element element;
  element.expandable.type = EXPANDABLE;
  element.expandable.small_rect = s_rect;
  element.expandable.big_rect = b_rect;
  element.expandable.rect = s_rect;
  element.expandable.msToBig = msToBig;
  element.expandable.msToSmall = msToSmall;
  element.expandable.msTimer = 0;
  /*element.expandable.colour = colour;*/

  element.expandable.r = r;
  element.expandable.g = g;
  element.expandable.b = b;
  return element;
}

UI_Element createUI_Draggable(SDL_Rect rect, UI_Element *parent, int r, int g, int b){
  UI_Element element;
  element.draggable.type = DRAGGABLE;
  element.draggable.rect = rect;
  element.draggable.status = ROOTED;
  element.draggable.parent = parent;
  /*element.draggable.colour = colour;*/

  element.draggable.r = r;
  element.draggable.g = g;
  element.draggable.b = b;
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

      SDL_SetRenderDrawColor(graphicsData->renderer,
                             element->panel.r,
                             element->panel.g,
                             element->panel.b,
                             SDL_ALPHA_OPAQUE);

      SDL_RenderFillRect(graphicsData->renderer,
                   getUIElementRect(element));
/*
      SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                   getUIElementRect(element),
                   element->panel.colour);*/
      break;
    case GENERIC:
      break;
  }
}
int clickOnUIElement(UI_Element *element, SDL_Event *event){
  switch(element->type){
    case CLICKABLE:
    {
      if(isPointInRect(event->button.x, event->button.y,*getUIElementRect(element))){
        printf("clicked on clickable\n");
        return 1;
      }
	  break;
    }
    case DRAGGABLE:
    {
      if(isPointInRect(event->button.x, event->button.y,*getUIElementRect(element))){
        element->draggable.status = GRABBED;
        printf("clicked on draggable\n");
        return 1;
      }
	  break;
    }
	default:
	  return 0;
  }
  return 0;
}
void clickupOnUIElement(UI_Element *element, SDL_Event *event){
  switch(element->type){
    case DRAGGABLE:
      if(element->draggable.status == GRABBED){
        element->draggable.status = ROOTED;
      }
	default:
	  return;
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
	default:
	  return;
  }
}

int clickOnUI(UIData *uiData, SDL_Event *event){
  int i = 0;
  int j = 0;
  while(i < uiData->numberOfUIElements){
    j+=clickOnUIElement(&uiData->UIElements[i],event);
    i++;
  }
  return j;
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
