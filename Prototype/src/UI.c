#include "UI.h"

#define SHOW_SUBRECTS 1

static int countUIActionsNeededForAITemplate(BlockFunctionTemplate *template);

static void UIAction_Init(UI_Element *element, UI_Action *action);
static void UIAction_Free(UI_Action *action);
static void UITrigger_Execute(UI_Action *action);
static int isLinked(UI_Element *test, UI_Element *to);
static int UIElement_isVisible(UI_Element *element);
static void UIElement_DeExternalise(UI_Element *root, UI_Element *external);
static void UITrigger_Destroy(UI_Action *action, UI_Trigger *trigger);
static void UIElement_RemoveExposedData(UI_Element *element);

static void sUIAction_Minimap_DrawGameObject(UI_Action *action, GraphicsData *graphicsData, SDL_Rect *rect);
//static void sUIAction_DisplayNumber_EditNumber(UI_Action *action, int editNumber);
static void sUIAction_DisplayString_EditString(UI_Action *action, char *editString);

int UIAction_PercentageFillRect(UI_Action *action, UIData *uiData);
int UIAction_External(UI_Action *action, UIData *uiData);
int UIAction_DisplayNumber(UI_Action *action, UIData *uiData);
int UIAction_DisplayString(UI_Action *action, UIData *uiData);
int UIAction_Counter(UI_Action *action, UIData *uiData);
int UIAction_RenderSquare(UI_Action *action, UIData *uiData);
int UIAction_FillRect(UI_Action *action, UIData *uiData);
int UIAction_TwoRectOverride(UI_Action *action, UIData *uiData);
int UIAction_ToggleActionStatus(UI_Action *action, UIData *uiData);
int UIAction_DraggableRectOverride(UI_Action *action, UIData *uiData);
int UIAction_RenderLine(UI_Action *action, UIData *uiData);
int UIAction_CalculateSibling(UI_Action *action, UIData *uiData);
int UIAction_ClickAnywhere(UI_Action *action, UIData *uiData);
int UIAction_ClickRect(UI_Action *action, UIData *uiData);
int UIAction_ResourceCounter(UI_Action *action, UIData *uiData);
int UIAction_DaysCounter(UI_Action *action, UIData *uiData);
int UIAction_YearsCounter(UI_Action *action, UIData *uiData);
int UIAction_Auto(UI_Action *action, UIData *uiData);
int UIAction_NullifyAI(UI_Action *action, UIData *uiData);
int UIAction_GetAnnouncement(UI_Action *action, UIData *uiData);
int UIAction_PassThrough(UI_Action *action, UIData *uiData);
int UIAction_SlideWithMouseWheel(UI_Action *action, UIData *uiData);
int UIAction_Minimap(UI_Action *action, UIData *uiData);
int UIAction_MinimapMouseMove(UI_Action *action, UIData *uiData);
int UIAction_ToggleInteger(UI_Action *action, UIData *uiData);


static UI_Action* makeAIBlockOperatorSelector(UI_Element *element, int *nextAvailableActionPointer, int x, int y){
	int nextAvailableAction = *nextAvailableActionPointer;
	UIConfigure_LeftClickSubrect(element, &element->actions[nextAvailableAction], x, y, 25, 25);
		UITrigger_Bind(&element->actions[nextAvailableAction],&element->actions[nextAvailableAction+3],-1,UITRIGGER_PLUSONE);
	UIConfigure_FillAndBorderSubrect(element, &element->actions[nextAvailableAction+1],x,y,25,25,255,255,255,0,0,0);
	UIConfigure_DisplayStringSubrect(element, &element->actions[nextAvailableAction+2],"",1,x,y,25,25);
	UIConfigure_StringCollection(element,&element->actions[nextAvailableAction+3],1,3,&element->actions[nextAvailableAction+2],"+","-","=");
	*nextAvailableActionPointer = *nextAvailableActionPointer + 4;
	return &element->actions[nextAvailableAction+2];
}


static UI_Action* makeAIBlockComparatorSelector(UI_Element *element, int *nextAvailableActionPointer, int x, int y){
	int nextAvailableAction = *nextAvailableActionPointer;
	UIConfigure_LeftClickSubrect(element, &element->actions[nextAvailableAction], x, y, 25, 25);
		UITrigger_Bind(&element->actions[nextAvailableAction],&element->actions[nextAvailableAction+3],-1,UITRIGGER_PLUSONE);
	UIConfigure_FillAndBorderSubrect(element, &element->actions[nextAvailableAction+1],x,y,25,25,255,255,255,0,0,0);
	UIConfigure_DisplayStringSubrect(element, &element->actions[nextAvailableAction+2],"",1,x,y,25,25);
	UIConfigure_StringCollection(element,&element->actions[nextAvailableAction+3],1,3,&element->actions[nextAvailableAction+2],"=","<",">");
	*nextAvailableActionPointer = *nextAvailableActionPointer + 4;
	return &element->actions[nextAvailableAction+2];
}

static UI_Action* makeAIBlockIntegerSelector(UI_Element *element, int *nextAvailableActionPointer, int x, int y){
		int nextAvailableAction = *nextAvailableActionPointer;
		UIConfigure_LeftClickSubrect(element, &element->actions[nextAvailableAction], x, y, 25, 25);
			UITrigger_Bind(&element->actions[nextAvailableAction],&element->actions[nextAvailableAction+3],-1,UITRIGGER_PLUSONE);
		UIConfigure_FillAndBorderSubrect(element, &element->actions[nextAvailableAction+1],x,y,25,25,255,255,255,0,0,0);
		UIConfigure_DisplayNumberSubrect(element, &element->actions[nextAvailableAction+2],0,1,x,y,25,25);
		UIConfigure_Counter(element,&element->actions[nextAvailableAction+3],99,1,&element->actions[nextAvailableAction+2]);
		*nextAvailableActionPointer = *nextAvailableActionPointer + 4;
		return &element->actions[nextAvailableAction+2];
}

static UI_Action* makeAIBlockLineLinker(UI_Element *element, int *nextAvailableActionPointer, int x, int y, char *label, enum LineOrigins lineorigin){
	int nextAvailableAction = *nextAvailableActionPointer;
	UIConfigure_StoreMousePosition(element, &element->actions[nextAvailableAction],2,&element->actions[nextAvailableAction+3],&element->actions[nextAvailableAction+2]);
		element->actions[nextAvailableAction].status = 0;
		element->actions[nextAvailableAction].new_status = 0;
	UIConfigure_LeftClickSubrect(element, &element->actions[nextAvailableAction+1], x, y, 35, 25);
		element->actions[nextAvailableAction+1].status = 1;
		element->actions[nextAvailableAction+1].new_status = 1;
		UITrigger_Bind(&element->actions[nextAvailableAction+1],&element->actions[nextAvailableAction+3],-1,1);
		UITrigger_Bind(&element->actions[nextAvailableAction+1],&element->actions[nextAvailableAction+1],1,0);
		UITrigger_Bind(&element->actions[nextAvailableAction+1],&element->actions[nextAvailableAction],0,1);
		UITrigger_Bind(&element->actions[nextAvailableAction+1],&element->actions[nextAvailableAction+4],0,1);
		UIConfigure_CalculateSibling(element, &element->actions[nextAvailableAction+2],1,&element->actions[nextAvailableAction+3]);
			element->actions[nextAvailableAction+2].status = 0;
			element->actions[nextAvailableAction+2].new_status = 0;
	UIConfigure_RenderLine(element, &element->actions[nextAvailableAction+3],lineorigin,NULL);
	UIConfigure_LeftReleaseAnywhere(element, &element->actions[nextAvailableAction+4]);
		element->actions[nextAvailableAction+4].status = 0;
		element->actions[nextAvailableAction+4].new_status = 0;
		UITrigger_Bind(&element->actions[nextAvailableAction+4],&element->actions[nextAvailableAction+3],1,2);
		UITrigger_Bind(&element->actions[nextAvailableAction+4],&element->actions[nextAvailableAction+1],0,1);
		UITrigger_Bind(&element->actions[nextAvailableAction+4],&element->actions[nextAvailableAction],1,0);
		UITrigger_Bind(&element->actions[nextAvailableAction+4],&element->actions[nextAvailableAction+2],0,1);
		UITrigger_Bind(&element->actions[nextAvailableAction+4],&element->actions[nextAvailableAction+4],1,0);
	UIConfigure_FillAndBorderSubrect(element, &element->actions[nextAvailableAction+5],x,y,35,25,255,255,255,0,0,0);
	UIConfigure_DisplayStringSubrect(element, &element->actions[nextAvailableAction+6],label,1,x,y,35,25);
	*nextAvailableActionPointer = *nextAvailableActionPointer + 7;
	return &element->actions[nextAvailableAction+3];
}

static int countOptionsForAITemplate(BlockFunctionTemplate *template){
	int i = 0;
	int count = 0;
	while(i < template->numOfArguments){
		switch(template->arguments[i]){
			case BF_COMPARISON:
			case BF_CARGO_QUANTITY:
			case BF_PERCENT:
			case BF_COUNT_QUANTITY:
			case BF_DISTANCE:
			case BF_SEASON_TIME:
			case BF_DURATION:
			case BF_OPERATION:
			case BF_INTEGER:
				count += 1;
				break;
			default:
				break;
		}
		i++;
	}
	return count;
}

static int countUIActionsNeededForAITemplate(BlockFunctionTemplate *template){
	int i = 0;
	int count = 0;
	while(i < template->numOfArguments){
		switch(template->arguments[i]){
			case BF_PRIMARY:
			case BF_SECONDARY:
			case BF_THEN:
				count += 7;
				break;
			case BF_CARGO_QUANTITY:
			case BF_DISTANCE:
			case BF_COUNT_QUANTITY:
			case BF_COMPARISON:
			case BF_PERCENT:
			case BF_SEASON_TIME:
			case BF_DURATION:
			case BF_INTEGER:
			case BF_OPERATION:
				count += 4;
				break;
			default:
				break;
		}
		i++;
	}
	return count;
}

UI_Element *makeHiveCellBlock(int x_offset, int y_offset, UI_Element *parent, HiveCell *hiveCell){
	UI_Element *element = UIElement_Create(x_offset, y_offset, 50,50,6);

	UIConfigure_FillAndBorderRect(element,&element->actions[0], 150,150,150,0,0,0,HIVECELL);
	UIConfigure_ShrinkFitToParent(element,&element->actions[1]);
	UIConfigure_GetPercentCellDone(element,&element->actions[2],hiveCell,HIVE_CELL_SPAWN_DELAY,1,&element->actions[3]);
	UIConfigure_PercentageFillRect(element,&element->actions[3],255,0,0);
	UIConfigure_LeftClickRect(element,&element->actions[4]);
		UITrigger_Bind(&element->actions[4],&element->actions[5],0,1);
	UIConfigure_SetCellToSpawn(element,&element->actions[5],hiveCell);
		quickSetStatus(&element->actions[5],0);
	UIElement_Reparent(element,parent);
	return element;
}

UI_Element *makeStartBlock(int x_offset, int y_offset, UI_Element *parent){
	UI_Element *element;
	element = UIElement_Create(x_offset,y_offset,200,50,10);
	UIConfigure_FillAndBorderRect(element,&element->actions[0],248,221,35,0,0,0,FILLRECT);
	UIConfigure_ShrinkFitToParent(element, &element->actions[1]);
	UIConfigure_DisplayString(element, &element->actions[2],"START",0,UISTRING_ALIGN_CENTER);
	UIConfigure_RenderLine(element, &element->actions[7],BR_CORNER,NULL);

	UIConfigure_LeftClickSubrect(element, &element->actions[4], 175, 25, 25, 25);
		UITrigger_Bind(&element->actions[4],&element->actions[7],-1,1);
		UITrigger_Bind(&element->actions[4],&element->actions[4],1,0);
		UITrigger_Bind(&element->actions[4],&element->actions[5],0,1);
		UITrigger_Bind(&element->actions[4],&element->actions[6],0,1);

	UIConfigure_LeftReleaseAnywhere(element, &element->actions[5]);
		element->actions[5].status = 0;
		element->actions[5].new_status = 0;
		UITrigger_Bind(&element->actions[5],&element->actions[7],1,2);
		UITrigger_Bind(&element->actions[5],&element->actions[4],0,1);
		UITrigger_Bind(&element->actions[5],&element->actions[3],-1,1);
		UITrigger_Bind(&element->actions[5],&element->actions[6],1,0);
		UITrigger_Bind(&element->actions[5],&element->actions[5],1,0);

	UIConfigure_StoreMousePosition(element, &element->actions[6],2,&element->actions[7],&element->actions[3]);
		element->actions[6].status = 0;
		element->actions[6].new_status = 0;
	UIConfigure_CalculateSibling(element, &element->actions[3],1,&element->actions[7]);
		element->actions[3].status = 0;
		element->actions[3].new_status = 0;
	UIConfigure_SetUpAiBlock(element,&element->actions[8],2,&element->actions[2],&element->actions[7]);
	UIConfigure_FillSubrect(element, &element->actions[9], 175, 25, 24, 24, 255, 255, 255);
	UIElement_Reparent(element,parent);
	return element;
}

UI_Element *makeAIResetButton(int x_offset, int y_offset, UI_Element *parent){
	UI_Element *element;
	element = UIElement_Create(x_offset,y_offset,200,50,6);
	UIConfigure_FillAndBorderRect(element,&element->actions[0],74,74,74,0,0,0,STOPBOX);
	UIConfigure_ShrinkFitToParent(element,&element->actions[1]);
	UIConfigure_LeftClickRect(element,&element->actions[2]);
		UITrigger_Bind(&element->actions[2],&element->actions[3],0,1);
		UITrigger_Bind(&element->actions[2],&element->actions[4],0,1);
	UIConfigure_NullifyAI(element,&element->actions[3]);
	UIConfigure_RecallWorkers(element,&element->actions[4]);
	UIConfigure_PercPosition(element,&element->actions[5],1.0,0.0,x_offset,y_offset,1,&element->actions[1]);
	UIElement_Reparent(element,parent);
	return element;
}

