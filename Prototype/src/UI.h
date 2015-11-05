#include "AI.h"

enum ExpandableStatus {SMALL,SMALL2BIG,BIG2SMALL,BIG};
enum DraggableStatus {ROOTED,GRABBED};

typedef struct SimpleButton SimpleButton;
typedef struct UIData UIData;
typedef struct ExpandablePanel ExpandablePanel;
typedef struct DraggableBlock DraggableBlock;
typedef enum ExpandableStatus ExpandableStatus;

typedef union UI_Link UI_Link;

SimpleButton createUISimpleButton(int x, int y, int w, int h, char *message, Uint32 colour);
void updateSimpleButton(GraphicsData *graphicsData, SimpleButton *simpleButton);
int checkClickSimpleButton(int click_x, int click_y, SimpleButton *simpleButton);
void performClickSimpleButton(SimpleButton *simpleButton);

ExpandablePanel createExpandingPanel(int s_x, int s_y, int s_h, int s_w,
                                     int b_x, int b_y, int b_h, int b_w,
                                     int msToBig, int msToSmall, Uint32 colour);
void updateExpandablePanel(GraphicsData *graphicsData, ExpandablePanel *expandablePanel, float ticks);
int checkClickExpandablePanel(int click_x, int click_y, ExpandablePanel *expandablePanel);
void performClickExpandablePanel(ExpandablePanel *expandablePanel);

DraggableBlock createDraggableBlock(int x, int y, int w, int h, ExpandablePanel *parent, Uint32 colour);
void updateDraggableBlock(GraphicsData *graphicsData, DraggableBlock *draggableBlock);
void checkMouseMoveDraggableBlock(int click_x, int click_y, DraggableBlock *draggableBlock);
int checkClickDownDraggableBlock(int click_x, int click_y, DraggableBlock *draggableBlock);
int checkClickUpDraggableBlock(int click_x, int click_y, DraggableBlock *draggableBlock);

void clickDownOnUI(UIData *uiData, SDL_Event *event);
void clickUpOnUI(UIData *uiData, SDL_Event *event);
void moveMouseOnUi(UIData *uiData, SDL_Event *event);
void updateUI(UIData *uiData, GraphicsData *graphicsData, float ticks);

struct SimpleButton{
  SDL_Rect rect;
  Uint32 colour;
  char *message;
};

struct DraggableBlock{
  SDL_Rect rect;
  SDL_Rect mobile_rect;
  Uint32 colour;
  enum DraggableStatus status;
  ExpandablePanel *parent;
};

struct ExpandablePanel{
  SDL_Rect big_rect;
  SDL_Rect small_rect;
  SDL_Rect mobile_rect;
  int msToBig;
  int msToSmall;
  int msTimer;
  enum ExpandableStatus status;
  Uint32 colour;
};

struct UIData{
  /* This is a very simple way of doing it */
  int numberOfSimpleButtons;
  SimpleButton simpleButtons[1];
  int numberOfExpandablePanels;
  ExpandablePanel expandablePanels[1];
  int numberOfDraggableBlocks;
  DraggableBlock draggableBlocks[1];
  /* We will be moving to a more tree/node based way of structuring this soon. */
};

union UI_Link{
  ExpandablePanel *expandablePanel;
  SimpleButton *simpleButton;
};
