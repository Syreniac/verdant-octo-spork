#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <stdarg.h>
enum Response {NONE,
               RENDER,
               CLICK,
               MOTION,
               UPDATE,
               INTERNAL,
               EXTERNAL,
               FREED};

typedef struct UI_Element UI_Element;
typedef struct UI_Action UI_Action;
typedef struct UI_Trigger UI_Trigger;
typedef int(UI_ActionFunction)(UI_Action *action, ...);

struct UI_Element{
  int *enableFlags;
  UI_Action *actions;
  int num_of_actions;
  SDL_Rect rect;
};

struct UI_Action{
  enum Response response;
  UI_ActionFunction *function;
  UI_Element *element;
  UI_Element *external;
  UI_Action *companion;
  int status;
  int *integers;
  int num_of_integers;
  UI_Trigger *triggers;
  int num_of_triggers;
};

struct UI_Trigger{
  UI_Action *action;
  int *states_from;
  int *states_to;
  int states;
};