UI_Element *makeAITemplateScrollList(int x_offset, int y_offset, AIData *aiData, UI_Element *parent, UI_Element *blockHolder){
	UI_Element *element;
	UI_Element *element2;
	BlockFunctionTemplate *template = aiData->templates;
	int y_offset2 = 0;
	/* Set up the main panel */
	element = UIElement_Create(x_offset, y_offset, 220, 360,7);
	UIConfigure_FillAndBorderRect(element,&element->actions[0],147,147, 170,0,0,0,FILLRECT);
	UIConfigure_ShrinkFitToParent(element,&element->actions[1]);
	/* Set up the slider bar */
		element2 = UIElement_Create(x_offset+200,y_offset,21,20,7);
		UIConfigure_FillAndBorderRect(element2,&element2->actions[0],249,252,124,0,0,0,SCROLLHANDLE);
		UIConfigure_ShrinkFitToParent(element2,&element2->actions[1]);
		UIConfigure_DraggableVerticalOverride(element2,&element2->actions[2],1,&element2->actions[1]);
		UIConfigure_LeftClickRect(element2,&element2->actions[3]);
			UITrigger_Bind(&element2->actions[3],&element2->actions[2],0,1);
			UITrigger_Bind(&element2->actions[3],&element2->actions[4],0,1);
		UIConfigure_LeftReleaseAnywhere(element2,&element2->actions[4]);
			UITrigger_Bind(&element2->actions[4],&element2->actions[2],1,0);
			UITrigger_Bind(&element2->actions[4],&element2->actions[4],1,0);
			UIElement_Reparent(element2,element);
		UIConfigure_PercPosition(element2,&element2->actions[5],1.0,0.0,-x_offset+199,y_offset,1,&element2->actions[1]);
		UIConfigure_SlideWithMouseWheel(element2,&element2->actions[6],0,15,1,&element2->actions[1]);
			element2->actions[6].response = NONE;
		/* Set up the main panel again */
	UIConfigure_GetDifferenceInChildYOffset(element,&element->actions[2],0.0,y_offset,element2,1,&element->actions[4]);
	UIConfigure_PassThrough(element,&element->actions[3],MOUSEWHEEL,1,&element2->actions[6]);
	/* Set up the options */
	while(template!=NULL){
		element2 = UIElement_Create(x_offset, y_offset + y_offset2, 200,25,6);
		UIConfigure_FillAndBorderRect(element2,&element2->actions[0],template->red,template->green,template->blue,0,0,0,BLOCK);
		UIConfigure_ShrinkFitToParentWithYShift(element2,&element2->actions[1],&element->actions[4]);
		UIConfigure_DisplayString(element2,&element2->actions[2],template->name,0,UISTRING_ALIGN_CENTER);
		UIConfigure_LeftClickRect(element2,&element2->actions[3]);
			UITrigger_Bind(&element2->actions[3],&element2->actions[4],-1,1);
		UIConfigure_AddAiBlock(element2,&element2->actions[4],template,blockHolder);
		UIConfigure_PercPosition(element2,&element2->actions[5],1.0,0.0,-x_offset,y_offset+y_offset2,1,&element2->actions[1]);
		UIElement_Reparent(element2,element);
		template = template->next;
		y_offset2 += 35;
	}
	UIConfigure_CalculateScrollListOffset(element,&element->actions[4],y_offset2+60);
	UIConfigure_PercOffsetRect(element,&element->actions[5],1.0,0.0,1.0,1.0,-x_offset,y_offset,-50,-50,1,&element->actions[1]);
	UIConfigure_FillAndBorderSubrect(element,&element->actions[6],200,0,19,99999,74,74,74,0,0,0);
	UIElement_Reparent(element,parent);
	return element;
}

UI_Element *makeAIBlock(int x_offset, int y_offset, BlockFunctionTemplate *template, UI_Element *parent){
	UI_Element *element2;
	char *aiString = template->name;
	int nextAvailableAction = 0;
	int i = 0;
	int optionsPlaced = 0;
	int x;
	int optionsCount = countOptionsForAITemplate(template);
	int count = 8 + countUIActionsNeededForAITemplate(template);
	element2 = UIElement_Create(x_offset,y_offset,200,50,count);
	UI_Action *primary = NULL;
	UI_Action *secondary = NULL;
	UI_Action *comparator = NULL;
	UI_Action *integerArg = NULL;
	/* Set it up */
	UIConfigure_FillAndBorderRect(element2,
															  &element2->actions[0],
																template->red,
																template->green,
																template->blue,
																0,0,0,BLOCK);
	UIConfigure_ShrinkFitToParent(element2,&element2->actions[1]);
	UIConfigure_RightClickRect(element2, &element2->actions[2]);
		UITrigger_Bind(&element2->actions[2],&element2->actions[3],0,1);
		UITrigger_Bind(&element2->actions[2],&element2->actions[4],0,1);
		UITrigger_Bind(&element2->actions[2],&element2->actions[2],1,0);
	UIConfigure_RightReleaseAnywhere(element2, &element2->actions[3]);
		UITrigger_Bind(&element2->actions[3],&element2->actions[3],1,0);
		UITrigger_Bind(&element2->actions[3],&element2->actions[4],1,0);
		UITrigger_Bind(&element2->actions[3],&element2->actions[2],0,1);
	UIConfigure_DraggableRectOverride(element2, &element2->actions[4],1,&element2->actions[1]);
	UIConfigure_DisplayString(element2, &element2->actions[5],aiString,0,UISTRING_ALIGN_CENTER);

	nextAvailableAction = 6;

	while(i < template->numOfArguments){
		switch(template->arguments[i]){
			case BF_PRIMARY:
				primary = makeAIBlockLineLinker(element2, &nextAvailableAction, 0,25, "Yes", BL_CORNER);
				break;
			case BF_THEN:
				primary = makeAIBlockLineLinker(element2, &nextAvailableAction, 0,25, "Then", BL_CORNER);
				break;
			case BF_SECONDARY:
				secondary = makeAIBlockLineLinker(element2, &nextAvailableAction, 165,25, "No", BR_CORNER);
				break;
			case BF_COMPARISON:
				x = 130 / (optionsCount + 1);
				x *= (1 + optionsPlaced);
				x -= 25/2;
				x += 35;
				comparator = makeAIBlockComparatorSelector(element2, &nextAvailableAction, x, 25);
				optionsPlaced++;
				break;
			case BF_OPERATION:
				x = 130 / (optionsCount + 1);
				x *= (1 + optionsPlaced);
				x -= 25/2;
				x += 35;
				comparator = makeAIBlockOperatorSelector(element2, &nextAvailableAction, x, 25);
				optionsPlaced++;
				break;
			case BF_DISTANCE:
			case BF_COUNT_QUANTITY:
			case BF_CARGO_QUANTITY:
			case BF_PERCENT:
			case BF_SEASON_TIME:
			case BF_DURATION:
			case BF_INTEGER:
				x = 130 / (optionsCount + 1);
				x *= (1 + optionsPlaced);
				x -= 25/2;
				x += 35;
				integerArg = makeAIBlockIntegerSelector(element2, &nextAvailableAction, x, 25);
				optionsPlaced++;
				break;
			case BF_WORKER_STATUS:
				break;
		}
		i++;
	}

	UIConfigure_DeleteKeyFlagDestroy(element2, &element2->actions[nextAvailableAction]);
	UIConfigure_SetUpAiBlock(element2,&element2->actions[nextAvailableAction+1],5,&element2->actions[5],primary,secondary,comparator,integerArg);
	assert(nextAvailableAction < element2->num_of_actions);

	UIElement_Reparent(element2,parent);
	return element2;
}

UI_Element *UIElement_Create(int x, int y, int w, int h, int num_of_actions){
	   UI_Element *element;
	   element  = calloc(1,sizeof(UI_Element));


	   element->rect.x = x;
	   element->rect.y = y;
	   element->rect.w = w;
	   element->rect.h = h;
	   if(num_of_actions == 0){
			element->actions = NULL;
			element->num_of_actions = 0;
	   }
	   else{
		   element->actions = malloc(sizeof(UI_Action)*num_of_actions);
		   element->num_of_actions = num_of_actions;
	   }
	   element->parent = NULL;
	   element->child = NULL;
	   element->sibling = NULL;
	   element->exposed_data_types = NULL;
	   element->exposed_data = NULL;
	   element->exposed_data_count = 0;
	   return element;
}

/* UIAction_OnIntegerChange

   This calls it's triggers when a given integer changes value */

#define watchedInteger extra
#define previousValue integers[0]

int UIAction_OnIntegerChange(UI_Action *action, UIData *uiData){
	(void)uiData;
	if(action->status != 0){
		if(action->previousValue != *((int*)action->watchedInteger)){
			action->previousValue = *((int*)action->watchedInteger);
			UITrigger_Execute(action);
		}
		return 1;
	}
	return 0;
}

void UIConfigure_OnIntegerChange(UI_Element *element, UI_Action *action, int *target){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_OnIntegerChange;
	action->integers = calloc(1,sizeof(int));
	action->previousValue = *target;
	action->watchedInteger = target;
}

#undef watchedInteger
#undef previousValue

/* UIAction_StringCollection

	 This action holds a list of strings and writes it into it's companions buffers
   The string selected is toggled by it's status, enabling it to be triggered using UITRIGGER_PLUSONE */

