#include "AI.h"

#define UITRIGGER_PLUSONE -2

enum Response {NONE,
			         RENDER_BASE,										/* 1 */
               RENDER,												/* 2 */
               LEFT_CLICK,										/* 3 */
		      	   RIGHT_CLICK,										/* 4 */
			         LEFT_RELEASE,									/* 5 */
			         RIGHT_RELEASE,									/* 6 */
               MOTION,												/* 7 */
               UPDATE,												/* 8 */
               INTERNAL,											/* 9 */
               EXTERNAL,											/* 10 */
							 GAME_OBJECT_UPDATE,						/* 11 */
							 RESPONSE_PAUSE,
               FREED};												/* 13 */

enum LineOrigins{CENTER,BL_CORNER,BR_CORNER,TL_CORNER,TR_CORNER};

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
	SDL_Texture *texture;
};

struct UI_Trigger{
  UI_Action *action;
  UI_Trigger *next;
  int status_from;
  int status_to;
};

int UIAction_External(UI_Action *action, va_list copy_from);
int UIAction_DisplayNumber(UI_Action *action, va_list copy_from);
int UIAction_DisplayString(UI_Action *action, va_list copy_from);
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
int UIAction_ResourceCounter(UI_Action *action, va_list copy_from);
int UIAction_Auto(UI_Action *action, va_list copy_from);


void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b);
void UIConfigure_LeftClickRect(UI_Element *element, UI_Action *action);
void UIConfigure_RightClickRect(UI_Element *element, UI_Action *action);
void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action,
                                 int sx, int sy, int sw, int sh,
                                 int bx, int by, int bw, int bh, int delay,
                                 int small_triggers, int big_triggers, ...);
void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_RenderLine(UI_Element *element, UI_Action *action, enum LineOrigins origin, UI_Element *external);
void UIConfigure_StoreMousePosition(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_CalculateSibling(UI_Element *element, UI_Action *action, int num_of_companion, ...);
void UIConfigure_LeftClickAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_RightClickAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_LeftReleaseAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_RightReleaseAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_GenerateAIString(UI_Element *element, UI_Action *action, char *string, UI_Action *linkPrimary, UI_Action *linkSecondary);
void UIConfigure_Counter(UI_Element *element, UI_Action *action);
void UIConfigure_ResourceCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_DisplayString(UI_Element *element, UI_Action *action, char *string, int font);
void UIConfigure_DisplayNumber(UI_Element *element, UI_Action *action, int number, int font);
void UIConfigure_ShrinkFitToParent(UI_Element *element, UI_Action *action);
void UIConfigure_External(UI_Element *element, UI_Action *action, UI_Element *external);
void UIConfigure_Auto(UI_Element *element, UI_Action *action, enum Response response);
void UIConfigure_DisplayImage(UI_Element *element, UI_Action *action, SDL_Texture *image);


void UITrigger_Bind(UI_Action *action, UI_Action *target, int state_from, int state_to);

void UIElement_Free(UI_Element *element);
void UIElement_Reparent(UI_Element *element, UI_Element *parent);
void UIElement_Deparent(UI_Element *element);
void UIElement_DeExternalise(UI_Element *element);
void UIElement_Execute(UI_Element *element, enum Response response, va_list copy_from);
UI_Element *UIElement_Create(int x, int y, int w, int h, int num_of_actions);
UI_Element *UIElement_CreateByPercentage(float rx, float ry, float rw, float rh, int x, int y, int num_of_actions);

void UIRoot_Execute(UIData *uiData, enum Response response, ...);
void UIRoot_Destroy(UIData *uiData);
