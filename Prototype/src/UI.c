#include "UI.h"


#define TWORECTOVERRIDE_SX integers[0]
#define TWORECTOVERRIDE_SY integers[1]
#define TWORECTOVERRIDE_SW integers[2]
#define TWORECTOVERRIDE_SH integers[3]
#define TWORECTOVERRIDE_BX integers[4]
#define TWORECTOVERRIDE_BY integers[5]
#define TWORECTOVERRIDE_BW integers[6]
#define TWORECTOVERRIDE_BH integers[7]
#define TWORECTOVERRIDE_COUNTER integers[8]
#define TWORECTOVERRIDE_TRANSITION integers[9]

#define RENDERRECT_R integers[0]
#define RENDERRECT_B integers[1]
#define RENDERRECT_G integers[2]

static void UIAction_Init(UI_Element *element, UI_Action *action);
static void freeUIAction(UI_Action *action);
static void UITrigger_Execute(UI_Action *action);
static int isLinked(UI_Element *test, UI_Element *to);
static int UIElement_isVisible(UI_Element *element);
static int percWindowDimension(int dimension, float perc);

UI_Element *UIElement_CreateByPercentage(float rx, float ry, float rw, float rh, int x, int y, int num_of_actions){
	   UI_Element *element;
		 element  = calloc(1,sizeof(UI_Element));


	   element->rect.x = percWindowDimension(x,rx);
	   element->rect.y = percWindowDimension(y,ry);
	   element->rect.w = percWindowDimension(x,rw);
	   element->rect.h = percWindowDimension(y,rh);
	   element->actions = malloc(sizeof(UI_Action)*num_of_actions);
	   element->num_of_actions = num_of_actions;
	   element->parent = NULL;
	   element->child = NULL;
	   element->sibling = NULL;
	return element;
}

UI_Element *UIElement_Create(int x, int y, int w, int h, int num_of_actions){
	   UI_Element *element;
		 element  = calloc(1,sizeof(UI_Element));


	   element->rect.x = x;
	   element->rect.y = y;
	   element->rect.w = w;
	   element->rect.h = h;
	   element->actions = malloc(sizeof(UI_Action)*num_of_actions);
	   element->num_of_actions = num_of_actions;
	   element->parent = NULL;
	   element->child = NULL;
	   element->sibling = NULL;
		 return element;
}

int UIAction_DisplayImage(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	va_copy(vargs, copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);

	if(action->status){
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&action->element->rect);
	}
	return 1;
}

int UIAction_Auto(UI_Action *action, va_list copy_from){
	int i = 0;
	if(action->status == 1){
		UITrigger_Execute(action);
	}
	return action->status;
}

int UIAction_External(UI_Action *action, va_list copy_from){
	int i = 0;
	if(action->status == 1 && action->external != NULL){
		UITrigger_Execute(action);
		action->status = 0;
		return 1;
	}
	return 0;
}

int UIAction_ShrinkFitToParent(UI_Action *action, va_list copy_from){
	SDL_Rect temp_rect;
	SDL_Rect *parent_rect = &action->element->parent->rect;
	if(action->status == 1){
			temp_rect.x = action->integers[0];
			temp_rect.y = action->integers[1];
			temp_rect.w = action->integers[2];
			temp_rect.h = action->integers[3];
			/* returns 1 if the rectangle is visible otherwise returns 0 */
			if(!testRectIntersection(temp_rect,*parent_rect)){
				/* It's outside of the limit, make it 0 */
				temp_rect.w = 0;
				temp_rect.h = 0;
			}
			else{
				if(temp_rect.x < parent_rect->x){
					temp_rect.x = parent_rect->x;
				}
				if(temp_rect.x + temp_rect.w > parent_rect->x + parent_rect->w){
					temp_rect.w = (parent_rect->x + parent_rect->w) - temp_rect.x;
				}

				if(temp_rect.y < parent_rect->y){
					temp_rect.h -= parent_rect->y - temp_rect.y;
					temp_rect.y = parent_rect->y;
				}
				else if(temp_rect.y + temp_rect.h > parent_rect->y + parent_rect->h){
					temp_rect.h -= temp_rect.y + temp_rect.h - parent_rect->y - parent_rect->h;
				}
			}
			action->element->rect = temp_rect;
			return 1;
	}
	return 0;
}

