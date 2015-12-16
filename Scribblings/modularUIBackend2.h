#include "modularUIStructs2.h"


int UIAction_RenderSquare(UI_Action *action, ...);
int UIAction_FillRect(UI_Action *action, ...);
int UIAction_TwoRectOverride(UI_Action *action,...);
int UIAction_ToggleActionStatus(UI_Action *action,...);
int UIAction_DraggableRectOverride(UI_Action *action,...);
int UIAction_RenderLine(UI_Action *action,...);

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b);
void UIConfigure_ClickRect(UI_Element *element, UI_Action *action, int num_of_triggers, ...);
void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action,
                                 int sx, int sy, int sw, int sh,
                                 int bx, int by, int bw, int bh,
                                 int small_triggers, int big_triggers, ...);
void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action);
void UIConfigure_RenderLine(UI_Element *element, UI_Action *action);
void UIConfigure_CalculateLine(UI_Element *element, UI_Action *action, UI_Action *companion);

UI_Trigger UIConfigure_Trigger(UI_Action *action, int num_of_states,...);

void freeUIElement(UI_Element *element);
