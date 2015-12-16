#include "modularUIStructs.h"

UI_Element *createBlank(void);
UI_Element *createRoot(void);
UI_Element *createUI_Button(int x, int y, int w, int h, int colour,
                            UI_Controller_Function function,UI_Element **args);
UI_Element *createUI_Expandable(int s_x, int s_y, int s_w, int s_h,
                                int b_x, int b_y, int b_w, int b_h, int colour,
                                UI_Controller_Function function, UI_Element **args);
UI_Element *createUI_Draggable(int x, int y, int w, int h, int colour,UI_Controller_Function function,UI_Element **args);
UI_Element *createUI_Panel(int x, int y, int w, int h, int colour);
UI_Element *createUI_OffsetPanel(int x, int y, int w, int h, int colour,UI_Controller_Function function,UI_Element **args);
UI_Element *addChild(UI_Element *parent, UI_Element *child);
SDL_Rect *getElementRect(UI_Element *element);
SDL_Rect getElementRectWithOffset(UI_Element *element);
SDL_Rect getTheoreticalBoundingBox(UI_Element *element, int padding);
void preventOverlap(UI_Element *element);

int blankUI_Function(UI_Element *element);

void renderUIElement(UI_Element *element, SDL_Window *window, int ticks);
void clickonUIElement(UI_Element *element, SDL_Event *event);
void cickuponUIElement(UI_Element *element, SDL_Event *event);
void motiononUIElement(UI_Element *element, SDL_Event *event);

void renderUI(UI_Element *element, SDL_Window *window, int ticks);
void clickonUI(UI_Element *root, SDL_Event *event);
void clickuponUI(UI_Element *root, SDL_Event *event);
void motiononUI(UI_Element *root, SDL_Event *event);
