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
			   WINDOW_RESIZE,
			   RESPONSE_DELETE,
			   DISPOSAL,
			   AI_RESPONSE,
			   SOUND,
               FREED};												/* 13 */

enum UIDataTypes{UI_NULL,
				 UI_INT,
				 UI_ACTION_POINTER,
				 UI_STRING};

enum LineOrigins{CENTER,BL_CORNER,BR_CORNER,TL_CORNER,TR_CORNER};

typedef struct UI_Element UI_Element;
typedef struct UI_Action UI_Action;
typedef struct UI_Trigger UI_Trigger;
typedef struct UIData UIData;
typedef int(UI_ActionFunction)(UI_Action *action, va_list vargs);
typedef enum UIDataTypes UIDataTypes;

struct UIData{
  UI_Element *root;
};

struct UI_Element{
  int *enableFlags;
  int destroy;
  UI_Action *actions;
  int num_of_actions;
  SDL_Rect rect;
  UIDataTypes *exposed_data_types;
  int exposed_data_count;
  void **exposed_data;

  UI_Element *parent;
  UI_Element *child;
  UI_Element *sibling;
  int sibling_position;
};

struct UI_Action{
  enum Response response;
  UI_ActionFunction *function;
  UI_Element *element;
  UI_Element *external;
  SDL_Texture *texture;
  int status;
  int new_status;
  int num_of_companions;
  int num_of_integers;
  int num_of_triggers;
  int num_of_floats;
  int num_of_strings;
  int *integers;
  UI_Trigger *triggers;
  float *floats;
  char **strings;
  UI_Action **companions;
	void *extra;
};
/* These are stored as a linked list! */
struct UI_Trigger{
  UI_Action *action;
  UI_Trigger *next;
  int status_from;
  int status_to;
};

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
void UIConfigure_Counter(UI_Element *element, UI_Action *action);
void UIConfigure_ResourceCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_DisplayString(UI_Element *element, UI_Action *action, char *string, int font);
void UIConfigure_DisplayNumber(UI_Element *element, UI_Action *action, int number, int font);
void UIConfigure_ShrinkFitToParent(UI_Element *element, UI_Action *action);
void UIConfigure_External(UI_Element *element, UI_Action *action, UI_Element *external);
void UIConfigure_Auto(UI_Element *element, UI_Action *action, enum Response response);
void UIConfigure_DisplayImage(UI_Element *element, UI_Action *action, SDL_Texture *image);
void UIConfigure_InverseRect(UI_Element *element, UI_Action *action, int from_left, int from_top, int from_right, int from_bot, int num_of_companions, ...);
void UIConfigure_PercRect(UI_Element *element, UI_Action *action, float from_left, float from_right, float width, float height);
void UIConfigure_AddAiBlock(UI_Element *element, UI_Action *action, BlockFunctionTemplate *template, UI_Element *target);
void UIConfigure_DeleteKeyFlagDestroy(UI_Element *element, UI_Action *action);
void UIConfigure_ReadAiBlocks(UI_Element *element, UI_Action *action);
void UIConfigure_SetUpAiBlock(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_NullifyAI(UI_Element *element, UI_Action *action);
void UIConfigure_RecallWorkers(UI_Element *element, UI_Action *action);
void UIConfigure_DraggableVerticalOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_GetDifferenceInChildYOffset(UI_Element *element, UI_Action *action, float initial_perc, int initial_offset, UI_Element *child, int num_of_companions, ...);
void UIConfigure_ShiftChildren(UI_Element *element, UI_Action *action);
void UIConfigure_ShrinkFitToParentWithYShift(UI_Element *element, UI_Action *action, UI_Action *shiftSource);
void UIConfigure_CalculateScrollListOffset(UI_Element *element, UI_Action *action, int total_possible_offset);
void UIConfigure_InversePosition(UI_Element *element, UI_Action *action, int x_offset, int y_offset);
void UIConfigure_PercPosition(UI_Element *element, UI_Action *action, float x_offset, float y_offset, int xi_offset, int yi_offset, int num_of_companions, ...);
void UIConfigure_PercPositionV(UI_Element *element, UI_Action *action, float x_offset, float y_offset, int xi_offset, int yi_offset, int num_of_companions, ...);
void UIConfigure_UpdateTwoRectOverrideOnWindowResize(UI_Element *element, UI_Action *action, UI_Action *twoRectOverride, int bxip, int byip, float bxfp, float byfp,
	                                                                                                                      int bxid, int byid, float bxfd, float byfd,
                                                                                                                        int sxip, int syip, float sxfp, float syfp,
																																																											  int sxid, int syid, float sxfd, float syfd);

void UIConfigure_PercOffsetRect(UI_Element *element, UI_Action *action, float xfp, float yfp, float xfd, float yfd, int xip, int yip, int xid, int yid, int num_of_companions,...);

void UIConfigure_MuteSound(UI_Element *element, UI_Action *action);
void UIConfigure_MuteSoundFX(UI_Element *element, UI_Action *action);

void UITrigger_Bind(UI_Action *action, UI_Action *target, int state_from, int state_to);

void UIElement_Free(UI_Element *element);
void UIElement_Reparent(UI_Element *element, UI_Element *parent);
void UIElement_Deparent(UI_Element *element);
int UIElement_Execute(UI_Element *element, enum Response response, va_list copy_from);
UI_Element *UIElement_Create(int x, int y, int w, int h, int num_of_actions);
UI_Element *UIElement_CreateByPercentage(float rx, float ry, float rw, float rh, int x, int y, int num_of_actions);

void UIRoot_Execute(UIData *uiData, enum Response response, int stopAtFirst, ...);
void UIRoot_Destroy(UIData *uiData);
void UIRoot_Pack(UIData *uiData, GraphicsData *graphicsData);
void UIRoot_ExecuteUpwards(UIData *uiData, enum Response response, int stopAtFirst, ...);

UI_Element *makeAIBlock(int x_offset, int y_offset, char *aiString, UI_Element *parent);
UI_Element *makeStartBlock(int x_offset, int y_offset, UI_Element *parent);
UI_Element *makeAIResetButton(int x_offset, int y_offset, UI_Element *parent);
UI_Element *makeAITemplateScrollList(int x_offset, int y_offset, AIData *aiData, UI_Element *parent, UI_Element *blockHolder);
