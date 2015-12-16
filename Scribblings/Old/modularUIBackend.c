#include "modularUIBackend.h"

int testRectIntersection(SDL_Rect rectA, SDL_Rect rectB){
	if (rectA.x+rectA.w<rectB.x || rectB.x+rectB.w<rectA.x || rectA.y+rectA.h<rectB.y || rectB.y+rectB.h<rectA.y){
		return 0;
	}
	return 1; /*There is intersection!*/
}

void preventOverlap(UI_Element *element){
	UI_Element *child = element->generic.parent->generic.child;
	SDL_Rect rectA,rectB,*element_rect,*child_rect;
	int i = 0;
	printf("get rectA\n");
	element_rect = getElementRect(element);
	rectA = getElementRectWithOffset(element);
	while(child!=NULL){
		if(child != element){
			printf("get rectB\n");
			rectB = getElementRectWithOffset(child);
			child_rect = getElementRect(child);
			i = testRectIntersection(rectA,rectB);
			if(i){
				child = element->generic.parent->generic.child;
			}
			while(i){
				element_rect->x -= 1;
				element_rect->y -= 1;
				rectA.x -= 1;
				rectA.y -= 1;
				/* This is incredibly risky, but I think it should be ok! */
				i = testRectIntersection(rectA,rectB);
			}
		}
		child = child->generic.sibling;
	}
}

SDL_Rect getTheoreticalBoundingBox(UI_Element *element, int padding){
	/* This gets a rectangle that in theory can hold a UI_Element and all its children */
	SDL_Rect bounding_box = *getElementRect(element);
	SDL_Rect *child_rect;
	UI_Element *child = element->generic.child;
	int i = 0;
	
	while(child != NULL){
		child_rect = getElementRect(child);
		if(child_rect->x < bounding_box.x){
			bounding_box.x = child_rect->x;
		}
		if(child_rect->y < bounding_box.y){
			bounding_box.y = child_rect->y;
		}
		child = child->generic.sibling;	
	}
	child = element->generic.child;
	while(child != NULL){ 
		i++;
		child_rect = getElementRect(child);
		if(child_rect->x + child_rect->w > bounding_box.x + bounding_box.w){
			bounding_box.w = child_rect->x + child_rect->w - bounding_box.x;
		}
		if(child_rect->y + child_rect->h > bounding_box.y + bounding_box.h){
			bounding_box.h = child_rect->y + child_rect->h - bounding_box.y;
		}
		child = child->generic.sibling;	
	}
	bounding_box.x += padding;
	bounding_box.y += padding;
	bounding_box.w += padding;
	bounding_box.h += padding;
	return bounding_box;
}

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

SDL_Rect *getElementRect(UI_Element *element){
	switch(element->type){
		case PANEL:
			return &element->panel.rect;
		case DRAGGABLE:
			return &element->draggable.rect;
		case EXPANDABLE:
			return &element->expandable.rect;
		default:
			return NULL;
	}
}

SDL_Rect getElementRectWithOffset(UI_Element *element){
	SDL_Rect rect = *getElementRect(element);
	if(element->generic.parent->type == PANEL){
		rect.x += element->generic.parent->panel.x_offset;
		rect.y += element->generic.parent->panel.y_offset;
	}
	if(element->type == PANEL){
		rect.x -= element->panel.x_offset;
		rect.y -= element->panel.y_offset;
	}
	return rect;
}

