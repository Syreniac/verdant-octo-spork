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


static void freeUIAction(UI_Action *action);
static void UITrigger_Execute(UI_Trigger *trigger);
static void getCenterOfRect(SDL_Rect *rect, int *x, int *y);
static int isLinked(UI_Element *test, UI_Element *to);

int UIAction_Counter(UI_Action *action, va_list copy_from){
	action->status = 0;
	return 1;
}

int UIAction_RenderLine(UI_Action *action, va_list copy_from){
	int ax,ay,bx,by;
	GraphicsData *graphicsData;
	va_list vargs;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs, GraphicsData*);
	va_end(vargs);
	if(action->status == 3){
		if(action->external == NULL || isLinked(action->external,action->element)){
			action->external = NULL;
			action->status = 0;
			printf("rejecting link\n");
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
		getCenterOfRect(&action->element->rect,&ax,&ay);
		SDL_RenderDrawLine(graphicsData->renderer,ax,ay,
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
		ax = action->element->rect.x + action->element->rect.w/2;
		ay = action->element->rect.y + action->element->rect.h;
		bx = action->external->rect.x + action->element->rect.w/2;
		by = action->external->rect.y;
		SDL_RenderDrawLine(graphicsData->renderer,ax,ay,
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
		while(i < action->num_of_triggers){
			UITrigger_Execute(&action->triggers[i]);
			i++;
		}
		return 1;
	}
	return 0;
}

int UIAction_ReleaseAnywhere(UI_Action *action, va_list copy_from){
	int i = 0;
	if(action->status == 1){
		while(i < action->num_of_triggers){
			UITrigger_Execute(&action->triggers[i]);
			i++;
		}
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
		printf("%d | %d\n",i,action->num_of_companions);
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
	SDL_SetRenderDrawColor(graphicsData->renderer,action->RENDERRECT_R,
	                                action->RENDERRECT_G,
	                                action->RENDERRECT_B,
	                                255);
	SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
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
	printf("clickrect\n");

	if(event->button.x < action->element->rect.x || event->button.x > action->element->rect.x + action->element->rect.w){
	  return 0;
	}
	if(event->button.y < action->element->rect.y || event->button.y > action->element->rect.y + action->element->rect.h){
	  return 0;
	}
	while(i < action->num_of_triggers){
	  UITrigger_Execute(&action->triggers[i]);
	  i++;
	}
	return 1;
}

int UIAction_DraggableRectOverride(UI_Action *action, va_list copy_from){
	SDL_Event *event;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);

	/* UIACTION_INTS is the status, 0 = rooted, 1 = grabbed */

	if(action->status == 1){
	  action->element->rect.x = event->motion.x;
	  action->element->rect.y = event->motion.y;
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
	action->strings[1] = realloc(action->strings[1],strlen(action->strings[0]));
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
			sprintf(working_space,"\n\tprimary = %d",primary);
			action->strings[1] = realloc(action->strings[1],strlen(action->strings[1]) + strlen(working_space)+1);
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
			sprintf(working_space,"\n\tsecondary = %d",secondary);
			action->strings[1] = realloc(action->strings[1],strlen(action->strings[1]) + strlen(working_space)+1);
			strcat(action->strings[1],working_space);
		}
	}
	printf("%s\n",action->strings[1]);
	printf("--------------------\n");
	return 1;
}

void UIConfigure_GenerateAIString(UI_Element *element, UI_Action *action, char *string, UI_Action *linkPrimary, UI_Action *linkSecondary){
	printf("UIConfigure_GenerateAIString\n");
	action->response = EXTERNAL;
	action->function = UIAction_GenerateAIString;
	action->element = element;
	action->external = NULL;
	action->companions = malloc(sizeof(UI_Action*));
	action->companions[0] = linkPrimary;
	action->companions[1] = linkSecondary;
	action->num_of_companions = 2;
	action->status = 1;
	action->integers = NULL;
	action->num_of_integers = 0;
	action->triggers = NULL;
	action->num_of_triggers = 0;
	action->strings = malloc(sizeof(char*)*2);
	action->strings[0] = malloc(strlen(string) * sizeof(char));
	action->strings[1] = malloc(strlen(string) * sizeof(char));
	strcpy(action->strings[0],string);
	strcpy(action->strings[1],string);
}

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b){
	printf("UIConfigure_FillRect\n");
	action->response = RENDER_BASE;
	action->function = UIAction_FillRect;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 1;
	action->integers = malloc(sizeof(int) * 3);
	action->num_of_integers = 3;
	action->triggers = NULL;
	action->num_of_triggers = 0;

	action->RENDERRECT_R = r;
	action->RENDERRECT_G = g;
	action->RENDERRECT_B = b;
}

void UIConfigure_LeftClickRect(UI_Element *element, UI_Action *action){
	printf("UIConfigure_LeftClickRect\n");
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickRect;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 1;

	action->integers = NULL;
	action->num_of_integers = 0;

	action->triggers = NULL;
	action->num_of_triggers = 0;
}

void UIConfigure_RightClickRect(UI_Element *element, UI_Action *action){
	printf("UIConfigure_RightClickRect\n");
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickRect;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 1;

	action->integers = NULL;
	action->num_of_integers = 0;

	action->triggers = NULL;
	action->num_of_triggers = 0;
}

void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action, int sx, int sy, int sw, int sh, int bx, int by, int bw, int bh, int small_triggers, int big_triggers,...){
	printf("UIConfigure_TwoRectOverride\n");
	action->response = UPDATE;
	action->function = UIAction_TwoRectOverride;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;
	action->integers = malloc(sizeof(int) * 10);
	action->num_of_integers = 10;
	action->triggers = NULL;
	action->num_of_triggers = 0;
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
	action->TWORECTOVERRIDE_TRANSITION = 2000;
}