int UIAction_DisplayNumber(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);
	if(action->integers[0] != action->integers[1]){
		action->integers[1] = action->integers[0];
		sprintf(action->strings[0],"%d",action->integers[1]);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Solid(graphicsData->fonts[action->integers[2]],action->strings[0],colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
	}
	if(UIElement_isVisible(action->element) && action->status != 0 && action->texture != NULL){
		temp_rect.x = action->element->rect.x;
		temp_rect.y = action->element->rect.y;
		TTF_SizeText(graphicsData->fonts[action->integers[2]], action->strings[0], &temp_rect.w, &temp_rect.h);
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
	}
	return 1;
}

int UIAction_DisplayString(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);
	if(action->strings[0] != NULL && action->strings[1] != NULL && strcmp(action->strings[0],action->strings[1]) != 0){
		strcpy(action->strings[0],action->strings[1]);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Solid(graphicsData->fonts[action->integers[0]],action->strings[0],colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
	}
	if(UIElement_isVisible(action->element) && action->status != 0 && action->texture != NULL){
		temp_rect.x = action->element->rect.x;
		temp_rect.y = action->element->rect.y;
		TTF_SizeText(graphicsData->fonts[action->integers[0]], action->strings[0], &temp_rect.w, &temp_rect.h);
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
	}
	return 1;
}

int UIAction_ResourceCounter(UI_Action *action, va_list copy_from){
	va_list vargs;
	int i = 0;
	GameObjectData *gameObjectData;
	va_copy(vargs, copy_from);
	gameObjectData = va_arg(vargs, GameObjectData*);
	va_end(vargs);
	if(action->status == 1){
		action->integers[0] = gameObjectData->hive.flowers_collected;
		while(i < action->num_of_companions){
			action->companions[i]->integers[0] = action->integers[0];
			i++;
		}
	}
	return 1;
}

int UIAction_Counter(UI_Action *action, va_list copy_from){
	action->status = 0;
	return 1;
}

int UIAction_RenderLine(UI_Action *action, va_list copy_from){
	int bx,by;
	GraphicsData *graphicsData;
	va_list vargs;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs, GraphicsData*);
	va_end(vargs);
	if(!UIElement_isVisible(action->element)){
		return 0;
	}
	if(action->status == 3){
		if(action->external == NULL || isLinked(action->external,action->element)){
			action->external = NULL;
			action->status = 0;
		}
		else{
			action->status = 2;
		}
	}
	if(action->status == 1){
		SDL_SetRenderDrawColor(graphicsData->renderer,255,
										255,
										255,
										255);
		SDL_RenderDrawLine(graphicsData->renderer,action->element->rect.x+action->integers[2],action->element->rect.y+action->integers[3],
									action->integers[0],action->integers[1]);
		return 1;
	}
	else if(action->status == 2){
		if(action->external == NULL){
			action->status = 0;
			return 0;
		}
		SDL_SetRenderDrawColor(graphicsData->renderer,255,
										255,
										255,
										255);
		bx = action->external->rect.x + action->element->rect.w/2;
		by = action->external->rect.y;
		SDL_RenderDrawLine(graphicsData->renderer,action->element->rect.x+action->integers[2],action->element->rect.y+action->integers[3],
									bx,by);
		return 1;
	}
	else if(action->status == 4){
		action->external = NULL;
		action->status = 0;
		action->integers[0] = action->element->rect.x;
		action->integers[1] = action->element->rect.y;
		return 0;
	}
	return 0;
}

