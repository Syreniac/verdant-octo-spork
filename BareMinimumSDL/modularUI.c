#include "ModularUI.h"

int isPointInRect(int point_x, int point_y, SDL_Rect rect){
  /* int point_x   = the x coordinate of the point we want to test
     int point_y   = the y coordinate fo the point we want to test
     SDL_Rect rect = the rectangle we want to test against. It holds x,y,w,h.

     This checks whether a point is inside a given rectangle.
     returns 1 if it is or 0 if it isn't.*/
  if(point_x < rect.x || point_x > rect.x + rect.w){
    return 0;
  }
  if(point_y < rect.y || point_y > rect.y + rect.h){
    return 0;
  }
  return 1;
}

int isRectEnclosedInRect(SDL_Rect rectA, SDL_Rect rectB){
  /* SDL_Rect rectA = the rectangle that we want to test is enclosed.
     SDL_Rect rectB = the rectangle that we want to see if it encloses rectA.

     This checks to see whether a given rectangle is entirely inside another. */
  if(rectA.x < rectB.x || rectA.x+rectA.w > rectB.x+rectB.w){
    return 0;
  }
  if(rectA.y < rectB.y || rectA.y+rectA.h > rectB.y+rectB.h){
    return 0;
  }
  return 1;
}

void ensureRectEnclosed(SDL_Rect *ensure, SDL_Rect *limit){
  /* SDL_Rect *ensure = the pointer to the rectangle that we want to ensure is
                        inside another
     SDL_Rect *limit  = the pointer to the rectangle that we want to use as the
                        limit for the other

     This function will ensure that the first rectangle is entirely within the
     other by moving it until there is no collision */
  int xOffset = 0, yOffset = 0;
  if(ensure->x < limit->x){
    xOffset = limit->x - ensure->x;
  }
  else if(ensure->x + ensure->w > limit->x + limit->w){
    xOffset = (limit->x+limit->w) - (ensure->x+ensure->w);
  }

  if(ensure->y < limit->y){
    yOffset = limit->y - ensure->y;
  }
  else if(ensure->y + ensure->h > limit->y + limit->h){
    yOffset = (limit->y+limit->h) - (ensure->y+ensure->h);
  }
  ensure->x += xOffset;
  ensure->y += yOffset;
}

void printUIElement(UI_Element *element){
  assert(element!=NULL);
  printf("UI_Element @ %p:",element);
  switch(element->type){
    case BUTTON:
      printf("%12s","Button:");
      printf("%4d,%-4d %4d,%-4d:",element->button.rect.x,
                            element->button.rect.y,
                            element->button.rect.w,
                            element->button.rect.h);
      printf("%d,%d\n",element->button.visibility,
                       element->button.tangibility);
      break;
    case PANEL:
      printf("%12s","Panel:");
      printf("%4d,%-4d %4d,%-4d:",element->panel.rect.x,
                            element->panel.rect.y,
                            element->panel.rect.w,
                            element->panel.rect.h);
      printf("%d\n",element->button.visibility);
      break;
    case DRAGGABLE:
      printf("%12s","Draggable:");
      printf("%4d,%-4d %4d,%-4d:",element->draggable.rect.x,
                            element->draggable.rect.y,
                            element->draggable.rect.w,
                            element->draggable.rect.h);
      printf("%d,%d:",element->draggable.visibility,
                      element->draggable.tangibility);
      printf("%d\n",element->draggable.status);
      break;
  }
}

UI_Element *createButton(int x, int y, int w, int h){
  UI_Element *element = malloc(sizeof(UI_Element));

  element->button.type = BUTTON;
  element->button.child = NULL;
  element->button.parent = NULL;
  element->button.sibling = NULL;
  element->button.rect.x = x;
  element->button.rect.y = y;
  element->button.rect.w = w;
  element->button.rect.h = h;
  element->button.visibility = SHOWN;
  element->button.tangibility = TANGIBLE;

  return(element);
}

UI_Element *createDraggable(int x, int y, int w, int h){
  UI_Element *element = malloc(sizeof(UI_Element));

  element->draggable.type = DRAGGABLE;
  element->draggable.child = NULL;
  element->draggable.parent = NULL;
  element->draggable.sibling = NULL;
  element->draggable.rect.x = x;
  element->draggable.rect.y = y;
  element->draggable.rect.w = w;
  element->draggable.rect.h = h;
  element->draggable.visibility = SHOWN;
  element->draggable.status = ROOTED;
  element->draggable.tangibility = TANGIBLE;

  return(element);
}