void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action){
	printf("UIConfigure_DraggableRectOverride\n");
	action->response = MOTION;
	action->function = UIAction_DraggableRectOverride;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;
	action->integers = NULL;
	action->num_of_integers = 0;
	action->triggers = NULL;
	action->num_of_triggers = 0;
	action->external = NULL;
}

void UIConfigure_RenderLine(UI_Element *element, UI_Action *action){
	printf("UIConfigure_RenderLine\n");
	action->response = RENDER;
	action->function = UIAction_RenderLine;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;
	action->integers = malloc(sizeof(int) * 2);
	action->num_of_integers = 2;
	action->triggers = NULL;
	action->num_of_triggers = 0;

	action->integers[0] = element->rect.x;
	action->integers[1] = element->rect.y;
}

void UIConfigure_StoreMousePosition(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_companions);
	printf("UIConfigure_StoreMousePosition\n");
	action->response = MOTION;
	action->function = UIAction_StoreMousePosition;
	action->element = element;
	action->external = NULL;

	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	while(i < num_of_companions){
	action->companions[i] = va_arg(vargs,UI_Action*);
	i++;
	}
	action->num_of_companions = num_of_companions;

	action->status = 0;
	action->integers = malloc(sizeof(int) * 2);
	action->num_of_integers = 2;
	action->triggers = NULL;
	action->num_of_triggers = 0;
	va_end(vargs);
	printf("done\n");
}

void UIConfigure_CalculateSibling(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_companions);
	printf("UIConfigure_CalculateSibling\n");
	action->response = UPDATE;
	action->function = UIAction_CalculateSibling;
	action->element = element;
	action->external = NULL;

	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	while(i < num_of_companions){
	action->companions[i] = va_arg(vargs,UI_Action*);
	i++;
	}
	action->num_of_companions = num_of_companions;

	action->status = 0;
	action->integers = malloc(sizeof(int) * 2);
	action->num_of_integers = 2;
	action->triggers = NULL;
	action->num_of_triggers = 0;
	va_end(vargs);
	printf("done\n");
}

void UIConfigure_LeftClickAnywhere(UI_Element *element, UI_Action *action){
	printf("UIConfigure_LeftClickRect\n");
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickAnywhere;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;

	action->integers = NULL;
	action->num_of_integers = 0;

	action->triggers = NULL;
	action->num_of_triggers = 0;
}

void UIConfigure_RightClickAnywhere(UI_Element *element, UI_Action *action){
	printf("UIConfigure_RightClickRect\n");
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickAnywhere;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;

	action->integers = NULL;
	action->num_of_integers = 0;

	action->triggers = NULL;
	action->num_of_triggers = 0;
}

void UIConfigure_LeftReleaseAnywhere(UI_Element *element, UI_Action *action){
	printf("UIConfigure_LeftReleaseAnywhere\n");
	action->response = LEFT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;

	action->integers = NULL;
	action->num_of_integers = 0;

	action->triggers = NULL;
	action->num_of_triggers = 0;
}

void UIConfigure_RightReleaseAnywhere(UI_Element *element, UI_Action *action){
	printf("UIConfigure_RightReleaseAnywhere\n");
	action->response = RIGHT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;

	action->integers = NULL;
	action->num_of_integers = 0;

	action->triggers = NULL;
	action->num_of_triggers = 0;

}

void UIConfigure_Counter(UI_Element *element, UI_Action *action){
	printf("UIConfigure_Counter\n");
	action->response = UPDATE;
	action->function = UIAction_Counter;
	action->element = element;
	action->external = NULL;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->status = 0;
	action->integers = NULL;
	action->num_of_integers = 0;
	action->triggers = NULL;
	action->num_of_triggers = 0;
}


static void UITrigger_Execute(UI_Trigger *trigger){
	if(trigger->action == NULL){
		return;
	}
	if(trigger->status_from == -1 || trigger->action->status == trigger->status_from){
		printf("executing trigger\n");
		if(trigger->status_to == UITRIGGER_PLUSONE){
			trigger->action->status++;
		}
		else{
			trigger->action->status = trigger->status_to;
		}
	}
}

void UIElement_Free(UI_Element *element){
	int i = 0;
	if(element->parent != NULL){
		UIElement_DeExternalise(element);
		UIElement_Deparent(element);
	}
	while(i < element->num_of_actions){
	  freeUIAction(&element->actions[i]);
	  i++;
	}
	free(element->actions);
	printf("freeing element\n");
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
		printf("stepped through %d elements to fit this\n",testing);
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
	UI_Element *child = element->parent->child;
	int i = 0;
	while(child != NULL){
		i = 0;
		while(i < child->num_of_actions){
			if(child->actions[i].external == element){
				child->actions[i].external = NULL;
			}
			i++;
		}
		child = child->sibling;
	}
}

static void getCenterOfRect(SDL_Rect *rect, int *x, int *y){
	*x = rect->x + rect->w/2;
	*y = rect->y + rect->h/2;
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
	UI_Trigger trigger;
	trigger.action = target;
	trigger.status_from = status_from;
	trigger.status_to = status_to;
	action->num_of_triggers++;
	action->triggers = realloc(action->triggers, sizeof(UI_Trigger) * action->num_of_triggers);
	action->triggers[action->num_of_triggers-1] = trigger;
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
}