int UIAction_ClickAnywhere(UI_Action *action, va_list copy_from){
	int i = 0;
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

int UIAction_ReleaseAnywhere(UI_Action *action, va_list copy_from){
	int i = 0;
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

int UIAction_CalculateSibling(UI_Action *action, va_list copy_from){
	UI_Element *element;
	int i = 0;
	if(action->status == 1){
		element = action->element->parent->child;
		while(element != NULL){
			if(element != action->element){
				if(!(action->integers[0] < element->rect.x || action->integers[0] > element->rect.x + element->rect.w) &&
					 !(action->integers[1] < element->rect.y || action->integers[1] > element->rect.y + element->rect.h)){
						break;
					 }
			}
			element = element->sibling;
		}
		while(i < action->num_of_companions){
			action->companions[i]->external = element;
			i++;
		}
		action->status = 0;
	}
	return 1;
}

int UIAction_StoreMousePosition(UI_Action *action, va_list copy_from){
	SDL_Event *event;
	int i = 0;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);
	action->integers[0] = event->motion.x;
	action->integers[1] = event->motion.y;
	if(action->status == 1){
		while(i < action->num_of_companions){
			action->companions[i]->integers[0] = action->integers[0];
			action->companions[i]->integers[1] = action->integers[1];
			i++;
		}
	}
	return 1;
}

int UIAction_FillRect(UI_Action *action, va_list copy_from){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	va_list vargs;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);
	if(action->status != 0){
		SDL_SetRenderDrawColor(graphicsData->renderer,action->RENDERRECT_R,
		                                action->RENDERRECT_G,
		                                action->RENDERRECT_B,
		                                255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
	}
	return 1;
}

int UIAction_ClickRect(UI_Action *action, va_list copy_from){
	/* This should get a SDL_Event pointer */
	SDL_Event *event;
	int i = 0;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);

	if(action->status == 0){return 0;}

	if(event->button.x < action->element->rect.x || event->button.x > action->element->rect.x + action->element->rect.w){
	  return 0;
	}
	if(event->button.y < action->element->rect.y || event->button.y > action->element->rect.y + action->element->rect.h){
	  return 0;
	}
	UITrigger_Execute(action);
	return 1;
}

int UIAction_DraggableRectOverride(UI_Action *action, va_list copy_from){
	SDL_Event *event;
	int x,y,i=0;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);

	/* UIACTION_INTS is the status, 0 = rooted, 1 = grabbed */

	if(action->status == 1){
		x = event->motion.x;
		y = event->motion.y;
		if(x < action->element->parent->rect.x){
			x = action->element->parent->rect.x;
		}
		else if(x + action->element->rect.w > action->element->parent->rect.x + action->element->parent->rect.w){
			x = action->element->parent->rect.x + action->element->parent->rect.w - action->element->rect.w;
		}

		if(y < action->element->parent->rect.y){
			y = action->element->parent->rect.y;
		}
		else if(y + action->element->rect.h > action->element->parent->rect.y + action->element->parent->rect.h){
			y = action->element->parent->rect.y + action->element->parent->rect.h - action->element->rect.h;
		}

	  action->element->rect.x = x;
	  action->element->rect.y = y;
		while(i < action->num_of_companions){
			action->companions[i]->integers[0] = x;
			action->companions[i]->integers[1] = y;
			i++;
		}
	}
	return 1;
}