int UIAction_StringCollection(UI_Action *action, UIData *uiData){
	(void)uiData;
	if(action->status != 0){
		if(action->status > action->num_of_strings){
			action->status = 1;
			action->new_status = 1;
		}
		int i = 0;
		while(i < action->num_of_companions){
			sUIAction_DisplayString_EditString(action->companions[i], action->strings[action->status-1]);
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_StringCollection(UI_Element *element, UI_Action *action, int num_of_companions, int num_of_strings, ...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_strings);
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_StringCollection;
	action->companions =  calloc(num_of_companions,sizeof(void*));
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	i = 0;
	action->strings = calloc(num_of_strings,sizeof(void*));
	while(i < num_of_strings){
		char *str = va_arg(vargs,char*);
		action->strings[i] = calloc(strlen(str)+1,sizeof(char));
		strcpy(action->strings[i],str);
		i++;
	}
	action->num_of_strings = num_of_strings;
	va_end(vargs);
}



/* UIAction_DisplayNumber

	 This UI_Action displays a number onto the screen. The number can be modified
	 using sUIAction_DisplayNumber_EditNumber(). Alignment of the number can be
	 specified when configuring it. Font is using a index to the array of fonts in
	 GraphicsData */

#define newNumber integers[0]
#define currentNumber integers[1]
#define fontID integers[2]
#define alignStyle integers[3]
#define alignOffset integers[4]
#define numberAsString strings[0]
#define rectX integers[5]
#define rectY integers[6]
#define rectW integers[7]
#define rectH integers[8]

int UIAction_DisplayNumberSubrect(UI_Action *action, UIData *uiData){
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	SDL_Rect temp_rect2;
	int w,h;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	graphicsData = uiData->graphicsData;
	if(action->newNumber != action->currentNumber){
		action->currentNumber = action->newNumber;
		sprintf(action->numberAsString,"%d",action->currentNumber);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Blended(graphicsData->fonts[action->fontID], action->numberAsString, colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
		SDL_FreeSurface(temp);
		TTF_SizeText(graphicsData->fonts[action->fontID],action->numberAsString,&w,&h);
		action->alignOffset = w;

	}
	if(UIElement_isVisible(action->element) && action->element->rect.h > TTF_FontHeight(graphicsData->fonts[action->fontID]) && action->status != 0 && action->texture != NULL){
		temp_rect2.w = action->rectW;
		temp_rect2.h = action->rectH;
		temp_rect2.x = action->rectX + action->element->rect.x;
		temp_rect2.y = action->rectY + action->element->rect.y;
		shrinkRectToFit(&temp_rect2, &action->element->rect);
		TTF_SizeText(graphicsData->fonts[action->fontID], action->numberAsString, &temp_rect.w, &temp_rect.h);
		temp_rect.x = temp_rect2.x + (temp_rect2.w - temp_rect.w)/2;
		temp_rect.y = temp_rect2.y + (temp_rect2.h - temp_rect.h)/2;
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
		return 1;
	}
	return 0;
}

void UIConfigure_DisplayNumberSubrect(UI_Element *element, UI_Action *action, int number, int font, int x, int y, int w, int h){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->status = 1;
	action->new_status = 1;
	action->function = UIAction_DisplayNumberSubrect;
	action->integers = calloc(9, sizeof(int));
	action->newNumber = number;
	action->currentNumber = font;
	action->rectX = x;
	action->rectY = y;
	action->rectW = w;
	action->rectH = h;
	action->num_of_integers = 9;
	action->strings = malloc(sizeof(char*) * 1);
	action->numberAsString = calloc(30,sizeof(char));
	action->num_of_strings = 1;
}

#undef newNumber
#undef currentNumber
#undef fontID
#undef alignStyle
#undef alignOffset
#undef numberAsString
#undef rectX
#undef rectY
#undef rectW
#undef rectH


/* UIAction_DisplayStringSubrect

	*/

#define currentString strings[0]
#define newString strings[1]
#define fontID integers[0]
#define alignOffset integers[3]
#define rectX integers[4]
#define rectY integers[5]
#define rectW integers[6]
#define rectH integers[7]


int UIAction_DisplayStringSubrect(UI_Action *action, UIData *uiData){
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	SDL_Rect temp_rect2;
	int w,h;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	graphicsData = uiData->graphicsData;
	if(action->currentString != NULL && action->newString != NULL && strcmp(action->currentString,action->newString) != 0){
		action->currentString = realloc(action->currentString, strlen(action->newString)+1);
		strcpy(action->currentString,action->newString);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Blended(graphicsData->fonts[action->fontID],action->currentString,colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
		SDL_FreeSurface(temp);
		TTF_SizeText(graphicsData->fonts[action->fontID],action->currentString,&w,&h);
		action->alignOffset = w;
	}
	if(UIElement_isVisible(action->element) && action->element->rect.h > TTF_FontHeight(graphicsData->fonts[action->fontID]) && action->currentString != NULL && action->status != 0 && action->texture != NULL){
		temp_rect2.w = action->rectW;
		temp_rect2.h = action->rectH;
		temp_rect2.x = action->rectX + action->element->rect.x;
		temp_rect2.y = action->rectY + action->element->rect.y;
		shrinkRectToFit(&temp_rect2, &action->element->rect);
		TTF_SizeText(graphicsData->fonts[action->fontID], action->currentString, &temp_rect.w, &temp_rect.h);
		temp_rect.x = temp_rect2.x + (temp_rect2.w - temp_rect.w)/2;
		temp_rect.y = temp_rect2.y + (temp_rect2.h - temp_rect.h)/2;
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
		return 1;
	}
	return 0;
}

void UIConfigure_DisplayStringSubrect(UI_Element *element, UI_Action *action, char *string, int font, int x, int y, int w, int h){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_DisplayStringSubrect;
	action->strings = malloc(sizeof(char*) * 2);
	if(string != NULL){
		action->currentString = calloc(strlen(string)+1,sizeof(char));
		action->newString = calloc(strlen(string)+1,sizeof(char));
		strcpy(action->newString, string);
	}
	else{
		action->currentString = NULL;
		action->newString = NULL;
	}
	action->new_status = 1;
	action->status = 1;
	action->num_of_strings = 2;
	action->integers = calloc(8,sizeof(int));
	action->fontID = font;
	action->rectX = x;
	action->rectY = y;
	action->rectW = w;
	action->rectH = h;
	action->num_of_integers = 8;
}

#undef currentString
#undef newString
#undef fontID
#undef alignOffset
#undef rectX
#undef rectY
#undef rectW
#undef rectH


/* UIAction_ClickSubrect

	 This is a mutli-configurable action - you can do both left and right click versions
	 of it */

#define rectX integers[0]
#define rectY integers[1]
#define rectW integers[2]
#define rectH integers[3]

int UIAction_ClickSubrect(UI_Action *action, UIData *uiData){
		SDL_Rect r;
		if(action->status != 0){
			r.x = action->rectX + action->element->rect.x;
			r.y = action->rectY + action->element->rect.y;
			r.w = action->rectW;
			r.h = action->rectH;
			shrinkRectToFit(&r,&action->element->rect);

			#if SHOW_SUBRECTS
			SDL_SetRenderDrawColor(uiData->graphicsData->renderer, 255,255,255,50);
			SDL_RenderFillRect(uiData->graphicsData->renderer, &r);
			#endif

			if(isPointInRect(uiData->event->button.x, uiData->event->button.y, r)){
				UITrigger_Execute(action);
				return 1;
			}
		}
		return 0;
}

void UIConfigure_LeftClickSubrect(UI_Element *element, UI_Action *action, int x, int y, int w, int h){
	UIAction_Init(element,action);
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickSubrect;
	action->integers = calloc(4,sizeof(int));
	action->rectX = x;
	action->rectY = y;
	action->rectW = w;
	action->rectH = h;
	action->num_of_integers = 4;
}

void UIConfigure_RightClickSubrect(UI_Element *element, UI_Action *action, int x, int y, int w, int h){
	UIAction_Init(element,action);
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickSubrect;
	action->integers = calloc(4,sizeof(int));
	action->rectX = x;
	action->rectY = y;
	action->rectW = w;
	action->rectH = h;
	action->num_of_integers = 4;
}

#undef rectX
#undef rectY
#undef rectW
#undef rectH

/* UIAction_FillSubrect

	 This will fill a smaller rectangle on the surface of a UI_Element */

#define rectX integers[0]
#define rectY integers[1]
#define rectW integers[2]
#define rectH integers[3]

#define fillRed integers[4]
#define fillGreen integers[5]
#define fillBlue integers[6]
#define borderRed integers[7]
#define borderGreen integers[8]
#define borderBlue integers[9]

int UIAction_FillAndBorderSubrect(UI_Action *action, UIData *uiData){
	SDL_Rect r;
	if(action->status != 0 && UIElement_isVisible(action->element)){
		r.x = action->rectX + action->element->rect.x;
		r.y = action->rectY + action->element->rect.y;
		r.w = action->rectW;
		r.h = action->rectH;
		shrinkRectToFit(&r,&action->element->rect);
		SDL_SetRenderDrawColor(uiData->graphicsData->renderer,action->fillRed,action->fillGreen,action->fillBlue,255);
		SDL_RenderFillRect(uiData->graphicsData->renderer,&r);
		SDL_SetRenderDrawColor(uiData->graphicsData->renderer, action->borderRed, action->borderBlue, action->borderGreen,255);
		SDL_RenderDrawRect(uiData->graphicsData->renderer,&r);
		return 1;
	}
	return 0;
}

void UIConfigure_FillAndBorderSubrect(UI_Element *element, UI_Action *action, int x, int y, int w, int h, int red, int green, int blue,int bred, int bgreen, int bblue){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_FillAndBorderSubrect;
	action->integers = calloc(10,sizeof(int));
	action->num_of_integers = 10;
	action->rectX = x;
	action->rectY = y;
	action->rectW = w;
	action->rectH = h;
	action->fillRed = red;
	action->fillGreen = green;
	action->fillBlue = blue;
	action->borderRed = bred;
	action->borderGreen = bgreen;
	action->borderBlue = bblue;
}

#undef rectX
#undef rectY
#undef rectW
#undef rectH
#undef fillRed
#undef fillGreen
#undef fillBlue
#undef borderRed
#undef borderGreen
#undef borderBlue

/* UIAction_FillSubrect

	 This will fill a smaller rectangle on the surface of a UI_Element */

#define rectX integers[0]
#define rectY integers[1]
#define rectW integers[2]
#define rectH integers[3]

#define fillRed integers[4]
#define fillGreen integers[5]
#define fillBlue integers[6]

int UIAction_FillSubrect(UI_Action *action, UIData *uiData){
	SDL_Rect r;
	if(action->status != 0){
		r.x = action->rectX + action->element->rect.x;
		r.y = action->rectY + action->element->rect.y;
		r.w = action->rectW;
		r.h = action->rectH;
		shrinkRectToFit(&r,&action->element->rect);
		SDL_SetRenderDrawColor(uiData->graphicsData->renderer,action->fillRed,action->fillGreen,action->fillBlue,255);
		SDL_RenderFillRect(uiData->graphicsData->renderer,&r);
		return 1;
	}
	return 0;
}

void UIConfigure_FillSubrect(UI_Element *element, UI_Action *action, int x, int y, int w, int h, int red, int green, int blue){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_FillSubrect;
	action->integers = calloc(7,sizeof(int));
	action->num_of_integers = 7;
	action->rectX = x;
	action->rectY = y;
	action->rectW = w;
	action->rectH = h;
	action->fillRed = red;
	action->fillGreen = green;
	action->fillBlue = blue;
}

#undef rectX
#undef rectY
#undef rectW
#undef rectH
#undef fillRed
#undef fillGreen
#undef fillBlue

/* UIAction_SetCellToSpawn

	 This makes a cell start counting down from the start to spawn a new bee */

#define cell extra

int UIAction_SetCellToSpawn(UI_Action *action, UIData *uiData){
	if(action->status != 0){
		if(uiData->gameObjectData->hive.flowers_collected >= SUGAR_COST_OF_WORKER && ((HiveCell*)action->cell)->timer == -1){
			uiData->gameObjectData->hive.flowers_collected -= SUGAR_COST_OF_WORKER;
			((HiveCell*)action->cell)->timer = HIVE_CELL_SPAWN_DELAY;
		}
		action->new_status = 0;
	}
	return 0;
}

void UIConfigure_SetCellToSpawn(UI_Element *element, UI_Action *action, HiveCell *cell){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_SetCellToSpawn;
	action->extra = cell;
}

#undef cell

/* UIAction_GetCellPercentDone

	This reads how far done a HiveCell is done as a percentage into it's companions
	float[0] field */

#define percentage floats[0]
#define max_timer floats[1]
#define cell extra

int UIAction_GetCellPercentDone(UI_Action *action, UIData *uiData){
	(void)uiData;
	if(action->status != 0){
		if(((HiveCell*)action->cell)->timer < 0){
			action->percentage = 0.0;
		}
		else{
			action->percentage = (double)((double)((HiveCell*)action->cell)->timer) / action->max_timer;
		}
		int i = 0;
		while(i < action->num_of_companions){
			action->companions[0]->percentage = action->percentage;
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_GetPercentCellDone(UI_Element *element, UI_Action *action, HiveCell *hiveCell, int maximumTime, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = UPDATE;
	action->function = UIAction_GetCellPercentDone;
	action->floats = calloc(2,sizeof(double));
	action->floats[1] = (double) maximumTime;
	action->companions = calloc(num_of_companions,sizeof(void*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->num_of_companions = num_of_companions;
	action->extra = hiveCell;
	va_end(vargs);
}

#undef percentage
#undef max_timer
#undef cell

/* UIAction_PercentageFillRect

	 This UI_Action fills a percentage of the element's rectangle with a given color.
	 Defaults to 0% filled.*/

#define percentage floats[0]
#define fillRed integers[0]
#define fillGreen integers[1]
#define fillBlue integers[2]

int UIAction_PercentageFillRect(UI_Action *action, UIData *uiData){
  GraphicsData *graphicsData;
  graphicsData = uiData->graphicsData;
  SDL_Rect rect;
	if(action->status != 0){
		rect.x = action->element->rect.x;
		rect.w = action->element->rect.w;
		rect.h = (int)(action->percentage *(double)action->element->rect.h);
		rect.y = action->element->rect.y + (int)((1.0 - action->percentage) * (double)action->element->rect.h);
		SDL_SetRenderDrawColor(uiData->graphicsData->renderer,action->fillRed,action->fillGreen,action->fillBlue,100);
		SDL_RenderFillRect(uiData->graphicsData->renderer,&rect);
    SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[HIVECELLMASK_GRAPHIC], NULL, &action->element->rect);
		return 1;
	}
	return 0;
}

void UIConfigure_PercentageFillRect(UI_Element *element, UI_Action *action, int r, int g, int b){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_PercentageFillRect;
	action->floats = calloc(1,sizeof(double));
	action->num_of_floats = 1;
	action->integers = calloc(3,sizeof(int));
	action->fillRed = r;
	action->fillGreen = g;
	action->fillBlue = b;
	action->num_of_integers = 3;
}

#undef percentage
#undef fillRed
#undef fillGreen
#undef fillBlue

int UIAction_MuteSound(UI_Action *action, UIData *uiData){
	AudioData *audioData = uiData->audioData;
	if(action->status != 0){
		muteMusic(audioData);
		action->new_status = 0;
	}
	return 0;
}

void UIConfigure_MuteSound(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_MuteSound;
	action->new_status = 0;
	action->status = 0;
}


int UIAction_MuteSoundFX(UI_Action *action, UIData *uiData){
	AudioData *audioData = uiData->audioData;
	if(action->status != 0){
		muteSoundEffects(audioData);
		action->new_status = 0;
	}
	return 0;
}

void UIConfigure_MuteSoundFX(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_MuteSoundFX;
	action->new_status = 0;
	action->status = 0;
}
/* UIAction_MinimapMouseMove

	 THis UI_Action causes a UI_ELement to adjust the navigation offset in GraphicsData
	 relative to the position of the mouse cursor to the element. Its used to
	 implement the click-and-drag scrolling of the Minimap */

int UIAction_MinimapMouseMove(UI_Action *action, UIData *uiData){
	GraphicsData *graphicsData;
	SDL_Point p;
	int x,y;
	if(action->status != 0){
		graphicsData = uiData->graphicsData;
		SDL_GetMouseState(&x,&y);
		if(isPointInRect(x,y,action->element->rect)){
			p.x = (double)(-action->element->rect.x + x) / (double)action->element->rect.w *(double)X_SIZE_OF_WORLD;
			p.y = (double)(-action->element->rect.y + y) / (double)action->element->rect.h *(double)Y_SIZE_OF_WORLD;
			SDL_GetWindowSize(graphicsData->window,&x,&y);
			setNavigationOffset(graphicsData,p.x - x/2,p.y - y/2);
		}
		return 1;
	}
	return 0;
}

void UIConfigure_MinimapMouseMove(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_MinimapMouseMove;
}

/* UIAction_Minimap

	 This UI_Action draws representations of all the game objects in the world onto
	 the UI_Element it's attached to.

	 The static function sUIAction_Minimap_DrawGameObject takes the rectangle of a
	 GameObject and draws it onto the screen - I think it will be optimised out of
	 existence when compiled, so it's here to make the code shorter and more readable*/

static void sUIAction_Minimap_DrawGameObject(UI_Action *action, GraphicsData *graphicsData, SDL_Rect *rect){
		SDL_Rect rect2;
		double relx,rely;
		relx = (double)rect->x / X_SIZE_OF_WORLD;
		rely = (double)rect->y / Y_SIZE_OF_WORLD;
		rect2.x = relx*action->element->rect.w+action->element->rect.x;
		rect2.y = rely*action->element->rect.h+action->element->rect.y;
		rect2.w = ((double)rect->w*3 / X_SIZE_OF_WORLD) * action->element->rect.w;
		rect2.h = ((double)rect->h*3 / Y_SIZE_OF_WORLD) * action->element->rect.h;
		shrinkRectToFit(&rect2,&action->element->rect);
		SDL_RenderFillRect(graphicsData->renderer,&rect2);
}

int UIAction_Minimap(UI_Action *action, UIData *uiData){
	GameObjectData *gameObjectData;
	GraphicsData *graphicsData;
	SDL_Rect rect;
	int i = 0,j = 0;
	ProgrammableWorker *p;
	if(action->status != 0){
		gameObjectData = uiData->gameObjectData;
		graphicsData = uiData->graphicsData;
		/* Then we draw all the game object stuff into our element's rectangle */

		/* This is for drawing on the nodes (AKA flowers) */
		while(i < gameObjectData->resourceNodeSpawnerCount){
			j = 0;
			while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
				if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){
					switch(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].type){
						case 0:
							SDL_SetRenderDrawColor(graphicsData->renderer,0,0,255,255);
							break;
						case 1:
							SDL_SetRenderDrawColor(graphicsData->renderer,255,0,0,255);
							break;
						case 2:
						  SDL_SetRenderDrawColor(graphicsData->renderer,255,255,0,255);
							break;
					}
					sUIAction_Minimap_DrawGameObject(action,
					                                 graphicsData,
																				   &gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect);
				}
				j++;
			}
			i++;
		}
		/* This is for drawing on the trees */
		SDL_SetRenderDrawColor(graphicsData->renderer,0x0B,0x61,0x0B,255);
		i = 0;
		while(i < NUMBER_OF_TREES){
			sUIAction_Minimap_DrawGameObject(action,
			                                 graphicsData,
																		   &gameObjectData->tree[i].stumpRect);
			i++;
		}
		/* This is for drawing the hive */
		SDL_SetRenderDrawColor(graphicsData->renderer,0x8A,0x4B,0x08,0xFF);
		sUIAction_Minimap_DrawGameObject(action,graphicsData,&gameObjectData->hive.rect);
		/* This is for drawing the workers */
		SDL_SetRenderDrawColor(graphicsData->renderer,0xDB,0xA9,0x01,0xFF);
		p = gameObjectData->first_programmable_worker;
		while(p != NULL){
			sUIAction_Minimap_DrawGameObject(action,graphicsData,&p->rect);
			p = p->next;
		}
		SDL_SetRenderDrawColor(graphicsData->renderer,0,0,0,255);
		sUIAction_Minimap_DrawGameObject(action,graphicsData,&gameObjectData->roamingSpider->rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,255,255,255,255);
		sUIAction_Minimap_DrawGameObject(action, graphicsData,&gameObjectData->iceCreamPerson->rect);

		if(gameObjectData->droppedIceCream->dropped){
			SDL_SetRenderDrawColor(graphicsData->renderer,255,0,0,255);
			sUIAction_Minimap_DrawGameObject(action, graphicsData,&gameObjectData->droppedIceCream->rect);
		}

		SDL_SetRenderDrawColor(graphicsData->renderer,255,255,255,255);
		rect.x = (double)-graphicsData->navigationOffset.x / X_SIZE_OF_WORLD * action->element->rect.w + action->element->rect.x;
		rect.y = (double)-graphicsData->navigationOffset.y / Y_SIZE_OF_WORLD * action->element->rect.h + action->element->rect.y;
		SDL_GetWindowSize(graphicsData->window,&rect.w,&rect.h);
		rect.w = (double)rect.w / X_SIZE_OF_WORLD * action->element->rect.w;
		rect.h = (double)rect.h / Y_SIZE_OF_WORLD * action->element->rect.h;
		SDL_RenderDrawRect(graphicsData->renderer,&rect);
	}
	return 0;
}

void UIConfigure_Minimap(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_Minimap;
}

/* UIAction_SlideWithMouseWheel

	 this UI_Action makes an object adjust its position based on the movements of
	 the mousewheel - which can be used to either go vertically or horizontally
	 technically but in the program I think we only use it to vertically.

	 When creating it, it needs the x and y scrollspeeds (in pixels per scroll),
	 and also any companions that need to react to the change in position */

#define mouseWheelXScrollSpeed integers[0]
#define mouseWheelYScrollSpeed integers[1]
#define companionXPosition integers[0]
#define companionYPosition integers[1]

int UIAction_SlideWithMouseWheel(UI_Action *action, UIData *uiData){
	int i = 0;
	int x,y;
	SDL_Event *event;
	if(action->status != 0){
		event = uiData->event;
		while(i < action->num_of_companions){
			if(action->mouseWheelXScrollSpeed!=0){
				x = action->companions[i]->companionXPosition + event->wheel.x * -abs(action->mouseWheelXScrollSpeed);

				/* Make sure it fits !*/
				if(x < action->element->parent->rect.x){
					x = action->element->parent->rect.x;
				}
				else if(x + action->element->rect.h > action->element->parent->rect.x + action->element->parent->rect.w){
					x = action->element->parent->rect.x + action->element->parent->rect.w - action->element->rect.w;
				}

				action->companions[i]->companionXPosition = x;
			}
			if(action->mouseWheelYScrollSpeed!=0){
				y = action->companions[i]->companionYPosition + event->wheel.y * -abs(action->mouseWheelYScrollSpeed);

				/* Make sure it fits !*/
				if(y < action->element->parent->rect.y){
					y = action->element->parent->rect.y;
				}
				else if(y + action->element->rect.h > action->element->parent->rect.y + action->element->parent->rect.h){
					y = action->element->parent->rect.y + action->element->parent->rect.h - action->element->rect.h;
				}

				action->companions[i]->companionYPosition = y;
			}
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_SlideWithMouseWheel(UI_Element *element, UI_Action *action, int x, int y, int num_of_companions,...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = MOUSEWHEEL;
	action->function = UIAction_SlideWithMouseWheel;
	action->integers = calloc(2,sizeof(int));
	action->num_of_integers = 2;
	action->mouseWheelXScrollSpeed = x;
	action->mouseWheelYScrollSpeed = y;
	action->companions = calloc(num_of_companions,sizeof(UI_Action*));
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}
#undef mouseWheelXScrollSpeed
#undef mouseWheelYScrollSpeed
#undef companionXPosition
#undef companionYPosition

/* UIAction_PassThrough

	 This UI_Action simply calls another UI_Action when it is triggered, enabling
	 things like passing the MouseWheelEvents on one object to another.

	 You need to manually specify the Response that this UI_Action has, and it also
	 needs to have companions specified (otherwise it's kind of useless) */

int UIAction_PassThrough(UI_Action *action, UIData *uiData){
	int i = 0;
	if(action->status != 0){
		while(i < action->num_of_companions){
			action->companions[i]->function(action->companions[i],uiData);
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_PassThrough(UI_Element *element, UI_Action *action, enum Response response, int num_of_companions, ...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = response;
	action->function = UIAction_PassThrough;
	action->companions = calloc(num_of_companions,sizeof(UI_Action*));
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

/* UIAction_GetAnnouncement

	 This UI_Action takes whatever is currently in the announcement buffer and writes
	 it into a string displaying UI_Action. We use it to make the announcement bar
	 change as different messages are written in */

#define companionChangeDisplayString strings[1]
#define displayStringAction companions[0]

int UIAction_GetAnnouncement(UI_Action *action, UIData *uiData){
		AnnouncementsData* announcementsData;
		announcementsData = uiData->announcementsData;
		if(action->status != 0){
			action->displayStringAction->companionChangeDisplayString = realloc(action->displayStringAction->companionChangeDisplayString,
				                                                                  strlen(announcementsData->announcement)+1);
			strcpy(action->displayStringAction->companionChangeDisplayString, announcementsData->announcement);
		}
		return 0;
}

void UIConfigure_GetAnnouncement(UI_Element *element, UI_Action *action, UI_Action *placeToPut){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_GetAnnouncement;
	action->companions = calloc(1,sizeof(void*));
	action->num_of_companions = 1;
	action->displayStringAction = placeToPut;
}

int UIAction_GetGameOverString(UI_Action *action, UIData *uiData){
		GameOverData* gameOverData;
		gameOverData = uiData->gameOverData;
		if(action->status != 0){
			action->displayStringAction->companionChangeDisplayString = realloc(action->displayStringAction->companionChangeDisplayString,
				                                                                  strlen(gameOverData->gameOverString)+1);
			strcpy(action->displayStringAction->companionChangeDisplayString, gameOverData->gameOverString);
		}
		return 0;
}

void UIConfigure_GetGameOverString(UI_Element *element, UI_Action *action, UI_Action *placeToPut){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_GetGameOverString;
	action->companions = calloc(1,sizeof(void*));
	action->num_of_companions = 1;
	action->displayStringAction = placeToPut;
}

int UIAction_GetInfoDisplayString(UI_Action *action, UIData *uiData){
		if(action->status != 0){
			action->displayStringAction->companionChangeDisplayString = realloc(action->displayStringAction->companionChangeDisplayString,
				                                                                  strlen(uiData->gameObjectData->gameObjectSelection.nameString)+1);
			strcpy(action->displayStringAction->companionChangeDisplayString, uiData->gameObjectData->gameObjectSelection.nameString);
		}
		return 0;
}

void UIConfigure_GetInfoDisplayString(UI_Element *element, UI_Action *action, UI_Action *placeToPut){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_GetInfoDisplayString;
	action->companions = calloc(1,sizeof(void*));
	action->num_of_companions = 1;
	action->displayStringAction = placeToPut;
}

int UIAction_GetObjectStatusString(UI_Action *action, UIData *uiData){
		if(action->status != 0){
			action->displayStringAction->companionChangeDisplayString = realloc(action->displayStringAction->companionChangeDisplayString,
				                                                                  strlen(uiData->gameObjectData->gameObjectSelection.infoString)+1);
			strcpy(action->displayStringAction->companionChangeDisplayString, uiData->gameObjectData->gameObjectSelection.infoString);
		}
		return 0;
}

void UIConfigure_GetObjectStatusString(UI_Element *element, UI_Action *action, UI_Action *placeToPut){
	UIAction_Init(element,action);
	action->response = UPDATE;

	action->function = UIAction_GetObjectStatusString;
	action->companions = calloc(1,sizeof(void*));
	action->num_of_companions = 1;
	action->displayStringAction = placeToPut;
}

int UIAction_GetFinalScoreString(UI_Action *action, UIData *uiData){
		GameOverData* gameOverData;
		gameOverData = uiData->gameOverData;
		if(action->status != 0){
			action->displayStringAction->companionChangeDisplayString = realloc(action->displayStringAction->companionChangeDisplayString,
				                                                                  strlen(gameOverData->finalScoreString)+1);
			strcpy(action->displayStringAction->companionChangeDisplayString, gameOverData->finalScoreString);
		}
		return 0;
}

void UIConfigure_GetFinalScoreString(UI_Element *element, UI_Action *action, UI_Action *placeToPut){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_GetFinalScoreString;
	action->companions = calloc(1,sizeof(void*));
	action->num_of_companions = 1;
	action->displayStringAction = placeToPut;
}

#undef companionChangeDisplayString
#undef displayStringAction

/* UIAction_ToggleInteger

	 This UI_Action will take change the value of an integer given by a pointer to
	 be the NOT of it.

	 It's on you to ensure that this doesn't segfault - it shouldn't when pointing
	 at one of the [thing]Data structs because all those will outlive the UI */

#define targetInt extra

int UIAction_ToggleInteger(UI_Action *action, UIData *uiData){
	int *p;
	(void)uiData;
	if(action->status == 1){
		p = action->targetInt;
		*p = !*(p);
		action->new_status = 0;
	}
	return 0;
}

void UIConfigure_ToggleInteger(UI_Element *element, UI_Action *action, int *target){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_ToggleInteger;
	action->status = 0;
	action->new_status = 0;
	action->targetInt = target;
}

#undef targetInt

/* UIAction_PercOffsetRect

	 This UI_Action defines the rectangle of the parent UI_Element relative to the
	 size of the screen in both position and dimensions.

	 It's kind of hard to explain without a diagram, but what this means is that
	 you can define the x/y/w/h like this:

	 x/y/w/h = (size_of_the_screen * float_between_0_and_1) + fixed_integer_offset

	 which lets you do things like make the AI editor always take up all the window
	 except a 50 pixel border round the edge of the screen.

	 As with all, you'll need to add anything that wants to adjust the position or size
	 of the element as a companion to this, otherwise they'll fight and it'll mess things up */

#define xPercentageOfScreen floats[0]
#define yPercentageOfScreen floats[1]
#define wPercentageOfScreen floats[2]
#define hPercentageOfScreen floats[3]
#define xFixedAmount integers[0]
#define yFixedAmount integers[1]
#define wFixedAmount integers[2]
#define hFixedAmount integers[3]
#define companionX integers[0]
#define companionY integers[1]
#define companionW integers[2]
#define companionH integers[3]

int UIAction_PercOffsetRect(UI_Action *action, UIData *uiData){
		SDL_Point point;
		SDL_Point point2;
		int i = 0;
		if(action->status == 1){
			point = getPointFromPerc(uiData->graphicsData->window,
													                             action->xPercentageOfScreen,
													                             action->yPercentageOfScreen);
			point2 = getPointFromPerc(uiData->graphicsData->window,
		                            action->wPercentageOfScreen,
															  action->hPercentageOfScreen);
			action->element->rect.x = point.x + action->xFixedAmount;
			action->element->rect.y = point.y + action->yFixedAmount;
			action->element->rect.w = point2.x + action->wFixedAmount - point.x - action->xFixedAmount;
			action->element->rect.h = point2.y + action->hFixedAmount - point.y - action->yFixedAmount;
			while(i < action->num_of_companions){
				action->companions[i]->companionX = point.x + action->xFixedAmount;
				action->companions[i]->companionY = point.y + action->yFixedAmount;
				action->companions[i]->companionW = point2.x + action->wFixedAmount - point.x - action->xFixedAmount;
				action->companions[i]->companionH = point2.y + action->hFixedAmount - point.y - action->yFixedAmount;
				i++;
			}
			return 1;
		}
		return 0;
}

void UIConfigure_PercOffsetRect(UI_Element *element, UI_Action *action, double xPerc, double yPerc,
	 																																			double wPerc, double hPerc,
                                                                        int xFixed, int yFixed,
																																				int wFixed, int hFixed,
																																			  int num_of_companions, ...){
		va_list vargs;
		int i = 0;
		va_start(vargs,num_of_companions);
		UIAction_Init(element,action);
		action->response = WINDOW_RESIZE;
		action->function = UIAction_PercOffsetRect;
		action->floats = calloc(4,sizeof(double));
		action->xPercentageOfScreen = xPerc;
		action->yPercentageOfScreen = yPerc;
		action->wPercentageOfScreen = wPerc;
		action->hPercentageOfScreen = hPerc;
		action->integers = calloc(4,sizeof(int));
		action->xFixedAmount = xFixed;
		action->yFixedAmount = yFixed;
		action->wFixedAmount = wFixed;
		action->hFixedAmount = hFixed;
		action->companions = malloc(sizeof(void*) * num_of_companions);
		while(i < num_of_companions){
			action->companions[i] = va_arg(vargs,UI_Action*);
			i++;
		}
		action->num_of_companions = num_of_companions;
		va_end(vargs);
}

#undef xPercentageOfScreen
#undef yPercentageOfScreen
#undef wPercentageOfScreen
#undef hPercentageOfScreen
#undef xFixedAmount
#undef yFixedAmount
#undef wFixedAmount
#undef hFixedAmount
#undef companionX
#undef companionY
#undef companionW
#undef companionH

/* UIAction_PercPosition

	 This UI_Action allows you to define an element's position by a floating point
	 multiplication of the scren's width/height plus an integer constant.

	 This means you can do things like ensure that elements are always positioned
	 in corners even whent the screen gets resized.

	 Like all size adjusting UI_Actions, other UI_Actions that adjust the size need
	 need to be specified as companions to prevent them arguing over where and how
	 big things should be*/

#define xPercentageOfScreen floats[0]
#define yPercentageOfScreen floats[1]
#define xFixedAmount integers[0]
#define yFixedAmount integers[1]
#define companionX integers[0]
#define companionY integers[1]

int UIAction_PercPosition(UI_Action *action, UIData *uiData){
		SDL_Point point;
		int i = 0;
		if(action->status == 1){
			point = getPointFromPerc(uiData->graphicsData->window,
													                             action->xPercentageOfScreen,
													                             action->yPercentageOfScreen);
			action->element->rect.x = point.x + action->xFixedAmount;
			action->element->rect.y = point.y + action->yFixedAmount;
			while(i < action->num_of_companions){
				action->companions[i]->companionX = point.x + action->xFixedAmount;
				action->companions[i]->companionY = point.y + action->yFixedAmount;
				i++;
			}
			return 1;
		}
		return 0;
}

void UIConfigure_PercPosition(UI_Element *element, UI_Action *action, double xPerc, double yPerc,
	                                                                    int xFixed, int yFixed,
																																			int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_PercPosition;
	action->floats = calloc(2,sizeof(double));
	action->xPercentageOfScreen = xPerc;
	action->yPercentageOfScreen = yPerc;
	action->num_of_floats = 2;
	action->integers = calloc(2,sizeof(int));
	action->xFixedAmount = xFixed;
	action->yFixedAmount = yFixed;
	action->num_of_integers = 2;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->num_of_companions = num_of_companions;
	va_end(vargs);
}

#undef xPercentageOfScreen
#undef yPercentageOfScreen
#undef xFixedAmount
#undef yFixedAmount
#undef companionX
#undef companionY

/* UIAction_CalculateScrollListOffset

   This UI_Action is very specialised and only really works in conjuction with some
	 other UI_Actions. It gets the number of pixels the scroll list needs to have
	 scrolled to be properly reflective of where the scroll bar is */

#define scrollListOffset integers[0]
#define totalPossibleOffset integers[1]

int UIAction_CalculateScrollListOffset(UI_Action *action, UIData *uiData){
	(void)uiData;
	if(action->status == 1){
		if(action->totalPossibleOffset <= action->element->rect.h){
			action->scrollListOffset = 0;
			return 1;
		}
		/* Remind to make this clearer, at the moment it's rather bizarre */
		action->scrollListOffset = -(double)((double)action->integers[0]/(double)action->element->rect.h) * (action->totalPossibleOffset - action->element->rect.h);
		return 1;
	}
	return 0;
}

void UIConfigure_CalculateScrollListOffset(UI_Element *element, UI_Action *action, int totalOffset){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_CalculateScrollListOffset;
	action->integers = calloc(3, sizeof(int));
	action->scrollListOffset = 0;
	action->totalPossibleOffset = totalOffset;
	action->num_of_integers = 2;
}

#undef scrollListOffset
#undef totalPossibleOffset

/* UIAction_ShrinkFitToParentWithYShift

   What this UI_Action does is shrink the element if the parent is smaller. It also gets an
	 additional offset from a companion, which allows scrollbars to controlled correctly.

	 This is used to make the scroll bar behave itself and not go outside of its parent
	 whilst also disappearing if the AI editor is minimise. I don't think it'll be particularly
	 useful in any other situation */


#define xPosition integers[0]
#define yPosition integers[1]
#define xDimension integers[2]
#define yDimension integers[3]

#define yShiftSource companions[0]
#define yShiftValue integers[0]

int UIAction_ShrinkFitToParentWithYShift(UI_Action *action, UIData *uiData){
	SDL_Rect temp_rect;
	SDL_Rect *parent_rect = &action->element->parent->rect;
	(void)uiData;
	if(action->status == 1){
			temp_rect.x = action->xPosition;
			temp_rect.y = action->yPosition + action->yShiftSource->yShiftValue;
			temp_rect.w = action->xDimension;
			temp_rect.h = action->yDimension;
			/* returns 1 if the rectangle is visible otherwise returns 0 */
			if(!testRectIntersection(temp_rect,*parent_rect)){
				/* It's outside of the limit, make it 0 */
				temp_rect.w = 0;
				temp_rect.h = 0;
			}
			else{
				if(temp_rect.x < parent_rect->x){
					temp_rect.x = parent_rect->x;
				}
				if(temp_rect.x + temp_rect.w > parent_rect->x + parent_rect->w){
					temp_rect.w = (parent_rect->x + parent_rect->w) - temp_rect.x;
				}

				if(temp_rect.y < parent_rect->y){
					temp_rect.h -= parent_rect->y - temp_rect.y;
					temp_rect.y = parent_rect->y;
				}
				else if(temp_rect.y + temp_rect.h > parent_rect->y + parent_rect->h){
					temp_rect.h -= temp_rect.y + temp_rect.h - parent_rect->y - parent_rect->h;
				}
			}
			action->element->rect = temp_rect;
			return 1;
	}
	return 0;
}

void UIConfigure_ShrinkFitToParentWithYShift(UI_Element *element, UI_Action *action, UI_Action *yShiftSourceElement){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_ShrinkFitToParentWithYShift;
	action->integers = calloc(4,(sizeof(int)));
	action->xPosition = element->rect.x;
	action->yPosition = element->rect.y;
	action->xDimension = element->rect.w;
	action->yDimension = element->rect.h;
	action->companions = malloc(sizeof(UI_Action*));
	action->yShiftSource = yShiftSourceElement;
	action->num_of_companions = 1;
}

#define xPosition integers[0]
#define yPosition integers[1]
#define xDimension integers[2]
#define yDimension integers[3]
#define yShiftSource companions[0]
#define yShiftValue integers[0]

/* UIAction_GetDifferenceInChildYOffset

   This UI_Action gets the difference between where a child element of the element
	 which this action is attached to is now and where it was created, in the y direction
	 and writes this into a collection of companions.

	 This is used when making a scroll bar to determine how far down this list we
	 have scrolled */

#define yPercentageOffset floats[0]
#define yIntegerOffset integers[0]
#define childYOffset integers[1]

int UIAction_GetDifferenceInChildYOffset(UI_Action *action, UIData *uiData){
	GraphicsData *graphicsData;
	int i=0,win_x,win_y,base_offset;
	if(action->status == 1 && action->external != NULL){
		graphicsData = uiData->graphicsData;
		SDL_GetWindowSize(graphicsData->window,&win_x,&win_y);

		/* We need to know what the current base offset would be so that we can get a
		   appropriate offset that adjusts when the window is resized */
		base_offset = (int)(win_y * action->yPercentageOffset) + action->yIntegerOffset;

		action->childYOffset = base_offset - action->external->rect.y;
		while(i < action->num_of_companions){
			/* I haven't given this a name, because it's potentially used for a variety
			   of things. It's only important to note that any action used as a companion here
				 *MUST* have the space for this or we travel to segfault land */
			action->companions[i]->integers[0] = -action->childYOffset;
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_GetDifferenceInChildYOffset(UI_Element *element, UI_Action *action, double yPercOffset, int yIntOffset, UI_Element *child, int num_of_companions, ...){
		int i = 0;
		va_list vargs;
		va_start(vargs,num_of_companions);
		UIAction_Init(element,action);
		action->response = UPDATE;
		action->function = UIAction_GetDifferenceInChildYOffset;
		action->num_of_companions = num_of_companions;
		action->integers = malloc(2*sizeof(int));
		action->yIntegerOffset = yIntOffset;
		/* This might be set to 0 here, but it gets refreshed every frame so it's not
		   going to stay 0 for very long */
		action->childYOffset = 0;
		action->num_of_integers = 2;
		action->floats = malloc(sizeof(double));
		action->yPercentageOffset = yPercOffset;
		action->num_of_floats = 1;
		action->companions = calloc(num_of_companions, sizeof(UI_Action*));
		while(i < num_of_companions){
			action->companions[i] = va_arg(vargs,UI_Action*);
			i++;
		}
		action->external = child;
		va_end(vargs);
}

#undef yPercentageOffset
#undef yIntegerOffset
#undef childYOffset

/* UIAction_DraggableVerticalOverride

   This UI_Action when active causes it's UI_Element to follow your mouse only
	 in the y direction.

	 It's used with the Scrollbar to make it only go up and down.

	 As with all size or position altering actions, it needs to have other size or
	 position altering actions as companions to avoid conflicting changes*/

#define companionY integers[1]

int UIAction_DraggableVerticalOverride(UI_Action *action, UIData *uiData){
	SDL_Event *event;
	int y,i=0;
	event = uiData->event;

	if(action->status == 1){
		y = event->motion.y;

		if(y < action->element->parent->rect.y){
			y = action->element->parent->rect.y;
		}
		else if(y + action->element->rect.h > action->element->parent->rect.y + action->element->parent->rect.h){
			y = action->element->parent->rect.y + action->element->parent->rect.h - action->element->rect.h;
		}

	  action->element->rect.y = y;
		while(i < action->num_of_companions){
			action->companions[i]->companionY = y;
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_DraggableVerticalOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = MOTION;
	action->function = UIAction_DraggableVerticalOverride;
	action->status = 0;
	action->new_status = 0;
	action->num_of_companions = num_of_companions;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

#undef companionY

/* UIAction_NullifyAI

	 Simply put, this makes the currently loaded AI do nothing, so you're bees will
	 also just stop thinking. Unless given other orders, they'll just fly in a straight
	 line */

int UIAction_NullifyAI(UI_Action *action, UIData *uiData){
	AIData *aiData;
	if(action->status == 1){
		aiData = uiData->aiData;
		nullifyBlockFunctionRoot(&aiData->blockFunctionRoots[0],uiData->gameObjectData);
		action->new_status = 0;
		return 1;
	}
	return 0;
}

void UIConfigure_NullifyAI(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_NullifyAI;
	action->status = 0;
	action->new_status = 0;
}

/* UIAction_RecallWorkers

	 This forces all alive workers to return to the hive. If the AI is not nullified,
	 then you will probably have some of them do something else as the AI still commands them */

int UIAction_RecallWorkers(UI_Action *action, UIData *uiData){
	GameObjectData *gameObjectData;
	ProgrammableWorker *programmableWorker;
	if(action->status == 1){
		gameObjectData = uiData->gameObjectData;
		programmableWorker = gameObjectData->first_programmable_worker;
		while(programmableWorker != NULL){
			programmableWorker->status = RETURNING;
			programmableWorker->heading = getAngleBetweenRects(&gameObjectData->hive.rect,&programmableWorker->rect);
			programmableWorker = programmableWorker->next;
		}
		action->new_status = 0;
		return 1;
	}
	return 0;
}

void UIConfigure_RecallWorkers(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_RecallWorkers;
	action->status = 0;
	action->new_status = 0;
}

/* UIAction_ReadAiBlocks

	 This UI_Action makes an BlockFunctionRoot from with various bits of data extracted
	 from it's children. It's long and complicated and needs it's children to have the
	 UIAction_SetUpAiBlock all active on them which pushes the data into a space that
	 this can read from */

#define infoOuput companions[0]
#define changeString strings[1]

int UIAction_ReadAiBlocks(UI_Action *action, UIData *uiData){
	char *workingSpace;
	char *childElementExposedString;
	AIData *aiData;
	BlockFunctionRoot blockFunctionRoot;
	BlockFunction *erroneousBlockFunction;
	int i;
	UI_Element *childElement;
	int childCount;
	#ifdef DUMP_AI_TO_FILE
	FILE *aiDump;
	#endif
	if(action->status == 1){
		childElement = action->element->child;
		childCount = 0;
		workingSpace = calloc(1,sizeof(char));
		aiData = uiData->aiData;

		i = 0;
		while(childElement != NULL){
			childCount++;
			assert(childElement->exposed_data_count > 0);
			assert(childElement->exposed_data_types[0] == UI_STRING);
			childElementExposedString = ((char**)childElement->exposed_data)[0];
			workingSpace = realloc(workingSpace,1+strlen(workingSpace)+strlen(childElementExposedString));
			strcat(workingSpace,childElementExposedString);
			childElement = childElement->sibling;
		}
		#ifdef DUMP_AI_TO_FILE
		aiDump = fopen("AI_Output.bpp","w");
		fprintf(aiDump,workingSpace);
		fflush(aiDump);
		fclose(aiDump);
		#endif
		/* This function carves up the workingSpace string */
		blockFunctionRoot = makeBlockFunctionRootFromString(uiData->aiData,workingSpace,childCount);
	    erroneousBlockFunction = testBlockFunctionRootForLoops(uiData->aiData,&blockFunctionRoot.blockFunctions[0],NULL,0);
	    if(erroneousBlockFunction != NULL){
			while(erroneousBlockFunction != &blockFunctionRoot.blockFunctions[i]){
			  i++;
			}
			if(action->infoOuput != NULL){
				action->infoOuput->changeString = realloc(action->infoOuput->changeString,strlen("ERROR: Loop detected at Block ###: \'BlockFunctionGenericNameThatMakesSureWeHaveSpace\'")+1);
				sprintf(action->infoOuput->changeString,"ERROR: Loop detected at Block %d: \'%s\'",i,erroneousBlockFunction->name);
			}
			/* THIS FUNCTION BEING ONE LINE HIGHER CAUSED A SEGFAULT DO NOT TOUCH IT */
			nullifyBlockFunctionRoot(&blockFunctionRoot,NULL);
	    }
	    else if(testBlockFunctionRootForStart(&blockFunctionRoot) == 0){
				nullifyBlockFunctionRoot(&blockFunctionRoot, NULL);
				action->infoOuput->changeString = realloc(action->infoOuput->changeString,strlen("ERROR: No block linked to START")+1);
				sprintf(action->infoOuput->changeString,"ERROR: No block linked to START");
	    }
	    else{
				action->infoOuput->changeString = realloc(action->infoOuput->changeString,strlen("Compilation Complete")+1);
				strcpy(action->infoOuput->changeString, "Compilation Complete");
				nullifyBlockFunctionRoot(&aiData->blockFunctionRoots[0],uiData->gameObjectData);
				aiData->blockFunctionRoots[0] = blockFunctionRoot;
	    }
		action->new_status = 0;
		free(workingSpace);
	}
	return action->status;
}

void UIConfigure_ReadAiBlocks(UI_Element *element, UI_Action *action, UI_Action *infoOutput){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_ReadAiBlocks;
	action->status = 0;
	action->new_status = 0;
	action->companions = malloc(sizeof(void*));
	action->infoOuput = infoOutput;
	action->num_of_companions = 1;
}

#undef infoOutput
#undef changeString

/* UIAction_SetUpAiBlock

	 This takes the various bits of data and pushes it into a space where another
	 UI_Element can read it all.

	 I'm not going to do a full documentation for this because I'm thinking I'm going to change it */

int UIAction_SetUpAiBlock(UI_Action *action, UIData *uiData){
	int stringLength = 1;
	char workingSpace[30] = {0};
	(void)uiData;
	if(action->status == 1){
		action->element->exposed_data_types = malloc(sizeof(enum UIDataTypes));
		action->element->exposed_data_types[0] = UI_STRING;

		action->element->exposed_data_count = 1;

		action->element->exposed_data = calloc(1,sizeof(char*));
		action->element->exposed_data[0] = calloc(stringLength,sizeof(char));

		/* action->companions[0].strings[0] This should be the action holding the string */
		stringLength = stringLength + 1 + strlen(action->companions[0]->strings[0]);
		action->element->exposed_data[0] = realloc(action->element->exposed_data[0],
		                                           stringLength);
		strcat(action->element->exposed_data[0],action->companions[0]->strings[0]);
		strcat(action->element->exposed_data[0],"\n");
		/* This should be the action holding the primary link */
		if(action->num_of_companions > 1 && action->companions[1] != NULL && action->companions[1]->external != NULL){
			stringLength = stringLength + 17;
			action->element->exposed_data[0] = realloc(action->element->exposed_data[0],
																								 stringLength);
			sprintf(workingSpace,"\tprimary = %d\n",action->companions[1]->external->sibling_position + 1);
			strcat(action->element->exposed_data[0],workingSpace);
		}
		/* This should be the action holding the secondary link */
		if(action->num_of_companions > 2 && action->companions[2] != NULL && action->companions[2]->external != NULL){
			stringLength = stringLength + 19;
			action->element->exposed_data[0] = realloc(action->element->exposed_data[0],
																								 stringLength);
			sprintf(workingSpace,"\tsecondary = %d\n",action->companions[2]->external->sibling_position + 1);
			strcat(action->element->exposed_data[0],workingSpace);
		}
		if(action->num_of_companions > 3 && action->companions[3] != NULL){
			stringLength = stringLength + 18;
			action->element->exposed_data[0] = realloc(action->element->exposed_data[0],
																								 stringLength);
			sprintf(workingSpace,"\tchars = %c\n",action->companions[3]->strings[0][0]);
			strcat(action->element->exposed_data[0],workingSpace);
		}
		if(action->num_of_companions > 4 && action->companions[4] != NULL){
			stringLength = stringLength + 18;
			action->element->exposed_data[0] = realloc(action->element->exposed_data[0],
																								 stringLength);
			sprintf(workingSpace,"\tintegers = %s\n",action->companions[4]->strings[0]);
			strcat(action->element->exposed_data[0],workingSpace);
		}
		return 1;
	}
	return 0;
}

void UIConfigure_SetUpAiBlock(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = UPDATE;
	action->function = UIAction_SetUpAiBlock;
	action->num_of_companions = num_of_companions;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

/* UIAction_DeleteKeyFlagDestroy

   This should be replaced with a less specific method */

int UIAction_DeleteKeyFlagDestroy(UI_Action *action, UIData *uiData){
	int x,y;
	(void)uiData;
	if(action->status == 1){
		SDL_GetMouseState(&x,&y);
		if(x < action->element->rect.x || x > action->element->rect.x + action->element->rect.w){
		  return 0;
		}
		if(y < action->element->rect.y || y > action->element->rect.y + action->element->rect.h){
		  return 0;
		}
		action->element->destroy = 1;
		return 1;
	}
	return 0;
}

void UIConfigure_DeleteKeyFlagDestroy(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = RESPONSE_DELETE;
	action->function = UIAction_DeleteKeyFlagDestroy;
}

/* UIAction_AddAiBlock

	 This adds a new AI block styled element to a target element, based off one of
	 the templates from AIData. It's very specific to the situation it's in, so I
	 don't see it being useful anywhere else*/

#define linkedTemplate extra

int UIAction_AddAiBlock(UI_Action *action, UIData *uiData){
	(void)uiData;
	BlockFunctionTemplate *template;
	if(action->status == 1){
		template = action->linkedTemplate;
		makeAIBlock(action->external->rect.x,action->external->rect.y,template,action->external);
		action->new_status = 0;
		return 1;
	}
	return 0;
}

void UIConfigure_AddAiBlock(UI_Element *element, UI_Action *action, BlockFunctionTemplate *template, UI_Element *target){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_AddAiBlock;
	action->external = target;
	action->status = 0;
	action->new_status = 0;
	action->linkedTemplate = template;
}

#undef template

/* UIAction_DisplayImage

   Simply takes an image, and renders inside the UI_Element's rect */

int UIAction_DisplayImage(UI_Action *action, UIData *uiData){
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;

	if(action->status == 1){
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&action->element->rect);
		return 1;
	}
	return 0;
}

void UIConfigure_DisplayImage(UI_Element *element, UI_Action *action, SDL_Texture *image){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_DisplayImage;
	action->texture = image;
}

/* UIAction_Auto

	 Simply runs all attached triggers once its activated.

	 Takes a specified Response */

int UIAction_Auto(UI_Action *action, UIData *uiData){
	(void)uiData;
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

void UIConfigure_Auto(UI_Element *element, UI_Action *action, enum Response response){
	UIAction_Init(element,action);
	action->response = response;
	action->function = UIAction_Auto;
}

/* UIAction_ShrinkFitToParent

	 This UI_Action ensures that an element never extends outside of the rectangle
	 of it's parent, by making it smaller so that it always stays inside.

	 The main use for it at the moment is hiding parts of the AI editor when it's
	 in the small mode */

#define xPosition integers[0]
#define yPosition integers[1]
#define xDimension integers[2]
#define yDimension integers[3]

int UIAction_ShrinkFitToParent(UI_Action *action, UIData *uiData){
	SDL_Rect temp_rect;
	SDL_Rect *parent_rect = &action->element->parent->rect;
	(void)uiData;
	if(action->status == 1){
			temp_rect.x = action->xPosition;
			temp_rect.y = action->yPosition;
			temp_rect.w = action->xDimension;
			temp_rect.h = action->yDimension;
			/* returns 1 if the rectangle is visible otherwise returns 0 */
			if(!testRectIntersection(temp_rect,*parent_rect)){
				/* It's outside of the limit, make it 0 */
				temp_rect.w = 0;
				temp_rect.h = 0;
			}
			else{
				if(temp_rect.x < parent_rect->x){
					temp_rect.x = parent_rect->x;
				}
				if(temp_rect.x + temp_rect.w > parent_rect->x + parent_rect->w){
					temp_rect.w = (parent_rect->x + parent_rect->w) - temp_rect.x;
				}

				if(temp_rect.y < parent_rect->y){
					temp_rect.h -= parent_rect->y - temp_rect.y;
					temp_rect.y = parent_rect->y;
				}
				else if(temp_rect.y + temp_rect.h > parent_rect->y + parent_rect->h){
					temp_rect.h -= temp_rect.y + temp_rect.h - parent_rect->y - parent_rect->h;
				}
			}
			action->element->rect = temp_rect;
			return 1;
	}
	return 0;
}

void UIConfigure_ShrinkFitToParent(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_ShrinkFitToParent;
	action->integers = calloc(4,(sizeof(int)));
	action->xPosition = element->rect.x;
	action->yPosition= element->rect.y;
	action->xDimension = element->rect.w;
	action->yDimension = element->rect.h;
}

#undef xPosition
#undef yPosition
#undef xDimension
#undef yDimension

/* UIAction_DisplayNumber

	 This UI_Action displays a number onto the screen. The number can be modified
	 using sUIAction_DisplayNumber_EditNumber(). Alignment of the number can be
	 specified when configuring it. Font is using a index to the array of fonts in
	 GraphicsData */

#define newNumber integers[0]
#define currentNumber integers[1]
#define fontID integers[2]
#define alignStyle integers[3]
#define alignOffset integers[4]
#define numberAsString strings[0]

/* static void sUIAction_DisplayNumber_EditNumber(UI_Action *action, int editNumber){
	assert(action->function == UIAction_DisplayNumber);
	action->newNumber = editNumber;
} */

int UIAction_DisplayNumber(UI_Action *action, UIData *uiData){
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	int w,h;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	graphicsData = uiData->graphicsData;
	if(action->newNumber != action->currentNumber){
		action->currentNumber = action->newNumber;
		sprintf(action->numberAsString,"%d",action->currentNumber);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Blended(graphicsData->fonts[action->fontID], action->numberAsString, colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
		SDL_FreeSurface(temp);
		TTF_SizeText(graphicsData->fonts[action->fontID],action->numberAsString,&w,&h);
		action->alignOffset = w;

	}
	if(UIElement_isVisible(action->element) && action->element->rect.h > TTF_FontHeight(graphicsData->fonts[action->fontID]) && action->status != 0 && action->texture != NULL){
		if(action->alignStyle == UISTRING_ALIGN_CENTER){
			temp_rect.x = action->element->rect.x + action->element->rect.w/2 - action->alignOffset/2;
		}
		else if(action->alignStyle == UISTRING_ALIGN_RIGHT){
			temp_rect.x = action->element->rect.x + action->element->rect.w - action->alignOffset;
		}
		else{
			temp_rect.x = action->element->rect.x;
		}
		temp_rect.y = action->element->rect.y;
		TTF_SizeText(graphicsData->fonts[action->fontID], action->strings[0], &temp_rect.w, &temp_rect.h);
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
		return 1;
	}
	return 0;
}

void UIConfigure_DisplayNumber(UI_Element *element, UI_Action *action, int number, int font, enum UIString_Align align){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->status = 1;
	action->new_status = 1;
	action->function = UIAction_DisplayNumber;
	action->integers = calloc(5, sizeof(int));
	action->newNumber = number;
	action->currentNumber = font;
	action->alignStyle = align;
	action->num_of_integers = 5;
	action->strings = malloc(sizeof(char*) * 1);
	action->numberAsString = calloc(30,sizeof(char));
	action->num_of_strings = 1;
}

#undef newNumber
#undef currentNumber
#undef fontID
#undef alignStyle
#undef alignOffset
#undef numberAsString

/* UIAction_DisplayString

	 This displays a string onto the screen. The string can be modified
	 using sUIAction_DisplayNumber_EditNumber(). Alignment of the number can be
	 specified when configuring it. Font is using a index to the array of fonts in
	 GraphicsData */

#define currentString strings[0]
#define newString strings[1]
#define fontID integers[0]
#define alignStyle integers[1]
#define alignOffset integers[3]

static void sUIAction_DisplayString_EditString(UI_Action *action, char *editString){
	assert(action->function == UIAction_DisplayString || action->function == UIAction_DisplayStringSubrect);
	if(strcmp(action->newString,editString) != 0){
		action->newString = realloc(action->newString,strlen(editString)+1);
		strcpy(action->newString,editString);
	}
}

int UIAction_DisplayString(UI_Action *action, UIData *uiData){
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	int w,h;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	graphicsData = uiData->graphicsData;
	if(action->currentString != NULL && action->newString != NULL && strcmp(action->currentString,action->newString) != 0){
		action->currentString = realloc(action->currentString, strlen(action->newString)+1);
		strcpy(action->currentString,action->newString);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Blended(graphicsData->fonts[action->fontID],action->currentString,colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
		SDL_FreeSurface(temp);
		TTF_SizeText(graphicsData->fonts[action->fontID],action->currentString,&w,&h);
		action->alignOffset = w;
	}
	if(UIElement_isVisible(action->element) && action->element->rect.h > TTF_FontHeight(graphicsData->fonts[action->fontID]) && action->currentString != NULL && action->status != 0 && action->texture != NULL){
		if(action->alignStyle == UISTRING_ALIGN_CENTER){
			temp_rect.x = action->element->rect.x + action->element->rect.w/2 - action->alignOffset/2;
		}
		else if(action->alignStyle == UISTRING_ALIGN_RIGHT){
			temp_rect.x = action->element->rect.x + action->element->rect.w - action->alignOffset;
		}
		else{
			temp_rect.x = action->element->rect.x;
		}
		temp_rect.y = action->element->rect.y;
		TTF_SizeText(graphicsData->fonts[action->fontID], action->currentString, &temp_rect.w, &temp_rect.h);
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
		return 1;
	}
	return 0;
}

void UIConfigure_DisplayString(UI_Element *element, UI_Action *action, char *string, int font, enum UIString_Align align){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_DisplayString;
	action->strings = malloc(sizeof(char*) * 2);
	if(string != NULL){
		action->currentString = calloc(strlen(string)+1,sizeof(char));
		action->newString = calloc(strlen(string)+1,sizeof(char));
		strcpy(action->newString, string);
	}
	else{
		action->currentString = NULL;
		action->newString = NULL;
	}
	action->new_status = 1;
	action->status = 1;
	action->num_of_strings = 2;
	action->integers = calloc(4,sizeof(int));
	action->fontID = font;
	action->alignStyle = align;
	action->num_of_integers = 4;
}

#undef currentString
#undef newString
#undef fontID
#undef alignStyle
#undef alignOffset

/* UIAction_ResourceCounter

	 This takes the value of the number of flowers collected in our hive and puts
	 it into a companions data set. It's used for the source counter (or was, there's
   a newer more detailed version done by Alex that I think doesn't use this directly)

	 I'm not going to fully document this because I think it needs to go and be
	 replaced with something else */

int UIAction_ResourceCounter(UI_Action *action, UIData *uiData){
	int i = 0;
	GameObjectData *gameObjectData;
	gameObjectData = uiData->gameObjectData;
	if(action->status == 1){
		action->integers[0] = gameObjectData->hive.flowers_collected;
		while(i < action->num_of_companions){
			action->companions[i]->integers[0] = action->integers[0];
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_ResourceCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = UPDATE;
	action->function = UIAction_ResourceCounter;
	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->integers = calloc(1, sizeof(int));
	action->num_of_integers = 1;
	va_end(vargs);
}

/* UIAction_DaysCounter

	 This UI_Action stores the number of days remaining until winter and passes it
	 to all it's companions. It's used to implement the days till winter counter */

#define dayCount integers[0]

int UIAction_DaysCounter(UI_Action *action, UIData *uiData){
	int i = 0;
	GameObjectData *gameObjectData;
	gameObjectData = uiData->gameObjectData;
	if(action->status == 1){
		action->dayCount = gameObjectData->environment.winterCountdown;
		while(i < action->num_of_companions){
			action->companions[i]->dayCount = action->dayCount;
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_DaysCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = UPDATE;
	action->function = UIAction_DaysCounter;
	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->integers = calloc(1, sizeof(int));
	action->num_of_integers = 1;
	va_end(vargs);
}

#undef dayCount

/* UIAction_YearsCounter

	 This counts how many years the hive has survived for. It's used as part of the
	 new score system */

#define yearCount integers[0]

int UIAction_YearsCounter(UI_Action *action, UIData *uiData){
	int i = 0;
	GameObjectData *gameObjectData;
	gameObjectData = uiData->gameObjectData;
	if(action->status == 1){
		action->yearCount = gameObjectData->years_survived;
		while(i < action->num_of_companions){
			action->companions[i]->yearCount = action->yearCount;
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_YearsCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = UPDATE;
	action->function = UIAction_YearsCounter;
	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->integers = calloc(1, sizeof(int));
	action->num_of_integers = 1;
	va_end(vargs);
}

/* UIAction_Counter

	 This UI_Action simply acts as a counter - use it with a trigger that uses UITRIGGER_PLUSONE
	 to make it count something like how many times something has been clicked.

	 If it gets used, the count will reset. To disable this, give it the NONE response */

int UIAction_Counter(UI_Action *action, UIData *uiData){
	int i = 0;
	(void)uiData;
	if(action->status > action->integers[0]){
		action->status = 1;
		action->new_status = 1;
	}
	while(i < action->num_of_companions){
		action->companions[i]->integers[0] = action->status;
		i++;
	}
	return 1;
}

void UIConfigure_Counter(UI_Element *element, UI_Action *action, int maximum, int num_of_companions, ...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_Counter;
	action->status = 0;
	action->new_status = 0;
	action->integers = calloc(1,sizeof(int));
	action->integers[0] = maximum;
	action->companions = calloc(num_of_companions,sizeof(void*));
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

/* UIAction_Renderline

	 This probably the longest UI_Action. It has several states where it will draw
	 lines.

	 Status 0 is OFF - it won't draw a line at all.
	 Status 1 is FREEHAND - it draws to the point stored in drawToX, drawToY
	 Status 2 is LINK - it draws a line between two UI_Elements
	 Status 3 is TEST - it makes it check that it can link to something and switchs
	                    to OFF or LINKED as appropriate
	status 4 is RESET - it will make the link reset to OFF and clear up any data

	It can also be set to draw from different locations - at the moment only corners
	but I will be changing that */

#define OFF 0
#define FREEHAND 1
#define LINK 2
#define TEST 3
#define RESET 4
#define drawPosX integers[0]
#define drawPosY integers[1]
#define drawFromX integers[2]
#define drawFromY integers[3]

int UIAction_RenderLine(UI_Action *action, UIData *uiData){
	int bx,by;
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(!UIElement_isVisible(action->element) || (action->external != NULL && !UIElement_isVisible(action->external))){
		return 0;
	}
	switch(action->status){
		case TEST:
			if(action->external == NULL || isLinked(action->external,action->element)){
				action->new_status = 0;
				action->external = NULL;
				action->drawPosX = action->element->rect.x;
				action->drawPosY = action->element->rect.y;
			}
			else{
				action->new_status = 2;
			}
			return 1;
		case FREEHAND:
			SDL_SetRenderDrawColor(graphicsData->renderer,0,
											0,
											0,
											255);
			SDL_RenderDrawLine(graphicsData->renderer,
											   action->element->rect.x+action->drawFromX,
										     action->element->rect.y+action->drawFromY,
										     action->drawPosX,
											   action->drawPosY);
			return 1;
		case LINK:
			if(action->external == NULL){
				action->new_status = 0;
				action->external = NULL;
				action->drawPosX = action->element->rect.x;
				action->drawPosY = action->element->rect.y;
				return 0;
			}
			SDL_SetRenderDrawColor(graphicsData->renderer,0,
											0,
											0,
											255);
			bx = action->external->rect.x + action->element->rect.w/2;
			by = action->external->rect.y;
			SDL_RenderDrawLine(graphicsData->renderer,action->element->rect.x+action->drawFromX,action->element->rect.y+action->drawFromY,
										bx,by);
			return 1;
		case RESET:
			action->external = NULL;
			action->new_status = 0;
			action->drawPosX = action->element->rect.x;
			action->drawPosY = action->element->rect.y;
			return 0;
	}
	return 0;
}

void UIConfigure_RenderLine(UI_Element *element, UI_Action *action, enum LineOrigins origin, UI_Element *external){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_RenderLine;
	action->external = external;
	action->status = 2;
	action->new_status = 2;
	action->integers = calloc(4,sizeof(int));
	action->num_of_integers = 4;

	action->drawPosX = element->rect.x;
	action->drawPosY = element->rect.y;
	switch(origin){
		case CENTER:
			action->drawFromX = (element->rect.w/2);
			action->drawFromY = (element->rect.h/2);
			break;
		case BL_CORNER:
			action->drawFromX = 0;
			action->drawFromY = element->rect.h;
			break;
		case BR_CORNER:
			action->drawFromX = element->rect.w;
			action->drawFromY = element->rect.h;
			break;
		case TL_CORNER:
			action->drawFromX = 0;
			action->drawFromY = 0;
			break;
		case TR_CORNER:
			action->drawFromX = element->rect.w;
			action->drawFromY = 0;
			break;
	}
}

#undef OFF
#undef FREEHAND
#undef LINK
#undef TEST
#undef RESET
#undef drawPosX
#undef drawPosY
#undef drawFromX
#undef drawFromY

/* UIAction_ClickAnywhere

	 This UI_Action simply executes it's bound triggers on a mouse click anywhere.
	 There are two UIConfigures for this action, each binding to Left or Right click */

int UIAction_ClickAnywhere(UI_Action *action, UIData *uiData){
	(void)uiData;
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

void UIConfigure_LeftClickAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickAnywhere;
}

void UIConfigure_RightClickAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickAnywhere;
}

/* UIAction_ReleaseAnywhere

	 This UI_Action simply executes it's bound triggers when the mouse is released
	 anywhere. There are two UIConfigure functions for this action, each attaching
	 to the Left or Right mouse buttons.*/

int UIAction_ReleaseAnywhere(UI_Action *action, UIData *uiData){
	(void)uiData;
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

void UIConfigure_LeftReleaseAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = LEFT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
}

void UIConfigure_RightReleaseAnywhere(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = RIGHT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
}

/* UIAction_CalculateSibling

	 This UI_Action calculates what sibling (as in, UI_Element with the same parent
   as this UI_Actions's element) overlaps with the position stored in testX and testY
	 and then writes that in as an external reference for it's companions.

	 I used it when I was doing all the work to get the AI blocks to link together.
	 I don't think it's a particularly useful action outside of that */

#define testX integers[0]
#define testY integers[1]

int UIAction_CalculateSibling(UI_Action *action, UIData *uiData){
	UI_Element *element;
	int i = 0;
	(void)uiData;
	if(action->status == 1){
		/* Ignore the first sibling, because that's the start block */
		element = action->element->parent->child->sibling;
		while(element != NULL){
			if(element != action->element){
				if(!(action->testX < element->rect.x || action->testX > element->rect.x + element->rect.w) &&
					 !(action->testY < element->rect.y || action->testY > element->rect.y + element->rect.h)){
						break;
					 }
			}
			element = element->sibling;
		}

		while(i < action->num_of_companions){
			action->companions[i]->external = element;
			i++;
		}
		action->new_status = 0;
		/* Make sure that our integers are something that won't get confusing if we're not actually using this action.
		   This is to prevent the links snapping back to where they were if we click too quickly */
		action->testX = action->element->rect.x;
		action->testY = action->element->rect.y;
	}
	return 1;
}

void UIConfigure_CalculateSibling(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = UPDATE;
	action->function = UIAction_CalculateSibling;

	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->num_of_companions = num_of_companions;

	action->status = 0;
	action->new_status = 0;
	action->integers = calloc(2,sizeof(int));
	action->num_of_integers = 2;
	va_end(vargs);
}

#undef testX
#undef testY

/* UIAction_StoreMousePosition

	 This UI_Action simply stores the position of the mouse for its companions.

	 I used it to make the lines on the AI editor follow the mouse, and to work out
	 what blocks to link together */

#define mousePosX integers[0]
#define mousePosY integers[1]

int UIAction_StoreMousePosition(UI_Action *action, UIData *uiData){
	SDL_Event *event;
	int i = 0;
	event = uiData->event;
	if(action->status == 1){
		action->mousePosX = event->motion.x;
		action->mousePosY = event->motion.y;
		while(i < action->num_of_companions){
			action->companions[i]->mousePosX = action->mousePosX;
			action->companions[i]->mousePosY = action->mousePosY;
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_StoreMousePosition(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = MOTION;
	action->function = UIAction_StoreMousePosition;

	action->companions = malloc(sizeof(UI_Action*) * num_of_companions);
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->num_of_companions = num_of_companions;

	action->status = 0;
	action->new_status = 0;
	action->integers = calloc(2,sizeof(int));
	action->num_of_integers = 2;
	va_end(vargs);
}

#undef mousePosX
#undef mousePosY

/* UIAction_FillRect

   Simply paints the whole rectangle of a UI_Element in a single colour */

#define red integers[0]
#define green integers[1]
#define blue integers[2]

int UIAction_FillRect(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1){
		SDL_SetRenderDrawColor(graphicsData->renderer,
			                     action->red,
		                       action->green,
		                       action->blue,
		                       255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		return 1;
	}
	return 0;
}

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_FillRect;
	action->integers = malloc(sizeof(int) * 3);
	action->num_of_integers = 3;

	action->red = r;
	action->green = g;
	action->blue = b;
}
#undef red
#undef green
#undef blue

/* UIAction_FillAndBorderRect

   Fills the inside of a UI_Element's rectangle with a given colour and then draws
	 a border in a separate one (looks much nicer!) */

#define fillRed integers[0]
#define fillGreen integers[1]
#define fillBlue integers[2]
#define borderRed integers[3]
#define borderGreen integers[4]
#define borderBlue integers[5]

int UIAction_FillAndBorderRect(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1 && UIElement_isVisible(action->element)){
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->fillRed,
		                       action->fillGreen,
		                       action->fillBlue,
		                       255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,
			                     action->borderRed,
													 action->borderGreen,
													 action->borderBlue,
													 255);
		SDL_RenderDrawRect(graphicsData->renderer,&action->element->rect);


		return 1;
	}
	return 0;
}

int UIAction_DrawScrollhandle(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1 && UIElement_isVisible(action->element)){
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->fillRed,
													 action->fillGreen,
													 action->fillBlue,
													 255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		/* A border is already drawn into my scrollhandle graphic, so the code for drawing a border in UIAction_FillAndBorderRect() is removed. */
		/* SDL_SetTextureBlendMode() blends the overlaying colours of the FillRect down onto those of the greyscale image I provide. */
		SDL_SetTextureBlendMode(graphicsData->uiEle.graphic[SCROLLHANDLE_GRAPHIC],
														SDL_BLENDMODE_MOD);
		SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[SCROLLHANDLE_GRAPHIC], NULL, &action->element->rect);

		return 1;
	}
	return 0;
}

int UIAction_DrawCrossbox(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1 && UIElement_isVisible(action->element)){
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->fillRed,
													 action->fillGreen,
													 action->fillBlue,
													 255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->borderRed,
													 action->borderGreen,
													 action->borderBlue,
													 255);
		SDL_RenderDrawRect(graphicsData->renderer,&action->element->rect);

		/* SDL_SetTextureBlendMode() blends the overlaying colours of the FillRect down onto those of the greyscale image I provide. */
		SDL_SetTextureBlendMode(graphicsData->uiEle.graphic[CROSSBOX_GRAPHIC],
														SDL_BLENDMODE_MOD);
		SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[CROSSBOX_GRAPHIC], NULL, &action->element->rect);

		return 1;
	}
	return 0;
}

int UIAction_DrawCompilebox(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1 && UIElement_isVisible(action->element)){
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->fillRed,
													 action->fillGreen,
													 action->fillBlue,
													 255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->borderRed,
													 action->borderGreen,
													 action->borderBlue,
													 255);
		SDL_RenderDrawRect(graphicsData->renderer,&action->element->rect);

		/* SDL_SetTextureBlendMode() blends the overlaying colours of the FillRect down onto those of the greyscale image I provide. */
		SDL_SetTextureBlendMode(graphicsData->uiEle.graphic[COMPILEBOX_GRAPHIC],
														SDL_BLENDMODE_MOD);
		SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[COMPILEBOX_GRAPHIC], NULL, &action->element->rect);

		return 1;
	}
	return 0;
}

int UIAction_DrawStopbox(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1 && UIElement_isVisible(action->element)){
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->fillRed,
													 action->fillGreen,
													 action->fillBlue,
													 255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->borderRed,
													 action->borderGreen,
													 action->borderBlue,
													 255);
		SDL_RenderDrawRect(graphicsData->renderer,&action->element->rect);

		/* SDL_SetTextureBlendMode() blends the overlaying colours of the FillRect down onto those of the greyscale image I provide. */
		SDL_SetTextureBlendMode(graphicsData->uiEle.graphic[STOP_GRAPHIC],
														SDL_BLENDMODE_MOD);
		SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[STOP_GRAPHIC], NULL, &action->element->rect);

		return 1;
	}
	return 0;
}

int UIAction_DrawBlock(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1 && UIElement_isVisible(action->element)){
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->fillRed,
													 action->fillGreen,
													 action->fillBlue,
													 255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,
													 action->borderRed,
													 action->borderGreen,
													 action->borderBlue,
													 255);
		SDL_RenderDrawRect(graphicsData->renderer,&action->element->rect);

		/* SDL_SetTextureBlendMode() blends the overlaying colours of the FillRect down onto those of the greyscale image I provide. */
		SDL_SetTextureBlendMode(graphicsData->uiEle.graphic[BLOCK_GRAPHIC],
														SDL_BLENDMODE_MOD);
		SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[BLOCK_GRAPHIC], NULL, &action->element->rect);

		return 1;
	}
	return 0;
}

int UIAction_DrawHivecell(UI_Action *action, UIData *uiData){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	graphicsData = uiData->graphicsData;
	if(action->status == 1 && UIElement_isVisible(action->element)){
//    SDL_SetTextureBlendMode(graphicsData->uiEle.graphic[HIVECELL_GRAPHIC], SDL_BLENDMODE_MOD);
		SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[HIVECELL_GRAPHIC], NULL, &action->element->rect);
//		SDL_RenderCopy(graphicsData->renderer, graphicsData->uiEle.graphic[HIVECELLMASK_GRAPHIC], NULL, &action->element->rect);

		return 1;
	}
	return 0;
}

void UIConfigure_FillAndBorderRect(UI_Element *element, UI_Action *action, int fr, int fg, int fb, int br, int bg, int bb, UIElement_Variety variety){
	UIAction_Init(element,action);
	action->response = UPDATE;
	/* Here is where UIAction_FillAndBorderRect is called. */
	switch(variety){
		case HIVECELL:
      action->function = UIAction_DrawHivecell;
      break;
    case STOPBOX:
      action->function = UIAction_DrawStopbox;
      break;
		case COMPILEBOX:
			action->function = UIAction_DrawCompilebox;
			break;
    case CROSSBOX:
			action->function = UIAction_DrawCrossbox;
			break;
		case SCROLLHANDLE:
			action->function = UIAction_DrawScrollhandle;
			break;
		default:
			action->function = UIAction_FillAndBorderRect;
	}
	action->integers = malloc(sizeof(int) * 6);
	action->num_of_integers = 6;

	action->fillRed = fr;
	action->fillGreen = fg;
	action->fillBlue = fb;
	action->borderRed = br;
	action->borderGreen = bg;
	action->borderBlue = bb;
}

#undef fillRed
#undef fillGreen
#undef fillBlue
#undef borderRed
#undef borderGreen
#undef borderBlue

/* UIAction_ClickRect

	 Simply executes its triggers if a click is detected inside its UI_Element.
	 Has 2 UIConfigures, for left and right clicks */

int UIAction_ClickRect(UI_Action *action, UIData *uiData){
	/* This should get a SDL_Event pointer */
	SDL_Event *event;
	event = uiData->event;

	if(action->status == 0){return 0;}
	if(event->button.x < action->element->rect.x || event->button.x > action->element->rect.x + action->element->rect.w){
	  return 0;
	}
	if(event->button.y < action->element->rect.y || event->button.y > action->element->rect.y + action->element->rect.h){
	  return 0;
	}
	UITrigger_Execute(action);
	return 1;
}

void UIConfigure_LeftClickRect(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickRect;
}

void UIConfigure_RightClickRect(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickRect;
}

/* UIAction_DraggableRectOverride

   Whilst active, this causes the UI_Element to follow the mouse cursor, whilst
	 remaining inside its parent's rectangle.

	 Like other size altering actions, it needs to get companion access to actions
	 that also modify its UI_Element's shape to prevent conflicts over positioning*/

int UIAction_DraggableRectOverride(UI_Action *action, UIData *uiData){
	SDL_Event *event;
	int x,y,i=0;
	event = uiData->event;

	/* UIACTION_INTS is the status, 0 = rooted, 1 = grabbed */

	if(action->status == 1){
		x = event->motion.x - action->element->rect.w/2;
		y = event->motion.y - action->element->rect.h/2;
		if(x < action->element->parent->rect.x){
			x = action->element->parent->rect.x;
		}
		else if(x + action->element->rect.w > action->element->parent->rect.x + action->element->parent->rect.w){
			x = action->element->parent->rect.x + action->element->parent->rect.w - action->element->rect.w;
		}

		if(y < action->element->parent->rect.y){
			y = action->element->parent->rect.y;
		}
		else if(y + action->element->rect.h > action->element->parent->rect.y + action->element->parent->rect.h){
			y = action->element->parent->rect.y + action->element->parent->rect.h - action->element->rect.h;
		}

	  action->element->rect.x = x;
	  action->element->rect.y = y;
		while(i < action->num_of_companions){
			action->companions[i]->integers[0] = x;
			action->companions[i]->integers[1] = y;
			i++;
		}
		return 1;
	}
	return 0;
}

void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = MOTION;
	action->function = UIAction_DraggableRectOverride;
	action->status = 0;
	action->new_status = 0;
	action->num_of_companions = num_of_companions;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

/* UIAction_TwoRectOverride

   This action makes a UI_Element have two different sized and positioned rectangles
	 which it can transition between */

#define smallX integers[0]
#define smallY integers[1]
#define smallW integers[2]
#define smallH integers[3]
#define bigX integers[4]
#define bigY integers[5]
#define bigW integers[6]
#define bigH integers[7]
#define counter integers[8]
#define transitionTime integers[9]

int UIAction_TwoRectOverride(UI_Action *action, UIData *uiData){
	int dt;
	double transition_multiplier;
	dt = *uiData->ticks;

	if(action->status == 0){
		action->element->rect.x = action->smallX;
		action->element->rect.y = action->smallY;
		action->element->rect.w = action->smallW;
		action->element->rect.h = action->smallH;
		return 1;
	}
	else if(action->status == 1){
	  /* We're going big! */
	  action->counter+=dt;
	  if(action->counter > action->transitionTime){
	    /* We've become big */
	    action->new_status = 3;
	    action->counter = action->transitionTime;
	  }
	  transition_multiplier = ((double) action->counter)/((double) action->transitionTime);
	  action->element->rect.x = action->smallX + (action->bigX - action->smallX) * transition_multiplier;
	  action->element->rect.y = action->smallY + (action->bigY - action->smallY) * transition_multiplier;
	  action->element->rect.w = action->smallW + (action->bigW - action->smallW) * transition_multiplier;
	  action->element->rect.h = action->smallH + (action->bigH - action->smallH) * transition_multiplier;
	  return 1;
	}
	else if(action->status == 2){
	  /* We're going small! */
	  action->counter-=dt;
	  if(action->counter < 0){
	    /* We've become small */
	    action->new_status = 0;
	    action->counter = 0;
	  }
	  transition_multiplier = ((double) action->counter)/((double) action->transitionTime);
	  action->element->rect.x = action->smallX + (action->bigX - action->smallX) * transition_multiplier;
	  action->element->rect.y = action->smallY + (action->bigY - action->smallY) * transition_multiplier;
	  action->element->rect.w = action->smallW + (action->bigW - action->smallW) * transition_multiplier;
	  action->element->rect.h = action->smallH + (action->bigH - action->smallH) * transition_multiplier;
	  return 1;
	}
	else if(action->status == 3){
		action->element->rect.x = action->bigX;
		action->element->rect.y = action->bigY;
		action->element->rect.w = action->bigW;
		action->element->rect.h = action->bigH;
		return 1;
	}
  return 0;
}

void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action, int sx, int sy, int sw, int sh, int bx, int by, int bw, int bh, int delay){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_TwoRectOverride;
	action->element = element;
	action->status = 0;
	action->new_status = 0;
	action->integers = malloc(sizeof(int) * 10);
	action->num_of_integers = 10;
	/* Small */
	action->smallX = sx;
	action->smallY = sy;
	action->smallW = sw;
	action->smallH = sh;
	/* Big */
	action->bigX = bx;
	action->bigY = by;
	action->bigW = bw;
	action->bigH = bh;
	/* State  0 = SMALL*/
	/* Time */
	action->counter = 0;
	/* Transition time */
	action->transitionTime = delay;
}

#undef smallX
#undef smallY
#undef smallW
#undef smallH
#undef bigX
#undef bigY
#undef bigW
#undef bigH
#undef counter
#undef transitionTime

/* UIAction_UpdateTwoRectOverrideOnWindowResize

	 This abomination of a function is basically just two merged together versions
	 of UIAction_PercOffsetRect that allows you to define a TwoRectOverride in terms
	 of the screen size. It's necessary, but rather bloated (just look at all those
   arguments!)*/

#define percBigX floats[0]
#define percBigY floats[1]
#define percBigW floats[2]
#define percBigH floats[3]
#define percSmallX floats[4]
#define percSmallY floats[5]
#define percSmallW floats[6]
#define percSmallH floats[7]
#define fixedBigX integers[0]
#define fixedBigY integers[1]
#define fixedBigH integers[2]
#define fixedBigW integers[3]
#define fixedSmallX integers[4]
#define fixedSmallY integers[5]
#define fixedSmallW integers[6]
#define fixedSmallH integers[7]

#define companionSmallX integers[0]
#define companionSmallY integers[1]
#define companionSmallW integers[2]
#define companionSmallH integers[3]
#define companionBigX integers[4]
#define companionBigY integers[5]
#define companionBigW integers[6]
#define companionBigH integers[7]

int UIAction_UpdateTwoRectOverrideOnWindowResize(UI_Action *action, UIData *uiData){
		SDL_Point point;
		if(action->status == 1){
			point = getPointFromPerc(uiData->graphicsData->window,
													                             action->percBigX,
													                             action->percBigY);
			action->companions[0]->companionBigX = point.x + action->fixedBigX;
			action->companions[0]->companionBigY = point.y + action->fixedBigY;

			point = getPointFromPerc(uiData->graphicsData->window,
		                           action->percBigW,
														   action->percBigH);
			action->companions[0]->companionBigW = point.x + action->fixedBigW;
			action->companions[0]->companionBigH = point.y + action->fixedBigH;

			point = getPointFromPerc(uiData->graphicsData->window,
													                             action->percSmallX,
													                             action->percSmallY);
			action->companions[0]->companionSmallX = point.x + action->fixedSmallX;
			action->companions[0]->companionSmallY = point.y + action->fixedSmallY;

			point = getPointFromPerc(uiData->graphicsData->window,
		                           action->percSmallW,
														   action->percSmallH);
			action->companions[0]->companionSmallW = point.x + action->fixedSmallW;
			action->companions[0]->companionSmallH = point.y + action->fixedSmallH;

			return 1;

	}
	return 0;
}
/* These argument names are horrible, but they stand for Big/Small X/Y Int/Float Point/Dimension */
void UIConfigure_UpdateTwoRectOverrideOnWindowResize(UI_Element *element, UI_Action *action,
UI_Action *twoRectOverride, int bxip, int byip, double bxfp, double byfp, int bxid, int byid, double bxfd, double byfd,
int sxip, int syip, double sxfp, double syfp, int sxid, int syid, double sxfd, double syfd){
	UIAction_Init(element,action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_UpdateTwoRectOverrideOnWindowResize;
	action->integers = malloc(sizeof(int) * 8);
	action->num_of_integers = 8;
	action->fixedBigX = bxip;
	action->fixedBigY = byip;
	action->fixedBigW = bxid;
	action->fixedBigH = byid;
	action->fixedSmallX = sxip;
	action->fixedSmallY = syip;
	action->fixedSmallW = sxid;
	action->fixedSmallH = syid;

	action->floats = malloc(sizeof(double) * 8);
	action->num_of_floats = 8;
	action->percBigX = bxfp;
	action->percBigY = byfp;
	action->percBigW = bxfd;
	action->percBigH = byfd;
	action->percSmallX = sxfp;
	action->percSmallY = syfp;
	action->percSmallW = sxfd;
	action->percSmallH = syfd;
	action->companions = malloc(sizeof(void*));
	action->companions[0] = twoRectOverride;
	action->num_of_companions = 1;
}

#undef percBigX
#undef percBigY
#undef percBigW
#undef percBigH
#undef percSmallX
#undef percSmallY
#undef percSmallW
#undef percSmallH
#undef fixedBigX
#undef fixedBigY
#undef fixedBigH
#undef fixedBigW
#undef fixedSmallX
#undef fixedSmallY
#undef fixedSmallW
#undef fixedSmallH

#undef companionSmallX
#undef companionSmallY
#undef companionSmallW
#undef companionSmallH
#undef companionBigX
#undef companionBigY
#undef companionBigW
#undef companionBigH

static void UITrigger_Execute(UI_Action *action){
	UI_Trigger *trigger = action->triggers;
	while(trigger!=NULL){
		if(trigger->action != NULL){
			if(trigger->status_from == -1 || trigger->action->status == trigger->status_from){
				if(trigger->status_to == UITRIGGER_PLUSONE){
					trigger->action->new_status = trigger->action->status+1;
				}
				else{
					trigger->action->new_status = trigger->status_to;
				}
			}
		}
		trigger = trigger->next;
	}
}

void UIElement_Free(UI_Element *element){
	int i = 0;
	UI_Element *root = element;
	while(root->parent != NULL){
		root = root->parent;
	}
	UIElement_Deparent(element);
	/* Clean out all references to this element */
	UIElement_DeExternalise(root,element);
	while(i < element->num_of_actions){
	  UIAction_Free(&element->actions[i]);
	  i++;
	}
	UIElement_RemoveExposedData(element);
	free(element->actions);
	free(element);
}

static void UIAction_Free(UI_Action *action){
	int i = 0;
	while(action->triggers!=NULL){
		UITrigger_Destroy(action,action->triggers);
	}

	while(i < action->num_of_strings){
		if(action->strings[i] != NULL){
			free(action->strings[i]);
		}
		i++;
	}
	if(action->strings != NULL){
		free(action->strings);
	}
	if(action->companions != NULL){
		free(action->companions);
	}
	if(action->integers != NULL){
		free(action->integers);
	}
	if(action->floats != NULL){
		free(action->floats);
	}

	return;
}

void UIElement_Reparent(UI_Element *element, UI_Element *parent){
	UI_Element *child = parent->child;
	element->sibling_position = 0;
	if(element->parent != NULL){
		UIElement_Deparent(element);
	}
	if(child == NULL){
		parent->child = element;
	}
	else{
		element->sibling_position++;
		while(child->sibling != NULL){
			element->sibling_position++;
			child = child->sibling;
		}
		child->sibling = element;
	}
	element->parent = parent;
}

void UIElement_Deparent(UI_Element *element){
	UI_Element *parent = element->parent;
	UI_Element *child;
	UI_Element *sibling;
	int i;
	if(parent==NULL){
		return;
	}
	child = parent->child;
	if(child == element){
		parent->child = element->sibling;
	}
	else{
		while(child->sibling != NULL){
			if(child->sibling == element){
				child->sibling = element->sibling;
				break;
			}
		child = child->sibling;
		}
	}
	sibling = element->parent->child;
	i = 0;
	while(sibling != NULL){
		sibling->sibling_position = i;
		sibling = sibling->sibling;
		i++;
	}
	element->parent = NULL;
}

static void UIElement_DeExternalise(UI_Element *root, UI_Element *external){
	/* Cleans all external references in actions to a given element,
		 preventing segfaults */
	int i = 0;
	UI_Trigger *trigger;
	UI_Trigger *trigger2;
	if(root != external){
		while(i < root->num_of_actions){
			if(root->actions[i].external == external){
				root->actions[i].external = NULL;
				trigger = root->actions[i].triggers;
				while(trigger != NULL){
					trigger2 = trigger->next;
					if(trigger->action->element == external){
						trigger->action = NULL;
					}
					trigger = trigger2;
				}
			}
			i++;
		}
	}
	root = root->child;
	/* Suddenly, recursion */
	while(root != NULL){
		UIElement_DeExternalise(root,external);
		root = root->sibling;
	}
}

static void UITrigger_Destroy(UI_Action *action, UI_Trigger *trigger){
	UI_Trigger *next = trigger->next;
	UI_Trigger *prev = action->triggers;

	assert(prev != NULL);

	if(prev != trigger){
		while(prev->next != trigger){
			prev = prev->next;
		}
		prev->next = next;
	}
	else{
		action->triggers = next;
	}
	free(trigger);
}

int UIElement_Execute(UI_Element *element, UIData *uiData, enum Response response){
	int i = 0;
	int ret = 0;
	int result = 0;
	while(i < element->num_of_actions){
		if(element->actions[i].response == response){
			result = element->actions[i].function(&element->actions[i],uiData);
			ret = result || ret;
		}
		i++;
	}
	i = 0;
	while(i < element->num_of_actions){
		element->actions[i].status = element->actions[i].new_status;
		i++;
	}
	return ret;
}

static int isLinked(UI_Element *test, UI_Element *to){
	int i = 0;
	while(i < test->num_of_actions){
		if(test->actions[i].function == UIAction_RenderLine && test->actions[i].external == to){
			return 1;
		}
		i++;
	}
	return 0;
}

void UITrigger_Bind(UI_Action *action, UI_Action *target, int status_from, int status_to){
	UI_Trigger *trigger = malloc(sizeof(UI_Trigger));
	UI_Trigger *movingPointer = NULL;

	trigger->action = target;
	trigger->status_from = status_from;
	trigger->status_to = status_to;
	trigger->next = NULL;

	action->num_of_triggers++;

	if(action->triggers == NULL){
		action->triggers = trigger;
	}
	else{
		movingPointer = action->triggers;
		while(movingPointer->next != NULL){
			movingPointer = movingPointer->next;
		}
		movingPointer->next = trigger;
	}
}

void UIRoot_Execute(UIData *uiData, enum Response response, int stopAtFirst){
	int shouldStop = 0;
	UI_Element *queue[255] = {NULL};
	UI_Element *queue_element;
	UI_Element *queue_element_child;
	int front = 0, back = 1;
	queue[0] = uiData->root;

	while(front != back){
		queue_element = queue[front];
		front = (front + 1) % 255;
		if(response != DEBUG){
			shouldStop = UIElement_Execute(queue_element,uiData,response);
		}
		else{
			UIElement_Debug(queue_element);
		}
		if(shouldStop && stopAtFirst){
			break;
		}
		queue_element_child = queue_element->child;
		while(queue_element_child != NULL){
		  queue[back] = queue_element_child;
		  back = (back + 1) % 255;
		  queue_element_child = queue_element_child->sibling;
		}
	}
}

/* Does a depth based traversal of the tree backwards (i.e. going from the deepest layer to
   the top layer) */
void UIRoot_ExecuteUpwards(UIData *uiData, enum Response response, int stopAtFirst){
	static int guesstimated_queue_length = 2;
	UI_Element **queue = calloc(guesstimated_queue_length,sizeof(UI_Element*));
	UI_Element *queue_element;
	UI_Element *queue_element_child;
	int front = 0,back = 1;
	queue[0] = uiData->root;
	assert(queue!=NULL);
	while(queue[front] != NULL && front != back){
		queue_element = queue[front];
		front = (front + 1);
		queue_element_child = queue_element->child;
		while(queue_element_child != NULL){
			if(back == guesstimated_queue_length){
				guesstimated_queue_length = back+1;
				queue = realloc(queue,guesstimated_queue_length * sizeof(UI_Element*));
			}
			queue[back] = queue_element_child;
			back = (back + 1);
			queue_element_child = queue_element_child->sibling;
		}
	}
	if(back < guesstimated_queue_length){
		guesstimated_queue_length = back + 1;
	}
	while(back > 0){
		back--;
		queue_element = queue[back];
		/* We have to do our disposal here because doing it in a top down traversal
		   of the tree would break things */
		if(response == FREED){
			UIElement_Free(queue_element);
		}
		if(response == DISPOSAL && queue_element->destroy){
			UIElement_Free(queue_element);
		}
		else{
			if(UIElement_Execute(queue_element,uiData,response) && stopAtFirst){
				break;
			}
		}
	}
	free(queue);
 }

static void UIAction_Init(UI_Element *element, UI_Action *action){
	action->response = NONE;
	action->function = NULL;
	action->element = element;
	action->external = NULL;
	action->status = 1;
	action->new_status = 1;
	action->companions = NULL;
	action->num_of_companions = 0;
	action->integers = NULL;
	action->triggers = NULL;
	action->num_of_triggers = 0;
	action->strings = NULL;
	action->num_of_strings = 0;
	action->texture = NULL;
	action->floats = NULL;
	action->num_of_floats = 0;
	action->extra = NULL;
}

static int UIElement_isVisible(UI_Element *element){
	return element->rect.w && element->rect.h;
}

void UIRoot_Destroy(UIData *uiData){
	UIRoot_ExecuteUpwards(uiData,FREED,0);
}

void UIRoot_Pack(UIData *uiData){
	UIRoot_Execute(uiData,WINDOW_RESIZE,0);
}

static void UIElement_RemoveExposedData(UI_Element *element){
	/* I'm going to do some hax with void pointers here */
	int i = 0;
	void *thingToFree;
	if(element->exposed_data_types!=NULL){
		thingToFree = element->exposed_data_types;
		free(thingToFree);
	}
	while(i < element->exposed_data_count){
		if(element->exposed_data[i] != NULL){
			thingToFree = element->exposed_data[i];
			free(thingToFree);
		}
		i++;
	}
	if(element->exposed_data != NULL){
		thingToFree = element->exposed_data;
		free(thingToFree);
	}
	element->exposed_data_types = NULL;
	element->exposed_data = NULL;
	element->exposed_data_count = 0;
}

void initUIData(UIData *uiData){
	uiData->graphicsData = NULL;
	uiData->gameObjectData = NULL;
	uiData->aiData = NULL;
	uiData->announcementsData = NULL;
	uiData->event = NULL;
	uiData->ticks = 0;
}

void quickSetStatus(UI_Action *action, int status){
	action->new_status = status;
	action->status = 0;
}

void UIElement_Debug(UI_Element *element){
	int i = 0;
	while(i < element->num_of_actions){
		assert(element->actions[i].function != NULL);
		i++;
	}
}
