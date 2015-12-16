#include "modularUIBackend2.h"


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
static void freeUITrigger(UI_Trigger *trigger);
static void UIExecuteTrigger(UI_Trigger trigger);

int UIAction_RenderLine(UI_Action *action, ...){
  va_list vargs;
  SDL_Renderer *renderer;
  va_start(vargs,action);
  renderer = va_arg(vargs, SDL_Renderer*);
  va_end(vargs);
  SDL_SetRenderDrawColor(renderer,255,
                                  255,
                                  255,
                                  255);
  SDL_RenderDrawLine(renderer,action->element->rect.x,action->element->rect.y,
                              action->integers[0],action->integers[1]);
  return 1;
}

int UIAction_CalculateLine(UI_Action *action, ...){
  va_list vargs;
  SDL_Event *event;
  va_start(vargs,action);
  event = va_arg(vargs,SDL_Event*);
  action->integers[0] = event->motion.x;
  action->integers[1] = event->motion.y;
  if(action->status == 1){
    action->companion->integers[0] = action->integers[0];
    action->companion->integers[1] = action->integers[1];
  }
  return 1;
}

int UIAction_FillRect(UI_Action *action, ...){
  /* This should be given the necessary rendering information, in this case,
     a pointer to the SDL_Window */
  SDL_Rect rect;
  va_list vargs;
  SDL_Renderer *renderer;
  va_start(vargs,action);
  renderer = va_arg(vargs,SDL_Renderer*);
  va_end(vargs);
  SDL_SetRenderDrawColor(renderer,action->RENDERRECT_R,
                                  action->RENDERRECT_G,
                                  action->RENDERRECT_B,
                                  255);
  SDL_RenderFillRect(renderer,&action->element->rect);
  return 1;
}

int UIAction_ClickRect(UI_Action *action, ...){
  /* This should get a SDL_Event pointer */
  SDL_Event *event;
  va_list vargs;
  int i = 0;
  va_start(vargs,action);
  event = va_arg(vargs,SDL_Event*);
  va_end(vargs);

  if(event->button.x < action->element->rect.x || event->button.x > action->element->rect.x + action->element->rect.w){
    return 0;
  }
  if(event->button.y < action->element->rect.y || event->button.y > action->element->rect.y + action->element->rect.h){
    return 0;
  }
  while(i < action->num_of_triggers){
    printf("executing trigger %d\n",i);
    UIExecuteTrigger(action->triggers[i]);
    i++;
  }
  printf("click\n");
  return 1;
}

int UIAction_DraggableRectOverride(UI_Action *action, ... ){
  int i = 0;
  SDL_Event *event;
  va_list vargs;
  va_start(vargs,action);
  event = va_arg(vargs,SDL_Event*);
  va_end(vargs);

  /* UIACTION_INTS is the status, 0 = rooted, 1 = grabbed */

  if(action->status == 1){
    action->element->rect.x = event->motion.x;
    action->element->rect.y = event->motion.y;
  }
  return 1;
}

int UIAction_TwoRectOverride(UI_Action *action, ...){
  int i = 0;
  va_list vargs;
  int dt;
  float transition_multiplier;
  va_start(vargs,action);
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

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b){
  printf("UIConfigure_FillRect\n");
  action->response = RENDER;
  action->function = UIAction_FillRect;
  action->element = element;
  action->external = NULL;
  action->companion = NULL;
  action->status = 1;
  action->integers = malloc(sizeof(int) * 3);
  action->num_of_integers = 3;
  action->triggers = NULL;
  action->num_of_triggers = 0;

  action->RENDERRECT_R = r;
  action->RENDERRECT_G = g;
  action->RENDERRECT_B = b;
}