int UIAction_TwoRectOverride(UI_Action *action, va_list copy_from){
	int dt;
	float transition_multiplier;
	va_list vargs;
	va_copy(vargs,copy_from);
	dt = va_arg(vargs,int);
	va_end(vargs);


	if(action->status == 1){
	  /* We're going big! */
	  action->TWORECTOVERRIDE_COUNTER+=dt;
	  if(action->TWORECTOVERRIDE_COUNTER > action->TWORECTOVERRIDE_TRANSITION){
	    /* We've become big */
	    action->status = 3;
	    action->TWORECTOVERRIDE_COUNTER = action->TWORECTOVERRIDE_TRANSITION;
	  }
	  transition_multiplier = ((float) action->TWORECTOVERRIDE_COUNTER)/((float) action->TWORECTOVERRIDE_TRANSITION);
	  action->element->rect.x = action->TWORECTOVERRIDE_SX + (action->TWORECTOVERRIDE_BX - action->TWORECTOVERRIDE_SX) * transition_multiplier;
	  action->element->rect.y = action->TWORECTOVERRIDE_SY + (action->TWORECTOVERRIDE_BY - action->TWORECTOVERRIDE_SY) * transition_multiplier;
	  action->element->rect.w = action->TWORECTOVERRIDE_SW + (action->TWORECTOVERRIDE_BW - action->TWORECTOVERRIDE_SW) * transition_multiplier;
	  action->element->rect.h = action->TWORECTOVERRIDE_SH + (action->TWORECTOVERRIDE_BH - action->TWORECTOVERRIDE_SH) * transition_multiplier;
	}
	else if(action->status == 2){
	  /* We're going small! */
	  action->TWORECTOVERRIDE_COUNTER-=dt;
	  if(action->TWORECTOVERRIDE_COUNTER < 0){
	    /* We've become small */
	    action->status = 0;
	    action->TWORECTOVERRIDE_COUNTER = 0;
	  }
	  transition_multiplier = ((float) action->TWORECTOVERRIDE_COUNTER)/((float) action->TWORECTOVERRIDE_TRANSITION);
	  action->element->rect.x = action->TWORECTOVERRIDE_SX + (action->TWORECTOVERRIDE_BX - action->TWORECTOVERRIDE_SX) * transition_multiplier;
	  action->element->rect.y = action->TWORECTOVERRIDE_SY + (action->TWORECTOVERRIDE_BY - action->TWORECTOVERRIDE_SY) * transition_multiplier;
	  action->element->rect.w = action->TWORECTOVERRIDE_SW + (action->TWORECTOVERRIDE_BW - action->TWORECTOVERRIDE_SW) * transition_multiplier;
	  action->element->rect.h = action->TWORECTOVERRIDE_SH + (action->TWORECTOVERRIDE_BH - action->TWORECTOVERRIDE_SH) * transition_multiplier;
	}
	return 1;
}

int UIAction_GenerateAIString(UI_Action *action, va_list copy_from){
	/* This is hacky and shouldn't be used outside of this specific project */
	int primary_search = 0;
	int primary = 0;
	int secondary_search = 0;
	int secondary = 0;
	char working_space[300] = {'\0'};
	UI_Element *worker = action->element->parent->child;
	/* Copy in the default string */
	action->strings[1] = realloc(action->strings[1],strlen(action->strings[0])+1);
	strcpy(action->strings[1],action->strings[0]);
	if(action->companions[0]->external != NULL){
		memset(working_space, 0, 300);
		while(worker != NULL){
			primary_search++;
			if(action->companions[0]->external == worker){
				primary = primary_search;
				break;
			}
			worker = worker->sibling;
		}
		if(primary != 0){
			action->strings[1] = realloc(action->strings[1],strlen(action->strings[1]) + 1 + strlen(working_space)+1);
			strcat(action->strings[1],working_space);
		}
	}
	if(action->companions[1]->external != NULL){
		memset(working_space, 0, 300);
		worker = action->element->parent->child;
		while(worker != NULL){
			secondary_search++;
			if(action->companions[1]->external == worker){
				secondary = secondary_search;
				break;
			}
			worker = worker->sibling;
		}
		if(secondary != 0){
			action->strings[1] = realloc(action->strings[1],strlen(action->strings[1]) + 1 + strlen(working_space)+1);
			strcat(action->strings[1],working_space);
		}
	}
	return 1;
}

