#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum UI_Element_Type {GENERIC=0,
                      BUTTON,
                      PANEL,
                      DRAGGABLE,
                      EXPANDABLE};

enum DraggableStatus {ROOTED,
                      GRABBED};

enum Visibility {SHOWN,
                 HIDDEN};

enum Tangiblity {INTANGIBLE,
               TANGIBLE};

enum ExpandingStatus {SMALL,
                      BIG,
                      SMALL2BIG,
                      BIG2SMALL};

struct Generic{
  enum UI_Element_Type type;
  union UI_Element *parent;
  union UI_Element *sibling;
  union UI_Element *child;
};

struct Button{
  enum UI_Element_Type type;
  union UI_Element *parent;
  union UI_Element *sibling;
  union UI_Element *child;
  SDL_Rect rect;
  enum Visibility visibility;
  enum Tangiblity tangibility;
};

struct Panel{
  enum UI_Element_Type type;
  union UI_Element *parent;
  union UI_Element *sibling;
  union UI_Element *child;
  SDL_Rect rect;
  enum Visibility visibility;
};

struct ExpandingPanel{
  enum UI_Element_Type type;
  union UI_Element *parent;
  union UI_Element *sibling;
  union UI_Element *child;
  int small_x;
  int small_y;
  int small_w;
  int small_h;
  int big_x;
  int big_y;
  int big_w;
  int big_h;
  enum ExpandingStatus status;
  enum Visibility visibility;
  enum Tangiblity tangiblity;
};

struct Draggable{
  enum UI_Element_Type type;
  union UI_Element *parent;
  union UI_Element *sibling;
  union UI_Element *child;
  SDL_Rect rect;
  enum DraggableStatus status;
  enum Visibility visibility;
  enum Tangiblity tangibility;
};

union UI_Element{
  enum UI_Element_Type type;
  struct Generic generic;
  struct Button button;
  struct Panel panel;
  struct Draggable draggable;
  struct ExpandingPanel expandable;
};

typedef union UI_Element UI_Element;
typedef struct Button Button;
typedef struct Panel Panel;
typedef struct Draggable Draggable;
typedef struct ExpandingPanel Expandable;

void printUIElement(UI_Element *element);
UI_Element *createButton(int x, int y, int w, int h);
UI_Element *createDraggable(int x, int y, int w, int h);
UI_Element *createPanel(int x, int y, int w, int h);
UI_Element *createRoot(void);
UI_Element *addChild(UI_Element *parent, UI_Element *new_child);
int getNumOfChildren(UI_Element *toCount);
void renderUIElement(UI_Element *element, SDL_Window *window);
void renderUI(UI_Element *root, SDL_Window *window);
int clickOnUIElement(UI_Element *element, int click_x, int click_y);
void setUIElementVisibility(UI_Element *element, enum Visibility visible, int propagate);
void clickOnUI(UI_Element *root, SDL_Event event);
void clickupOnUI(UI_Element *root, SDL_Event event);
void destroyUIElement(UI_Element *element);
enum Visibility getUIElementVisibility(UI_Element *element);
int goSDL(void);
