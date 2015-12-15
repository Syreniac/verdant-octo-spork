#include "AI.h"

enum Response {NONE,
			         RENDER_BASE,
               RENDER,
               LEFT_CLICK,
		      	   RIGHT_CLICK,
			         LEFT_RELEASE,
			         RIGHT_RELEASE,
               MOTION,
               UPDATE,
               INTERNAL,
               EXTERNAL,
               FREED};

#define UITRIGGER_PLUSONE -2

typedef struct UI_Element UI_Element;
typedef struct UI_Action UI_Action;
typedef struct UI_Trigger UI_Trigger;
typedef struct UIData UIData;
typedef int(UI_ActionFunction)(UI_Action *action, va_list vargs);

struct UIData{
  UI_Element *root;
};

struct UI_Element{
  int *enableFlags;
  UI_Action *actions;
  int num_of_actions;
  SDL_Rect rect;
  UI_Element *parent;
  UI_Element *child;
  UI_Element *sibling;
};

struct UI_Action{
  enum Response response;
  UI_ActionFunction *function;
  UI_Element *element;
  UI_Element *external;
  UI_Action **companions;
  int num_of_companions;
  int status;
  int *integers;
  int num_of_integers;
  UI_Trigger *triggers;
  int num_of_triggers;
  char **strings;
  int num_of_strings;
};

struct UI_Trigger{
  UI_Action *action;
  int status_from;
  int status_to;
};

int UIAction_Counter(UI_Action *action, va_list copy_from);
int UIAction_RenderSquare(UI_Action *action, va_list copy_from);
int UIAction_FillRect(UI_Action *action, va_list copy_from);
int UIAction_TwoRectOverride(UI_Action *action, va_list copy_from);
int UIAction_ToggleActionStatus(UI_Action *action, va_list copy_from);
int UIAction_DraggableRectOverride(UI_Action *action, va_list copy_from);
int UIAction_RenderLine(UI_Action *action, va_list copy_from);
int UIAction_CalculateSibling(UI_Action *action, va_list copy_from);
int UIAction_ClickAnywhere(UI_Action *action, va_list copy_from);
int UIAction_ClickRect(UI_Action *action, va_list copy_from);

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b);
void UIConfigure_LeftClickRect(UI_Element *element, UI_Action *action);
void UIConfigure_RightClickRect(UI_Element *element, UI_Action *action);
void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action,
                                 int sx, int sy, int sw, int sh,
                                 int bx, int by, int bw, int bh,
                                 int small_triggers, int big_triggers, ...);
void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action);
void UIConfigure_RenderLine(UI_Element *element, UI_Action *action);
void UIConfigure_StoreMousePosition(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_CalculateSibling(UI_Element *element, UI_Action *action, int num_of_companion, ...);
void UIConfigure_LeftClickAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_RightClickAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_LeftReleaseAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_RightReleaseAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_GenerateAIString(UI_Element *element, UI_Action *action, char *string, UI_Action *linkPrimary, UI_Action *linkSecondary);
void UIConfigure_Counter(UI_Element *element, UI_Action *action);

void UITrigger_Bind(UI_Action *action, UI_Action *target, int state_from, int state_to);

void UIElement_Free(UI_Element *element);
void UIElement_Reparent(UI_Element *element, UI_Element *parent);
void UIElement_Deparent(UI_Element *element);
void UIElement_DeExternalise(UI_Element *element);
void UIElement_Execute(UI_Element *element, enum Response response, va_list copy_from);

void UIRoot_Execute(UIData *uiData, enum Response response, ...);