void UIConfigure_GenerateAIString(UI_Element *element, UI_Action *action, char *string, UI_Action *linkPrimary, UI_Action *linkSecondary){
	UIAction_Init(element,action);
	action->response = EXTERNAL;
	action->function = UIAction_GenerateAIString;
	action->companions = malloc(sizeof(UI_Action*));
	action->companions[0] = linkPrimary;
	action->companions[1] = linkSecondary;
	action->num_of_companions = 2;
	action->strings = malloc(sizeof(char*)*2);
	action->strings[0] = malloc(strlen(string)+1 * sizeof(char));
	action->strings[1] = malloc(strlen(string)+1 * sizeof(char));
	strcpy(action->strings[0],string);
	strcpy(action->strings[1],string);
}

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b){
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_FillRect;
	action->integers = malloc(sizeof(int) * 3);
	action->num_of_integers = 3;

	action->RENDERRECT_R = r;
	action->RENDERRECT_G = g;
	action->RENDERRECT_B = b;
}

void UIConfigure_LeftClickRect(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickRect;
}

void UIConfigure_RightClickRect(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickRect;
}

void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action, int sx, int sy, int sw, int sh, int bx, int by, int bw, int bh, int delay, int small_triggers, int big_triggers,...){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_TwoRectOverride;
	action->element = element;
	action->status = 0;
	action->integers = malloc(sizeof(int) * 10);
	action->num_of_integers = 10;
	/* Small */
	action->TWORECTOVERRIDE_SX = sx;
	action->TWORECTOVERRIDE_SY = sy;
	action->TWORECTOVERRIDE_SW = sw;
	action->TWORECTOVERRIDE_SH = sh;
	/* Big */
	action->TWORECTOVERRIDE_BX = bx;
	action->TWORECTOVERRIDE_BY = by;
	action->TWORECTOVERRIDE_BW = bw;
	action->TWORECTOVERRIDE_BH = bh;
	/* State  0 = SMALL*/
	/* Time */
	action->TWORECTOVERRIDE_COUNTER = 0;
	/* Transition time */
	action->TWORECTOVERRIDE_TRANSITION = delay;
}

void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = MOTION;
	action->function = UIAction_DraggableRectOverride;
	action->status = 0;
	action->num_of_companions = num_of_companions;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

void UIConfigure_RenderLine(UI_Element *element, UI_Action *action, enum LineOrigins origin, UI_Element *external){
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_RenderLine;
	action->external = external;
	action->status = 2;
	action->integers = calloc(4,sizeof(int));
	action->num_of_integers = 4;

	action->integers[0] = element->rect.x;
	action->integers[1] = element->rect.y;
	switch(origin){
		case CENTER:
			action->integers[2] = (element->rect.w/2);
			action->integers[3] = (element->rect.h/2);
			break;
		case BL_CORNER:
			action->integers[2] = 0;
			action->integers[3] = element->rect.h;
			break;
		case BR_CORNER:
			action->integers[2] = element->rect.w;
			action->integers[3] = element->rect.h;
			break;
		case TL_CORNER:
			action->integers[2] = 0;
			action->integers[3] = 0;
			break;
		case TR_CORNER:
			action->integers[2] = element->rect.w;
			action->integers[3] = 0;
			break;
	}
}

void UIConfigure_StoreMousePosition(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = MOTION;
	action->function = UIAction_StoreMousePosition;

	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	while(i < num_of_companions){
	action->companions[i] = va_arg(vargs,UI_Action*);
	i++;
	}
	action->num_of_companions = num_of_companions;

	action->status = 0;
	action->integers = malloc(sizeof(int) * 2);
	action->num_of_integers = 2;
	va_end(vargs);
}