UI_Element *createPanel(int x, int y, int w, int h){
  UI_Element *element = malloc(sizeof(UI_Element));

  element->panel.type = PANEL;
  element->panel.child = NULL;
  element->panel.parent = NULL;
  element->panel.sibling = NULL;
  element->panel.rect.x = x;
  element->panel.rect.y = y;
  element->panel.rect.w = w;
  element->panel.rect.h = h;
  element->panel.visibility = SHOWN;

  return(element);
}

UI_Element *createRoot(void){
  UI_Element *element = malloc(sizeof(UI_Element));

  element->generic.type = GENERIC;
  element->generic.child = NULL;
  element->generic.parent = NULL;
  element->generic.sibling = NULL;

  return(element);
}

UI_Element *addChild(UI_Element *parent, UI_Element *new_child){
  UI_Element *child = parent->generic.child;
  printf("%p\n",child);
  if(child != NULL){
    printf("UIElement already has a child, checking for first unsibling'd child...\n");
    while(child->generic.sibling != NULL){
      child = child->generic.sibling;
    }
    printf("\t%p\n",child);
    child->generic.sibling = new_child;
  }
  else{
    parent->generic.child = new_child;
  }
  new_child->generic.parent = parent;
  return new_child;
}

int getNumOfChildren(UI_Element *toCount){
  int i = 0;
  UI_Element *child = toCount->generic.child;
  while(child!=NULL){
    i++;
    child = child->generic.sibling;
  }
  return i;
}

void renderUIElement(UI_Element *element, SDL_Window *window){
  switch(element->type){
    case BUTTON:
      if(element->button.visibility == SHOWN){
        SDL_FillRect(SDL_GetWindowSurface(window),
                     &element->button.rect,
                     0xb00000);
      }
      break;
    case PANEL:
      if(element->panel.visibility == SHOWN){
        SDL_FillRect(SDL_GetWindowSurface(window),
                     &element->panel.rect,
                     0x00b000);
      }
      break;
    case DRAGGABLE:
      if(element->draggable.visibility == SHOWN){
        SDL_FillRect(SDL_GetWindowSurface(window),
                     &element->draggable.rect,
                     0x0000b0);
      }
      break;
  }
}

void renderUI(UI_Element *root, SDL_Window *window){
  assert(root->type == GENERIC);
  UI_Element *child = root->generic.child;
  UI_Element *sibling = NULL;
  UI_Element *moving_pointer;
  int rendered_on_this_pass = 1;
  int depth = 0;
  int i = 0;

  while(rendered_on_this_pass != 0){
    i = 0;
    child = root->generic.child;
    sibling = NULL;
    while(i < depth && child->generic.child != NULL){

    }


  }


  while(child != NULL){

    renderUIElement(child,window);
    sibling = child->generic.sibling;

    while(sibling != NULL){
      renderUIElement(sibling,window);
      sibling = sibling->generic.sibling;
    }

    child = child->generic.child;

  }
}

int clickOnUIElement(UI_Element *element, int click_x, int click_y){
  switch(element->type){
    case BUTTON:
      if(element->button.tangibility == TANGIBLE && isPointInRect(click_x,click_y,element->button.rect)){
           if(getUIElementVisibility(element->generic.parent) == HIDDEN){
             setUIElementVisibility(element->generic.parent,SHOWN,0);
           }
           else{
             setUIElementVisibility(element->generic.parent,HIDDEN,0);
           }
           printf("button pressed!\n");
           return 1;
         }
      break;
    case DRAGGABLE:
      if(element->draggable.tangibility == TANGIBLE && isPointInRect(click_x,click_y,element->draggable.rect)){
           printf("draggable clicked!\n");
           element->draggable.status = GRABBED;
           return 1;
         }
      break;
  }
  return 0;
}

int clickupOnUIElement(UI_Element *element, int click_x, int click_y){
  switch(element->type){
    case DRAGGABLE:
      if(element->draggable.status == GRABBED){
           printf("draggable clicked down!\n");
           element->draggable.status = ROOTED;
           return 1;
         }
      break;
  }
  return 0;
}

SDL_Rect *getUIElementRect(UI_Element *element){
  assert(element->type != GENERIC);
  switch(element->type){
    case BUTTON:
      return &element->button.rect;
    case PANEL:
      return &element->panel.rect;
    case DRAGGABLE:
      return &element->draggable.rect;
  }
  return NULL;
}

int moveOnUIElement(UI_Element *element, int move_x, int move_y){
  switch(element->type){
    case DRAGGABLE:
      if(element->draggable.status == GRABBED){
        element->draggable.rect.x = move_x;
        element->draggable.rect.y = move_y;
        ensureRectEnclosed(&element->draggable.rect,getUIElementRect(element->generic.parent));
      }
  }
  return 0;
}

