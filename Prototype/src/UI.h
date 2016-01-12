#include "AI.h"

#define UITRIGGER_PLUSONE -2

enum Response {NONE = 0,										/* 2 */
               LEFT_CLICK,										/* 3 */
		           RIGHT_CLICK,										/* 4 */
			         LEFT_RELEASE,									/* 5 */
			         RIGHT_RELEASE,									/* 6 */
               MOTION,												/* 7 */
               UPDATE,									/* 11 */
			         RESPONSE_PAUSE,
			         WINDOW_RESIZE,
			         RESPONSE_DELETE,
			         DISPOSAL,
				       MOUSEWHEEL,
               GAME_OVER,
               FREED,
               OBJECT_DISPLAY,
               DEBUG};

enum UIDataTypes{UI_NULL,
				 UI_INT,
				 UI_ACTION_POINTER,
				 UI_STRING};

enum LineOrigins{CENTER,BL_CORNER,BR_CORNER,TL_CORNER,TR_CORNER};

typedef struct UI_Element UI_Element;
typedef struct UI_Action UI_Action;
typedef struct UI_Trigger UI_Trigger;
typedef struct UIData UIData;
typedef int(UI_ActionFunction)(UI_Action *action, UIData *uiData);
typedef enum UIDataTypes UIDataTypes;
typedef enum UIElement_Variety UIElement_Variety;
typedef struct UI_Hook *hooks;

struct UIData{
  UI_Element *root;
	GraphicsData *graphicsData;
	GameObjectData *gameObjectData;
	AIData *aiData;
	AnnouncementsData *announcementsData;
	GameOverData *gameOverData;
	ObjectInfoDisplay *objectInfoDisplay;
	SDL_Event *event;
	int *ticks;
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

enum UIString_Align{
	UISTRING_ALIGN_LEFT,
	UISTRING_ALIGN_CENTER,
	UISTRING_ALIGN_RIGHT
};

enum UIElement_Variety{
  BLOCK,
  STOPBOX,
  COMPILEBOX,
	CROSSBOX,
	SCROLLHANDLE,
	FILLRECT
};

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b);
void UIConfigure_LeftClickRect(UI_Element *element, UI_Action *action);
void UIConfigure_RightClickRect(UI_Element *element, UI_Action *action);
void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action,
                                 int sx, int sy, int sw, int sh,
                                 int bx, int by, int bw, int bh, int delay);