void UIConfigure_CalculateSibling(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = UPDATE;
	action->function = UIAction_CalculateSibling;

	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	while(i < num_of_companions){
	action->companions[i] = va_arg(vargs,UI_Action*);
	i++;
	}
	action->num_of_companions = num_of_companions;

	action->status = 0;
	action->integers = malloc(sizeof(int) * 2);
	action->num_of_integers = 2;
	va_end(vargs);
}

void UIConfigure_LeftClickAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickAnywhere;
}

void UIConfigure_RightClickAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickAnywhere;
}

void UIConfigure_LeftReleaseAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = LEFT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
}

void UIConfigure_RightReleaseAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = RIGHT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
}

void UIConfigure_Counter(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_Counter;
	action->status = 0;
}

void UIConfigure_External(UI_Element *element, UI_Action *action, UI_Element *external){
	UIAction_Init(element,action);
	action->response = EXTERNAL;
	action->function = UIAction_External;
	action->external = external;
	action->status = 0;
}

void UIConfigure_ResourceCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = GAME_OBJECT_UPDATE;
	action->function = UIAction_ResourceCounter;
	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->integers = calloc(1, sizeof(int));
	action->num_of_integers = 1;
	va_end(vargs);
}

void UIConfigure_DisplayString(UI_Element *element, UI_Action *action, char *string, int font){
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_DisplayString;
	action->strings = malloc(sizeof(char*) * 2);
	if(string != NULL){
		action->strings[0] = calloc(strlen(string)+1,sizeof(char));
		action->strings[1] = calloc(strlen(string)+1,sizeof(char));
		strcpy(action->strings[1], string);
	}
	else{
		action->strings[0] = NULL;
		action->strings[1] = NULL;
	}
	action->num_of_strings = 2;
	action->integers = calloc(1,sizeof(int));
	action->integers[0] = font;
}

void UIConfigure_DisplayNumber(UI_Element *element, UI_Action *action, int number, int font){
	UIAction_Init(element,action);
	action->response = RENDER;
	action->status = 1;
	action->function = UIAction_DisplayNumber;
	action->integers = calloc(3, sizeof(int));
	action->integers[0] = number;
	action->integers[2] = font;
	action->num_of_integers = 3;
	action->strings = malloc(sizeof(char*) * 1);
	action->strings[0] = calloc(30,sizeof(char));
	action->num_of_strings = 1;
}

void UIConfigure_ShrinkFitToParent(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_ShrinkFitToParent;
	action->integers = calloc(4,(sizeof(int)));
	action->integers[0] = element->rect.x;
	action->integers[1] = element->rect.y;
	action->integers[2] = element->rect.w;
	action->integers[3] = element->rect.h;
}

void UIConfigure_Auto(UI_Element *element, UI_Action *action, enum Response response){
	UIAction_Init(element,action);
	action->response = response;
	action->function = UIAction_Auto;
}

void UIConfigure_DisplayImage(UI_Element *element, UI_Action *action, SDL_Texture *image){
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_DisplayImage;
	action->texture = image;
}

static void UITrigger_Execute(UI_Action *action){
	UI_Trigger *trigger = action->triggers;
	while(trigger!=NULL){
		if(trigger->action != NULL){
			if(trigger->status_from == -1 || trigger->action->status == trigger->status_from){
				if(trigger->status_to == UITRIGGER_PLUSONE){
					trigger->action->status++;
				}
				else{
					trigger->action->status = trigger->status_to;
				}
			}
		}
		trigger = trigger->next;
	}
}

void UIElement_Free(UI_Element *element){
	int i = 0;
	while(i < element->num_of_actions){
	  freeUIAction(&element->actions[i]);
	  i++;
	}
	free(element->actions);
	free(element);
}

static void freeUIAction(UI_Action *action){
	free(action->triggers);
	if(action->num_of_integers){
	  action->num_of_integers = 0;
	  free(action->integers);
	}
	return;
}

