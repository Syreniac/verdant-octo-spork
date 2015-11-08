#include "UI.h"

SimpleButton createUISimpleButton(int x, int y, int w, int h, char *message, Uint32 colour){
  /* int x = the x coordinates relative to the main window that we want to
             create the SimpleButton at.
     int y = the y coordinates relative to the main window that we want to
             create the SimpleButton at.
     int w = the width of the SimpleButton.
     int h = the height of the SimpleButton.
     char *message = the message the button will print to the terminal when clicked.
     Uint32 colour = the colour the button will be on the screen.

     This creates a SimpleButton using the arguments provided, and helps prevent
     the data being junk.
     It returns a SimpleButton.*/
  SimpleButton simpleButton;
  simpleButton.rect.x = x;
  simpleButton.rect.y = y;
  simpleButton.rect.w = w;
  simpleButton.rect.h = h;
  simpleButton.message = message;
  simpleButton.colour = colour;
  return simpleButton;
}

void updateSimpleButton(GraphicsData *graphicsData, SimpleButton *simpleButton){
  /* GraphicsData *graphicsData = a pointer to the GraphicsData struct that
                                  contains the information we need to draw
                                  things on the screen.
     SimpleButton *simpleButton = a pointer to the individual SimpleButton we
                                  are updating in this function

     This updates a SimpleButton and draws it onto the screen. At the moment
     this just means filling a rectangle with a given colour. */
  SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
               &simpleButton->rect,
               simpleButton->colour);
}

int checkClickSimpleButton(int click_x, int click_y, SimpleButton *simpleButton){
  if(isPointInRect(click_x, click_y, simpleButton->rect)){
    performClickSimpleButton(simpleButton);
    return(1);
  }
  return 0;
}

void performClickSimpleButton(SimpleButton *simpleButton){
  printf("%s\n",simpleButton->message);
}

ExpandablePanel createExpandingPanel(int s_x, int s_y, int s_h, int s_w,
                                     int b_x, int b_y, int b_h, int b_w,
                                     int msToBig, int msToSmall, Uint32 colour){
  ExpandablePanel expandablePanel;
  expandablePanel.small_rect.x = s_x;
  expandablePanel.small_rect.y = s_y;
  expandablePanel.small_rect.h = s_h;
  expandablePanel.small_rect.w = s_w;

  expandablePanel.big_rect.x = b_x;
  expandablePanel.big_rect.y = b_y;
  expandablePanel.big_rect.w = b_w;
  expandablePanel.big_rect.h = b_h;

  expandablePanel.msToBig = msToBig;
  expandablePanel.msToSmall = msToSmall;
  expandablePanel.msTimer = 0;
  expandablePanel.colour = colour;
  expandablePanel.status = SMALL;
  return expandablePanel;
}

void updateExpandablePanel(GraphicsData *graphicsData, ExpandablePanel *expandablePanel, float ticks){
  if(expandablePanel->status == SMALL){
    SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                 &expandablePanel->small_rect,
                 expandablePanel->colour);
  }
  else if(expandablePanel->status == BIG){
    SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                 &expandablePanel->big_rect,
                 expandablePanel->colour);
  }
  else if(expandablePanel->status == SMALL2BIG){
    expandablePanel->msTimer += ticks;
    if(expandablePanel->msTimer >= expandablePanel->msToBig){
      expandablePanel->status = BIG;
      expandablePanel->msTimer = 0;
      SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                   &expandablePanel->big_rect,
                   expandablePanel->colour);
    }
    else{
      expandablePanel->mobile_rect.x = expandablePanel->small_rect.x + expandablePanel->msTimer * (expandablePanel->big_rect.x - expandablePanel->small_rect.x)/expandablePanel->msToBig;
      expandablePanel->mobile_rect.y = expandablePanel->small_rect.y + expandablePanel->msTimer * (expandablePanel->big_rect.y - expandablePanel->small_rect.y)/expandablePanel->msToBig;
      expandablePanel->mobile_rect.w = expandablePanel->small_rect.w + expandablePanel->msTimer * (expandablePanel->big_rect.w - expandablePanel->small_rect.w)/expandablePanel->msToBig;
      expandablePanel->mobile_rect.h = expandablePanel->small_rect.h + expandablePanel->msTimer * (expandablePanel->big_rect.h - expandablePanel->small_rect.h)/expandablePanel->msToBig;
      SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                   &expandablePanel->mobile_rect,
                   expandablePanel->colour);
    }
  }
  else if(expandablePanel->status == BIG2SMALL){
    expandablePanel->msTimer += ticks;
    if(expandablePanel->msTimer >= expandablePanel->msToSmall){
      expandablePanel->status = SMALL;
      expandablePanel->msTimer = 0;
      SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                   &expandablePanel->small_rect,
                   expandablePanel->colour);
    }
    else{
      expandablePanel->mobile_rect.x = expandablePanel->big_rect.x - expandablePanel->msTimer * (expandablePanel->big_rect.x - expandablePanel->small_rect.x)/expandablePanel->msToSmall;
      expandablePanel->mobile_rect.y = expandablePanel->big_rect.y - expandablePanel->msTimer * (expandablePanel->big_rect.y - expandablePanel->small_rect.y)/expandablePanel->msToSmall;
      expandablePanel->mobile_rect.w = expandablePanel->big_rect.w - expandablePanel->msTimer * (expandablePanel->big_rect.w - expandablePanel->small_rect.w)/expandablePanel->msToSmall;
      expandablePanel->mobile_rect.h = expandablePanel->big_rect.h - expandablePanel->msTimer * (expandablePanel->big_rect.h - expandablePanel->small_rect.h)/expandablePanel->msToSmall;
      SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                   &expandablePanel->mobile_rect,
                   expandablePanel->colour);
    }
  }
}