int shrinkRectToFit(SDL_Rect *shrink, SDL_Rect *limit){
	/* returns 1 if the rectangle is visible otherwise returns 0 */
	if(!testRectIntersection(*shrink,*limit)){
		/* It's outside of the limit, make it 0 */
		shrink->w = 0;
		shrink->h = 0;
		return 0;
	}
	else{
		if(shrink->x < limit->x){
			shrink->w -= limit->x - shrink->x;  
			shrink->x = limit->x;
		}
		else if(shrink->x + shrink->w > limit->x + limit->w){
			shrink->w -= shrink->x + shrink->w - limit->x - limit->w;
		}
		if(shrink->y < limit->y){
			shrink->h -= limit->y - shrink->y;  
			shrink->y = limit->y;
		}
		else if(shrink->y + shrink->h > limit->y + limit->h){
			shrink->h -= shrink->y + shrink->h - limit->y - limit->h;
		}
		return 1;
	}
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

UI_Element *createBlank(void){
  UI_Element *element = malloc(sizeof(UI_Element));

  element->generic.parent = NULL;
  element->generic.child = NULL;
  element->generic.sibling = NULL;
  element->generic.controller.function = NULL;
  element->generic.controller.elements = NULL;
  element->generic.response = NONE;
  element->generic.visibility = VISIBLE;
  element->generic.tangibility = TANGIBLE;
  element->generic.rendered = 1;

  return element;
}

UI_Element *createRoot(void){
  UI_Element *element = createBlank();
  element->generic.type = ROOT;
  return element;
}

int blankUI_Function(UI_Element *element){
  printf("blankUI_Function\n");
  return(0);
}

UI_Element *createUI_Button(int x, int y, int w, int h, int colour,UI_Controller_Function function,UI_Element **args){
  UI_Element *element = createBlank();

  element->generic.type = PANEL;
  element->generic.controller.function = function;
  element->generic.controller.elements = args;
  element->generic.response = CLICK_DOWN;
  
  
  element->panel.rect.x = x;
  element->panel.rect.y = y;
  element->panel.rect.h = h;
  element->panel.rect.w = w;
  element->panel.x_offset = 0;
  element->panel.y_offset = 0;
  element->panel.colour = colour;

  return element;
}

UI_Element *createUI_OffsetPanel(int x, int y, int w, int h, int colour,UI_Controller_Function function,UI_Element **args){
  UI_Element *element = createBlank();

  element->generic.type = PANEL;
  element->generic.controller.function = function;
  element->generic.controller.elements = args;
  element->generic.response = MOUSEMOTION;
  
  element->panel.rect.x = x;
  element->panel.rect.y = y;
  element->panel.rect.h = h;
  element->panel.rect.w = w;
  element->panel.colour = colour;

  return element;
}

UI_Element *createUI_Panel(int x, int y, int w, int h, int colour){
  UI_Element *element = createBlank();

  element->generic.type = PANEL;
  
  element->panel.rect.x = x;
  element->panel.rect.y = y;
  element->panel.rect.h = h;
  element->panel.rect.w = w;
  element->panel.colour = colour;

  return element;
}

UI_Element *createUI_Expandable(int s_x, int s_y, int s_w, int s_h,
                                int b_x, int b_y, int b_w, int b_h, int colour,
                                UI_Controller_Function function, UI_Element **args){
    UI_Element *element = createBlank();

    element->generic.type = EXPANDABLE;
    element->generic.controller.function = function;
    element->generic.controller.elements = args;
    element->generic.response = EXPANSION;


    element->expandable.small_rect.x = s_x;
    element->expandable.small_rect.y = s_y;
    element->expandable.small_rect.h = s_h;
    element->expandable.small_rect.w = s_w;
    element->expandable.big_rect.x = b_x;
    element->expandable.big_rect.y = b_y;
    element->expandable.big_rect.h = b_h;
    element->expandable.big_rect.w = b_w;
    element->expandable.status = SMALL;

    element->expandable.msTimer = 0;
    element->expandable.msToBig = 1000;
    element->expandable.msToSmall = 1000;

    element->expandable.rect = element->expandable.small_rect;
    element->expandable.colour = colour;

    return element;
}

UI_Element *createUI_Draggable(int x, int y, int w, int h, int colour,UI_Controller_Function function,UI_Element **args){
  UI_Element *element = createBlank();

  element->generic.type = DRAGGABLE;
  element->generic.controller.function = function;
  element->generic.controller.elements = args;
  element->generic.response = CLICK_DOWN;
  
  element->draggable.rect.x = x;
  element->draggable.rect.y = y;
  element->draggable.rect.h = h;
  element->draggable.rect.w = w;
  element->draggable.colour = colour;
  element->draggable.status = ROOTED;

  return element;
}

UI_Element *addChild(UI_Element *parent, UI_Element *child){
  UI_Element *element = parent->generic.child;
  if(element == NULL){
    parent->generic.child = child;
  }
  else{
    while(element->generic.sibling != NULL){
      element = element->generic.sibling;
    }
    element->generic.sibling = child;
  }
  child->generic.parent = parent;
  return child;
}

void renderUIElement(UI_Element *element, SDL_Window *window, int ticks){
	SDL_Rect render_rect;
	int colour;
  if(element->generic.visibility != VISIBLE || element->type == ROOT){
    return;
  }
  switch(element->generic.type){
    case PANEL:
	  render_rect = element->panel.rect;
	  colour = element->panel.colour;
      break;
    case EXPANDABLE:
      if(element->expandable.status == SMALL2BIG){
        element->expandable.msTimer += ticks;
        if(element->expandable.msTimer < element->expandable.msToBig){
          element->expandable.rect.x = element->expandable.small_rect.x + (element->expandable.big_rect.x - element->expandable.small_rect.x) * (element->expandable.msTimer / element->expandable.msToSmall);
          element->expandable.rect.y = element->expandable.small_rect.y + (element->expandable.big_rect.y - element->expandable.small_rect.y) * (element->expandable.msTimer / element->expandable.msToSmall);
          element->expandable.rect.w = element->expandable.small_rect.w + (element->expandable.big_rect.w - element->expandable.small_rect.w) * (element->expandable.msTimer / element->expandable.msToSmall);
          element->expandable.rect.h = element->expandable.small_rect.h + (element->expandable.big_rect.h - element->expandable.small_rect.h) * (element->expandable.msTimer / element->expandable.msToSmall);
        }
        else{
          element->expandable.status = BIG;
          element->expandable.msTimer = 0;
          element->expandable.rect = element->expandable.big_rect;
          if(element->generic.response == EXPANSION){
            element->generic.controller.function(element);
          }
        }
      }
      else if(element->expandable.status == BIG2SMALL){
        element->expandable.msTimer += ticks;
        if(element->expandable.msTimer < element->expandable.msToBig){
          element->expandable.rect.x = element->expandable.big_rect.x - (element->expandable.big_rect.x - element->expandable.small_rect.x) * (element->expandable.msTimer / element->expandable.msToSmall);
          element->expandable.rect.y = element->expandable.big_rect.y - (element->expandable.big_rect.y - element->expandable.small_rect.y) * (element->expandable.msTimer / element->expandable.msToSmall);
          element->expandable.rect.w = element->expandable.big_rect.w - (element->expandable.big_rect.w - element->expandable.small_rect.w) * (element->expandable.msTimer / element->expandable.msToSmall);
          element->expandable.rect.h = element->expandable.big_rect.h - (element->expandable.big_rect.h - element->expandable.small_rect.h) * (element->expandable.msTimer / element->expandable.msToSmall);
        }
        else{
          element->expandable.status = SMALL;
          element->expandable.msTimer = 0;
          element->expandable.rect = element->expandable.small_rect;
          if(element->generic.response == EXPANSION){
            element->generic.controller.function(element);
          }
        }
      }
	  render_rect = element->expandable.rect;
	  colour = element->expandable.colour;
      break;
	case DRAGGABLE:
	  render_rect = element->draggable.rect;
	  colour = element->draggable.colour;
	  break;
    default:
      break;
  }
  if(element->generic.parent->type == PANEL){
	render_rect.x += element->generic.parent->panel.x_offset;
	render_rect.y += element->generic.parent->panel.y_offset;
	element->generic.rendered = shrinkRectToFit(&render_rect,getElementRect(element->generic.parent));
  }
  SDL_FillRect(SDL_GetWindowSurface(window),
			   &render_rect,
			   colour);
}

void clickonUIElement(UI_Element *element, SDL_Event *event){
  SDL_Rect rect;
  if((element->generic.response == CLICK_DOWN || element->generic.response == CLICK_UP_OR_DOWN) 
      && element->generic.tangibility == TANGIBLE){
		rect = getElementRectWithOffset(element);
		if(element->generic.type == PANEL){
			if(isPointInRect(event->button.x, event->button.y, rect)){
				element->generic.controller.function(element);
			}
		}
		if(element->generic.type == DRAGGABLE){
			if(isPointInRect(event->button.x, event->button.y, rect)){
				element->generic.controller.function(element);
			}
		}
  }
}

void clickuponUIElement(UI_Element *element, SDL_Event *event){
  if((element->generic.response == CLICK_UP || element->generic.response == CLICK_UP_OR_DOWN)
     && element->generic.tangibility == TANGIBLE){
		element->generic.controller.function(element);
  }
}

void motiononUIElement(UI_Element *element, SDL_Event *event){
  SDL_Rect parent_rect;
  
  if(element->generic.type == PANEL && element->generic.response == MOUSEMOTION){
	element->generic.controller.function(element);
  }
  if(element->generic.type == DRAGGABLE && element->draggable.status == GRABBED){
	element->draggable.rect.x = event->motion.x;
	element->draggable.rect.y = event->motion.y;
	if(element->generic.parent->type == PANEL){
		element->draggable.rect.x -= element->generic.parent->panel.x_offset;
		element->draggable.rect.y -= element->generic.parent->panel.y_offset;
	}
	parent_rect = getElementRectWithOffset(element->generic.parent);
	ensureRectEnclosed(&element->draggable.rect,&parent_rect);
  }
}

/* Sequential UI Rendering and so on */

void renderUI(UI_Element *root, SDL_Window *window, int ticks){
  UI_Element *queue[255] = {NULL};
  UI_Element *queue_element;
  UI_Element *queue_element_child;
  int front = 0, back = 1;
  assert(root->type == ROOT);
  queue[0] = root;
  while(front != back){
    queue_element = queue[front];
    front = (front + 1) % 255;
    renderUIElement(queue_element,window,ticks);
    queue_element_child = queue_element->generic.child;
    while(queue_element_child != NULL){
      queue[back] = queue_element_child;
      back = (back + 1) % 255;
      queue_element_child = queue_element_child->generic.sibling;
    }
  }
}

void clickonUI(UI_Element *root, SDL_Event *event){
  UI_Element *queue[255] = {NULL};
  UI_Element *queue_element;
  UI_Element *queue_element_child;
  int front = 0, back = 1;
  assert(root->type == ROOT);
  queue[0] = root;
  while(front != back){
    queue_element = queue[front];
    front = (front + 1) % 255;
    clickonUIElement(queue_element, event);
    queue_element_child = queue_element->generic.child;
    while(queue_element_child != NULL){
      queue[back] = queue_element_child;
      back = (back + 1) % 255;
      queue_element_child = queue_element_child->generic.sibling;
    }
  }
}

void motiononUI(UI_Element *root, SDL_Event *event){
  UI_Element *queue[255] = {NULL};
  UI_Element *queue_element;
  UI_Element *queue_element_child;
  int front = 0, back = 1;
  assert(root->type == ROOT);
  queue[0] = root;
  while(front != back){
    queue_element = queue[front];
    front = (front + 1) % 255;
    motiononUIElement(queue_element, event);
    queue_element_child = queue_element->generic.child;
    while(queue_element_child != NULL){
      queue[back] = queue_element_child;
      back = (back + 1) % 255;
      queue_element_child = queue_element_child->generic.sibling;
    }
  }
}

void clickuponUI(UI_Element *root, SDL_Event *event){
  UI_Element *queue[255] = {NULL};
  UI_Element *queue_element;
  UI_Element *queue_element_child;
  int front = 0, back = 1;
  assert(root->type == ROOT);
  queue[0] = root;
  while(front != back){
    queue_element = queue[front];
    front = (front + 1) % 255;
    clickuponUIElement(queue_element, event);
    queue_element_child = queue_element->generic.child;
    while(queue_element_child != NULL){
      queue[back] = queue_element_child;
      back = (back + 1) % 255;
      queue_element_child = queue_element_child->generic.sibling;
    }
  }
}
