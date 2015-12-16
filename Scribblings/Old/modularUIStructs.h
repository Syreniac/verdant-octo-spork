#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum UI_Element_Type{ROOT,
                     PANEL,
                     EXPANDABLE,
					 DRAGGABLE};

enum Response{NONE,
              CLICK_DOWN,
			  CLICK_UP,
			  CLICK_UP_OR_DOWN,
			  MOUSEMOTION,
			  MOUSEMOTION_OR_CLICK_DOWN,
              EXPANSION};

enum Visibility{VISIBLE,
                INVISIBLE,
                TEMPORARILY_INVISIBLE_A,
                TEMPORARILY_INVISIBLE_B,
                FIXED_INVISIBILITY};

enum Tangibility{TANGIBLE,
                 INTANGIBLE,
                 TEMPORARILY_INTANGIBLE_A,
                 TEMPORARILY_INTANGIBLE_B,
                 FIXED_INTANGIBILITY};
                 /* Temporarily intangibles are used to make things stop
                    responding whilst they are updating, whilst knowing
                    what they need to go back to */

enum ExpandableStatus{SMALL,
                      SMALL2BIG,
                      BIG2SMALL,
                      BIG};
				
enum DraggableStatus{ROOTED,
                     GRABBED};

typedef union UI_Element UI_Element;
typedef int(*UI_Controller_Function)(UI_Element *element);

struct UI_Controller{
  UI_Controller_Function function;
  UI_Element **elements;
};

struct UI_Generic{
  enum UI_Element_Type type;
  UI_Element *parent;
  UI_Element *child;
  UI_Element *sibling;
  struct UI_Controller controller;
  enum Response response;
  enum Visibility visibility;
  enum Tangibility tangibility;
  int rendered;
  char name[30];
};

struct UI_Panel{
  struct UI_Generic generic;
  SDL_Rect rect;
  int x_offset;
  int y_offset;
  int colour;
};

struct UI_Expandable{
  struct UI_Generic generic;
  SDL_Rect rect;
  SDL_Rect small_rect;
  SDL_Rect big_rect;
  float msTimer;
  float msToBig;	
  float msToSmall;
  enum ExpandableStatus status;
  int colour;
};

struct UI_Draggable{
	struct UI_Generic generic;
	SDL_Rect rect;
	enum DraggableStatus status;
	int colour;
};

union UI_Element{
  enum UI_Element_Type type;
  struct UI_Generic generic;
  struct UI_Expandable expandable;
  struct UI_Panel panel;
  struct UI_Draggable draggable;
};