void setUIElementVisibility(UI_Element *element, enum Visibility visible, int propagate){
  UI_Element *child = element->generic.child;
  UI_Element *sibling;
  switch(element->type){
    case BUTTON:
      element->button.visibility = visible;
      break;
    case PANEL:
      element->panel.visibility = visible;
      break;
    case DRAGGABLE:
      element->draggable.visibility = visible;
      break;
  }
  /* A propagated change will be pushed to all the children of this UI_Element,
     and to their children, and so on. */
  if(propagate){
    while(child != NULL){
      setUIElementVisibility(child,visible,0);
      sibling = child->generic.sibling;
      while(sibling != NULL){
        setUIElementVisibility(sibling,visible,0);
        sibling = sibling->generic.sibling;
      }
      child = child->generic.child;
    }
  }
}

enum Visibility getUIElementVisibility(UI_Element *element){
  switch(element->type){
    case BUTTON:
      return element->button.visibility;
    case PANEL:
      return element->panel.visibility;
    case DRAGGABLE:
      return element->draggable.visibility;
    }
  return HIDDEN;
}

void clickOnUI(UI_Element *root, SDL_Event event){
  assert(root->type == GENERIC);
  UI_Element *child = root->generic.child;
  UI_Element *sibling = NULL;
  int clicked = 0;
  while(child != NULL && !clicked){
    clicked = clickOnUIElement(child,event.button.x,event.button.y);
    sibling = child->generic.sibling;
    while(sibling != NULL && !clicked){
      clicked = clickOnUIElement(sibling,event.button.x,event.button.y);
      sibling = sibling->generic.sibling;
    }
    child = child->generic.child;
  }
}

void destroyUIElement(UI_Element *element){
  UI_Element *child = element->generic.child;
  /* This holds an element so we can destroy one and still access the next one
     in the chain */
  UI_Element *next;
  while(child != NULL){
    next = child->generic.sibling;
    destroyUIElement(child);
    child = next;
  }
  if(element->generic.parent->generic.child == element){
    element->generic.parent->generic.child = NULL;
  }
  free(element);
}

void moveOnUI(UI_Element *root, SDL_Event event){
  assert(root->type == GENERIC);
  UI_Element *child = root->generic.child;
  UI_Element *sibling = NULL;
  int clicked = 0;
  while(child != NULL && !clicked){
    clicked = moveOnUIElement(child,event.motion.x,event.motion.y);
    sibling = child->generic.sibling;
    while(sibling != NULL && !clicked){
      clicked = moveOnUIElement(sibling,event.motion.x,event.motion.y);
      sibling = sibling->generic.sibling;
    }
    child = child->generic.child;
  }
}

void clickupOnUI(UI_Element *root, SDL_Event event){
  assert(root->type == GENERIC);
  UI_Element *child = root->generic.child;
  UI_Element *sibling = NULL;
  int clicked = 0;
  while(child != NULL && !clicked){
    clicked = clickupOnUIElement(child,event.button.x,event.button.y);
    sibling = child->generic.sibling;
    while(sibling != NULL && !clicked){
      clicked = clickupOnUIElement(sibling,event.button.x,event.button.y);
      sibling = sibling->generic.sibling;
    }
    child = child->generic.child;
  }
}

int goSDL(void){
  SDL_Window *window;
  SDL_Event event;
  UI_Element *root = createRoot();
  UI_Element *panel;
  UI_Element *panel2;
  UI_Element *button;
  UI_Element *button2;

  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("An SDL2 window",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640,
                            480,
                            SDL_WINDOW_OPENGL);
  printf("adding first panel\n");
  panel = addChild(root,createPanel(0,0,160,160));
  printf("adding second panel\n");
  panel2 = addChild(root,createPanel(160,160,160,160));
  button = addChild(panel2,createDraggable(160,160,10,10));
  button2 = addChild(panel2,createDraggable(60,60,10,10));

  printUIElement(panel);
  printUIElement(panel2);
  printUIElement(button);
  printUIElement(button2);

  while(1){

      SDL_FillRect(SDL_GetWindowSurface(window),
                   NULL,
                   0x000000);
     renderUI(root,window);

     while(SDL_PollEvent(&event)){
          switch(event.type){
            case SDL_MOUSEBUTTONDOWN:
              clickOnUI(root, event);
              break;
            case SDL_MOUSEBUTTONUP:
              clickupOnUI(root, event);
              break;
            case SDL_MOUSEMOTION:
              moveOnUI(root, event);
              break;
            case SDL_KEYDOWN:
            case SDL_QUIT:
              SDL_Quit();
              exit(0);
              break;
          }
     }

     SDL_UpdateWindowSurface(window);

     SDL_Delay(500);

  }
}

int main(int argc, char *argv[]){
  goSDL();

  return 0;
}