int checkClickExpandablePanel(int click_x, int click_y, ExpandablePanel *expandablePanel){
  if(expandablePanel->status == SMALL && isPointInRect(click_x, click_y, expandablePanel->small_rect)){
    performClickExpandablePanel(expandablePanel);
    return(1);
  }
  else if(expandablePanel->status == BIG && isPointInRect(click_x, click_y, expandablePanel->big_rect)){
    performClickExpandablePanel(expandablePanel);
    return(1);
  }
  return 0;
}

void performClickExpandablePanel(ExpandablePanel *expandablePanel){
  int i = 0;
  if(expandablePanel->status == SMALL){
    expandablePanel->status = SMALL2BIG;
  }
  else if(expandablePanel->status == BIG){
    expandablePanel->status = BIG2SMALL;
  }
}

DraggableBlock createDraggableBlock(int x, int y, int w, int h, ExpandablePanel *parent, Uint32 colour){
  DraggableBlock draggableBlock;
  draggableBlock.rect.x = x;
  draggableBlock.rect.y = y;
  draggableBlock.rect.w = w;
  draggableBlock.rect.h = h;
  draggableBlock.colour = colour;
  draggableBlock.parent = parent;
  draggableBlock.status = ROOTED;
  ensureRectEnclosed(&draggableBlock.rect,&draggableBlock.parent->big_rect);
  return draggableBlock;

}

void updateDraggableBlock(GraphicsData *graphicsData, DraggableBlock *draggableBlock){
  if(draggableBlock->parent->status == BIG){
    SDL_FillRect(SDL_GetWindowSurface(graphicsData->window),
                 &draggableBlock->rect,
                 draggableBlock->colour);
  }
}

void checkMouseMoveDraggableBlock(int click_x, int click_y, DraggableBlock *draggableBlock){
  if(draggableBlock->status == GRABBED){
    draggableBlock->rect.x = click_x;
    draggableBlock->rect.y = click_y;
    ensureRectEnclosed(&draggableBlock->rect,&draggableBlock->parent->big_rect);

  }
}

int checkClickDownDraggableBlock(int click_x, int click_y, DraggableBlock *draggableBlock){
  if(isPointInRect(click_x,click_y,draggableBlock->rect) && draggableBlock->parent->status == BIG){
    if(draggableBlock->status == ROOTED){
      printf("block is now dragged\n");
      draggableBlock->status = GRABBED;
    }
    printf("clicked on draggable block\n");
    return(1);
  }
  return 0;
}

int checkClickUpDraggableBlock(int click_x, int click_y, DraggableBlock *draggableBlock){
  if(isPointInRect(click_x,click_y,draggableBlock->rect)){
    if(draggableBlock->status == GRABBED){
      ensureRectEnclosed(&draggableBlock->rect, &draggableBlock->parent->big_rect);
      draggableBlock->status = ROOTED;
    }
    return 1;
  }
  return 0;
}


void updateUI(UIData *uiData, GraphicsData *graphicsData, float ticks){
  int i = 0;
  while(i<uiData->numberOfSimpleButtons){
    updateSimpleButton(graphicsData,&uiData->simpleButtons[i]);
    i++;
  }
  i = 0;
  while(i < uiData->numberOfExpandablePanels){
    updateExpandablePanel(graphicsData,&uiData->expandablePanels[i],ticks);
    i++;
  }
  i = 0;
  while(i < uiData->numberOfDraggableBlocks){
    updateDraggableBlock(graphicsData,&uiData->draggableBlocks[i]);
    i++;
  }
}

void keydown(GraphicsData *graphicsData, SDL_Event *event){
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
        default:
            printf("redundant key pressed\n");
    }
}

void clickDownOnUI(UIData *uiData, SDL_Event *event){
  int i = 0;
  int checkBreak = 0;
  while(i < uiData->numberOfSimpleButtons){
    checkBreak = checkClickSimpleButton(event->button.x, event->button.y, &uiData->simpleButtons[i]);
    if(checkBreak){
      return;
    }
    i++;
  }
  i = 0;
  while(i < uiData->numberOfDraggableBlocks){
    checkBreak = checkClickDownDraggableBlock(event->button.x, event->button.y, &uiData->draggableBlocks[i]);
    if(checkBreak){
      return;
    }
    i++;
  }
  i = 0;
  while(i < uiData->numberOfExpandablePanels){
    checkBreak = checkClickExpandablePanel(event->button.x, event->button.y, &uiData->expandablePanels[i]);
    if(checkBreak){
      return;
    }
    i++;
  }
}

void moveMouseOnUi(UIData *uiData, SDL_Event *event){
  int i = 0;
  while(i < uiData->numberOfDraggableBlocks){
    checkMouseMoveDraggableBlock(event->motion.x, event->motion.y, &uiData->draggableBlocks[i]);
    i++;
  }
}

void clickUpOnUI(UIData *uiData, SDL_Event *event){
  int i = 0;
  while(i < uiData->numberOfDraggableBlocks){
    checkClickUpDraggableBlock(event->button.x, event->button.y, &uiData->draggableBlocks[i]);
    i++;
  }
}