void UIElement_Reparent(UI_Element *element, UI_Element *parent){
	UI_Element *child = parent->child;
	int testing = 0;
	if(element->parent != NULL){
		UIElement_Deparent(element);
	}
	if(child == NULL){
		parent->child = element;
	}
	else{
		while(child->sibling != NULL){
			child = child->sibling;
			testing++;
		}
		child->sibling = element;
	}
	element->parent = parent;
}

void UIElement_Deparent(UI_Element *element){
	UI_Element *parent = element->parent;
	UI_Element *child = parent->child;
	if(child == element){
		parent->child = element->sibling;
	}
	else{
		while(child->sibling != NULL){
			if(child->sibling == element){
				child->sibling = element->sibling;
				break;
			}
		child = child->sibling;
		}
	}
	element->parent = NULL;
}

void UIElement_DeExternalise(UI_Element *element){
	/* Cleans all external references in actions to a given element,
		 preventing segfaults */
	return;
}

void UIElement_Execute(UI_Element *element, enum Response response, va_list vargs){
	int i = 0;
	while(i < element->num_of_actions){
		if(element->actions[i].response == response){
			element->actions[i].function(&element->actions[i],vargs);
		}
		i++;
	}
}

static int isLinked(UI_Element *test, UI_Element *to){
	int i = 0;
	while(i < test->num_of_actions){
		if(test->actions[i].function == UIAction_RenderLine && test->actions[i].external == to){
			return 1;
		}
		i++;
	}
	return 0;
}

void UITrigger_Bind(UI_Action *action, UI_Action *target, int status_from, int status_to){
	UI_Trigger *trigger = malloc(sizeof(UI_Trigger));
	UI_Trigger *movingPointer = NULL;

	trigger->action = target;
	trigger->status_from = status_from;
	trigger->status_to = status_to;
	trigger->next = NULL;

	action->num_of_triggers++;

	if(action->triggers == NULL){
		action->triggers = trigger;
	}
	else{
		movingPointer = action->triggers;
		while(movingPointer->next != NULL){
			movingPointer = movingPointer->next;
		}
		movingPointer->next = trigger;
	}
}

void UIRoot_Execute(UIData *uiData, enum Response response, ...){
	va_list vargs;
  UI_Element *queue[255] = {NULL};
  UI_Element *queue_element;
  UI_Element *queue_element_child;
  int front = 0, back = 1;
	va_start(vargs, response);
  queue[0] = uiData->root;
  while(front != back){
    queue_element = queue[front];
    front = (front + 1) % 255;
    UIElement_Execute(queue_element,response,vargs);
    queue_element_child = queue_element->child;
    while(queue_element_child != NULL){
      queue[back] = queue_element_child;
      back = (back + 1) % 255;
      queue_element_child = queue_element_child->sibling;
    }
  }
	va_end(vargs);
}

static void UIAction_Init(UI_Element *element, UI_Action *action){
	action->response = NONE;
	action->function = NULL;
	action->element = element;
	action->external = NULL;
	action->status = 1;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->integers = NULL;
	action->triggers = NULL;
	action->num_of_triggers = 0;
	action->strings = NULL;
	action->num_of_strings = 0;
	action->texture = NULL;
}

static int UIElement_isVisible(UI_Element *element){
	return element->rect.w && element->rect.h;
}

static int percWindowDimension(int dimension, float perc){
	return (int) ((float) dimension * perc);
}

void UIRoot_Destroy(UIData *uiData){
  UI_Element *queue[255] = {NULL};
  UI_Element *queue_element;
  UI_Element *queue_element_child;
  int front = 0, back = 1;
  queue[0] = uiData->root;
  while(front != back){
    queue_element = queue[front];
    front = (front + 1) % 255;
    queue_element_child = queue_element->child;
    while(queue_element_child != NULL){
      queue[back] = queue_element_child;
      back = (back + 1) % 255;
      queue_element_child = queue_element_child->sibling;
    }
    UIElement_Free(queue_element);
  }
}
