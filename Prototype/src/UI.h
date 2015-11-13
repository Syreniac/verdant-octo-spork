#include "AI.h"

enum UI_Elements{GENERIC,
                 PANEL,
                 CLICKABLE,
                 EXPANDABLE,
                 DRAGGABLE};

enum ExpandableStatus {SMALL,SMALL2BIG,BIG2SMALL,BIG};
enum DraggableStatus {ROOTED,GRABBED};

typedef struct Clickable Clickable;
typedef struct Expandable Expandable;
typedef struct Draggable Draggable;
typedef struct Panel Panel;
typedef struct Generic Generic;
typedef enum ExpandableStatus ExpandableStatus;
typedef struct UIData UIData;

typedef union UI_Element UI_Element;

struct Generic{
  enum UI_Elements type;
};

struct Panel{
  enum UI_Elements type;
  SDL_Rect rect;
  /*Uint32 colour;*/
  int r;
  int g;
  int b;
};

struct Clickable{
  enum UI_Elements type;
  SDL_Rect rect;
  /*Uint32 colour;*/
  int r;
  int g;
  int b;
  char *message;
  UI_Element *parent;
};

struct Draggable{
  enum UI_Elements type;
  SDL_Rect rect;
  /*Uint32 colour;*/
  int r;
  int g;
  int b;
  enum DraggableStatus status;
  UI_Element *parent;
};

struct Expandable{
  enum UI_Elements type;
  SDL_Rect big_rect;
  int r;
  int g;
  int b;
  SDL_Rect small_rect;
  SDL_Rect rect;
  int msToBig;
  int msToSmall;
  int msTimer;
  enum ExpandableStatus status;
 /* Uint32 colour;*/

};

union UI_Element{
  enum UI_Elements type;
  Generic generic;
  Panel panel;
  Expandable expandable;
  Clickable clickable;
  Draggable draggable;
};

struct UIData{
  /* This is a very simple way of doing it */
  int numberOfUIElements;
  UI_Element UIElements[255];
};

UI_Element createUI_Clickable(SDL_Rect rect, char *message, int r, int g, int b);
UI_Element createUI_Expandable(SDL_Rect s_rect, SDL_Rect b_rect,
                               int msToBig, int msToSmall, int r, int g, int b);
UI_Element createUI_Draggable(SDL_Rect rect, UI_Element *parent, int r, int g, int b);
UI_Element createUI_Panel(SDL_Rect rect, int r, int g, int b);

SDL_Rect *getUIElementRect(UI_Element *element);

void renderUIElement(GraphicsData *graphicsData,UI_Element *element);
void clickOnUIElement(UI_Element *element, SDL_Event *event);
void clickupOnUIElement(UI_Element *element, SDL_Event *event);
void mousemoveOnUIElement(UI_Element *element, SDL_Event *event);

void clickOnUI(UIData *uiData, SDL_Event *event);
void clickupOnUI(UIData *uiData, SDL_Event *event);
void moveMouseOnUi(UIData *uiData, SDL_Event *event);
void renderUI(UIData *uiData, GraphicsData *graphicsData);