void UIConfigure_ClickRect(UI_Element *element, UI_Action *action, int num_of_triggers, ...){
  va_list vargs;
  int i = 0;
  printf("UIConfigure_ClickRect\n");
  action->response = CLICK;
  action->function = UIAction_ClickRect;
  action->element = element;
  action->external = NULL;
  action->companion = NULL;
  action->status = 1;

  action->integers = NULL;
  action->num_of_integers = 0;

  action->triggers = malloc(sizeof(UI_Trigger) * num_of_triggers);
  action->num_of_triggers = num_of_triggers;

  va_start(vargs,num_of_triggers);
  while(i < num_of_triggers){
    action->triggers[i] = va_arg(vargs,UI_Trigger);
    i++;
  }
  va_end(vargs);
}

void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action, int sx, int sy, int sw, int sh, int bx, int by, int bw, int bh, int small_triggers, int big_triggers,...){
  printf("UIConfigure_TwoRectOverride\n");
  action->response = UPDATE;
  action->function = UIAction_TwoRectOverride;
  action->element = element;
  action->external = NULL;
  action->companion = NULL;
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
  action->companion = NULL;
  action->status = 0;
  action->integers = NULL;
  action->num_of_integers = 0;
  action->triggers = NULL;
  action->num_of_triggers = 0;
  action->external = NULL;
}

void UIConfigure_RenderLine(UI_Element *element, UI_Action *action){
  printf("UIConfigure_RenderLine\n");
  action->response = MOTION;
  action->function = UIAction_RenderLine;
  action->element = element;
  action->external = NULL;
  action->companion = NULL;
  action->status = 0;
  action->integers = malloc(sizeof(int) * 2);
  action->num_of_integers = 2;
  action->triggers = NULL;
  action->num_of_triggers = 0;

  action->integers[0] = element->rect.x;
  action->integers[1] = element->rect.y;
}

void UIConfigure_CalculateLine(UI_Element *element, UI_Action *action, UI_Action *companion){
  printf("UIConfigure_CalculateLine\n");
  action->response = MOTION;
  action->function = UIAction_CalculateLine;
  action->element = element;
  action->external = NULL;
  assert(companion->function == UIAction_RenderLine);
  action->companion = companion;
  action->status = 0;
  action->integers = malloc(sizeof(int) * 2);
  action->num_of_integers = 2;
  action->triggers = NULL;
  action->num_of_triggers = 0;
  printf("done\n");
}

UI_Trigger UIConfigure_Trigger(UI_Action *action, int num_of_states,...){
  UI_Trigger trigger;
  va_list vargs;
  int i = 0;

  trigger.action = action;
  trigger.states_from = calloc(num_of_states,sizeof(int));
  trigger.states_to = calloc(num_of_states,sizeof(int));
  trigger.states = num_of_states;


  va_start(vargs,num_of_states);
  while(i < num_of_states){
    trigger.states_from[i] = va_arg(vargs,int);
    trigger.states_to[i] = va_arg(vargs,int);
    i++;
  }
  va_end(vargs);
  return trigger;
}

static void UIExecuteTrigger(UI_Trigger trigger){
  int i = 0;
  while(i < trigger.states){
    if(trigger.action->status == trigger.states_from[i]){
      trigger.action->status = trigger.states_to[i];
      /* I don't like breaks much, but this is a nice way of doing this. We need
         to avoid the situation where you have one controller looping between states
         (e.g. 1->0 and 0->1) without never hitting the first state */
      break;
    }
    i++;
  }
}

void freeUIElement(UI_Element *element){
  int i = 0;
  while(i < element->num_of_actions){
    freeUIAction(&element->actions[i]);
    i++;
  }
  printf("freeing action array\n");
  free(element->actions);
  printf("freeing element\n");
  free(element);
}

static void freeUIAction(UI_Action *action){
  int i = 0;
  if(action->num_of_integers){
    printf("freeing integer array\n");
    action->num_of_integers = 0;
    free(action->integers);
  }
  if(action->num_of_triggers){
    printf("freeing triggers\n");
    while(i < action->num_of_triggers){
      freeUITrigger(&action->triggers[i]);
      i++;
    }
    free(action->triggers);
    action->num_of_triggers = 0;
  }
  return;
}

static void freeUITrigger(UI_Trigger *trigger){
  printf("freeing trigger\n");
  free(trigger->states_from);
  free(trigger->states_to);
}