void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_RenderLine(UI_Element *element, UI_Action *action, enum LineOrigins origin, UI_Element *external);
void UIConfigure_StoreMousePosition(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_CalculateSibling(UI_Element *element, UI_Action *action, int num_of_companion, ...);
void UIConfigure_LeftClickAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_RightClickAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_LeftReleaseAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_RightReleaseAnywhere(UI_Element *element, UI_Action *action);
void UIConfigure_Counter(UI_Element *element, UI_Action *action, enum Response response);
void UIConfigure_ResourceCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_DaysCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_YearsCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_DisplayString(UI_Element *element, UI_Action *action, char *string, int font, enum UIString_Align align);
void UIConfigure_DisplayNumber(UI_Element *element, UI_Action *action, int number, int font, enum UIString_Align align);
void UIConfigure_ShrinkFitToParent(UI_Element *element, UI_Action *action);
void UIConfigure_Auto(UI_Element *element, UI_Action *action, enum Response response);
void UIConfigure_DisplayImage(UI_Element *element, UI_Action *action, SDL_Texture *image);
void UIConfigure_AddAiBlock(UI_Element *element, UI_Action *action, BlockFunctionTemplate *template, UI_Element *target);
void UIConfigure_DeleteKeyFlagDestroy(UI_Element *element, UI_Action *action);
void UIConfigure_ReadAiBlocks(UI_Element *element, UI_Action *action, UI_Action *infoOutput);
void UIConfigure_SetUpAiBlock(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_NullifyAI(UI_Element *element, UI_Action *action);
void UIConfigure_RecallWorkers(UI_Element *element, UI_Action *action);
void UIConfigure_DraggableVerticalOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...);
void UIConfigure_GetDifferenceInChildYOffset(UI_Element *element, UI_Action *action, float yPercOffset, int yIntOffset, UI_Element *child, int num_of_companions, ...);
void UIConfigure_ShiftChildren(UI_Element *element, UI_Action *action);
void UIConfigure_ShrinkFitToParentWithYShift(UI_Element *element, UI_Action *action, UI_Action *shiftSourceElement);
void UIConfigure_CalculateScrollListOffset(UI_Element *element, UI_Action *action, int totalOffset);
void UIConfigure_PercPosition(UI_Element *element, UI_Action *action, float xPerc, float yPerc,
	                                                                    int xFixed, int yFixed,
																																			int num_of_companions, ...);
void UIConfigure_UpdateTwoRectOverrideOnWindowResize(UI_Element *element, UI_Action *action, UI_Action *twoRectOverride, int bxip, int byip, float bxfp, float byfp,
	                                                                                                                       int bxid, int byid, float bxfd, float byfd,
                                                                                                                         int sxip, int syip, float sxfp, float syfp,
																																																											   int sxid, int syid, float sxfd, float syfd);


void UIConfigure_PercOffsetRect(UI_Element *element, UI_Action *action, float xPerc, float yPerc,
	 																																			float wPerc, float hPerc,
                                                                        int xFixed, int yFixed,
																																				int wFixed, int hFixed,
																																			  int num_of_companions, ...);
void UIConfigure_ToggleInteger(UI_Element *element, UI_Action *action, int* target);
void UIConfigure_GetAnnouncement(UI_Element *element, UI_Action *action, UI_Action *placeToPut);
void UIConfigure_GetGameOverString(UI_Element *element, UI_Action *action, UI_Action *placeToPut);
void UIConfigure_GetFinalScoreString(UI_Element *element, UI_Action *action, UI_Action *placeToPut);
void UIConfigure_GetInfoDisplayString(UI_Element *element, UI_Action *action, UI_Action *placeToPut);
void UIConfigure_GetObjectStatusString(UI_Element *element, UI_Action *action, UI_Action *placeToPut);
void UIConfigure_GetObjectStatusString(UI_Element *element, UI_Action *action, UI_Action *placeToPut);
void UIConfigure_FillAndBorderRect(UI_Element *element, UI_Action *action, int fr, int fg, int fb, int br, int bg, int bb, UIElement_Variety variety);
void UIConfigure_PassThrough(UI_Element *element, UI_Action *action, enum Response response, int num_of_companions, ...);
void UIConfigure_SlideWithMouseWheel(UI_Element *element, UI_Action *action, int x, int y, int num_of_companions,...);
void UIConfigure_Minimap(UI_Element *element, UI_Action *action);
void UIConfigure_MinimapMouseMove(UI_Element *element, UI_Action *action);

void UITrigger_Bind(UI_Action *action, UI_Action *target, int state_from, int state_to);

void UIElement_Free(UI_Element *element);
void UIElement_Reparent(UI_Element *element, UI_Element *parent);
void UIElement_Deparent(UI_Element *element);
int UIElement_Execute(UI_Element *element, UIData *uiData, enum Response response);
UI_Element *UIElement_Create(int x, int y, int w, int h, int num_of_actions);
UI_Element *UIElement_CreateByPercentage(float rx, float ry, float rw, float rh, int x, int y, int num_of_actions);

void UIRoot_Execute(UIData *uiData, enum Response response, int stopAtFirst);
void UIRoot_Destroy(UIData *uiData);
void UIRoot_Pack(UIData *uiData, GraphicsData *graphicsData);
void UIRoot_ExecuteUpwards(UIData *uiData, enum Response response, int stopAtFirst);

UI_Element *makeAIBlock(int x_offset, int y_offset, char *aiString, UI_Element *parent);
UI_Element *makeStartBlock(int x_offset, int y_offset, UI_Element *parent);
UI_Element *makeAIResetButton(int x_offset, int y_offset, UI_Element *parent);
UI_Element *makeAITemplateScrollList(int x_offset, int y_offset, AIData *aiData, UI_Element *parent, UI_Element *blockHolder);
UI_Element *makeHiveCellBlock(int x_offset, int y_offset, UI_Element *parent, HiveCell *hiveCell);
void initUIData(UIData *uiData);
void quickSetStatus(UI_Action *action, int status);
void UIElement_Debug(UI_Element *element);
