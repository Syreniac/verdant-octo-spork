#include "UI.h"


#define TWORECTOVERRIDE_SX integers[0]
#define TWORECTOVERRIDE_SY integers[1]
#define TWORECTOVERRIDE_SW integers[2]
#define TWORECTOVERRIDE_SH integers[3]
#define TWORECTOVERRIDE_BX integers[4]
#define TWORECTOVERRIDE_BY integers[5]
#define TWORECTOVERRIDE_BW integers[6]
#define TWORECTOVERRIDE_BH integers[7]
#define TWORECTOVERRIDE_COUNTER integers[8]
#define TWORECTOVERRIDE_TRANSITION integers[9]

#define RENDERRECT_R integers[0]
#define RENDERRECT_B integers[1]
#define RENDERRECT_G integers[2]

static void UIAction_Init(UI_Element *element, UI_Action *action);
static void UIAction_Free(UI_Action *action);
static void UITrigger_Execute(UI_Action *action);
static int isLinked(UI_Element *test, UI_Element *to);
static int UIElement_isVisible(UI_Element *element);
static int percWindowDimension(int dimension, float perc);
static void UIElement_DeExternalise(UI_Element *root, UI_Element *external);
static void UITrigger_Destroy(UI_Action *action, UI_Trigger *trigger);
static void UIElement_RemoveExposedData(UI_Element *element);

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
int UIAction_NullifyAI(UI_Action *action, va_list copy_from);

UI_Element *makeStartBlock(int x_offset, int y_offset, UI_Element *parent){
	UI_Element *element;
	element = UIElement_Create(x_offset,y_offset,200,50,9);
	UIConfigure_FillAndBorderRect(element,&element->actions[0],248,221,35,0,0,0);
	UIConfigure_ShrinkFitToParent(element, &element->actions[1]);
	UIConfigure_DisplayString(element, &element->actions[2],"START",0,UISTRING_ALIGN_CENTER);
	UIConfigure_RenderLine(element, &element->actions[3],BR_CORNER,NULL);

	UIConfigure_LeftClickRect(element, &element->actions[4]);
		UITrigger_Bind(&element->actions[4],&element->actions[3],-1,1);
		UITrigger_Bind(&element->actions[4],&element->actions[4],1,0);
		UITrigger_Bind(&element->actions[4],&element->actions[5],0,1);
		UITrigger_Bind(&element->actions[4],&element->actions[6],0,1);

	UIConfigure_LeftReleaseAnywhere(element, &element->actions[5]);
		element->actions[5].status = 0;
		element->actions[5].new_status = 0;
		UITrigger_Bind(&element->actions[5],&element->actions[3],1,2);
		UITrigger_Bind(&element->actions[5],&element->actions[4],0,1);
		UITrigger_Bind(&element->actions[5],&element->actions[7],0,1);
		UITrigger_Bind(&element->actions[5],&element->actions[6],0,1);
		UITrigger_Bind(&element->actions[5],&element->actions[5],1,0);

	UIConfigure_StoreMousePosition(element, &element->actions[6],2,&element->actions[3],&element->actions[7]);
		element->actions[6].status = 0;
		element->actions[6].new_status = 0;
	UIConfigure_CalculateSibling(element, &element->actions[7],1,&element->actions[3]);
		element->actions[7].status = 0;
		element->actions[7].new_status = 0;
	UIConfigure_SetUpAiBlock(element,&element->actions[8],2,&element->actions[2],&element->actions[3]);
	UIElement_Reparent(element,parent);
	return element;
}

UI_Element *makeAIResetButton(int x_offset, int y_offset, UI_Element *parent){
	UI_Element *element;
	element = UIElement_Create(x_offset,y_offset,200,50,6);
	UIConfigure_FillAndBorderRect(element,&element->actions[0],74,74,74,0,0,0);
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
	element = UIElement_Create(x_offset, y_offset, 220, 360,6);
	UIConfigure_FillAndBorderRect(element,&element->actions[0],185,122,87,0,0,0);
	UIConfigure_ShrinkFitToParent(element,&element->actions[1]);
	/* Set up the slider bar */
		element2 = UIElement_Create(x_offset+200,y_offset,21,20,7);
		UIConfigure_FillAndBorderRect(element2,&element2->actions[0],249,252,124,0,0,0);
		UIConfigure_ShrinkFitToParent(element2,&element2->actions[1]);
		UIConfigure_DraggableVerticalOverride(element2,&element2->actions[2],1,&element2->actions[1]);
		UIConfigure_LeftClickRect(element2,&element2->actions[3]);
			UITrigger_Bind(&element2->actions[3],&element2->actions[2],0,1);
			UITrigger_Bind(&element2->actions[3],&element2->actions[4],0,1);
		UIConfigure_LeftReleaseAnywhere(element2,&element2->actions[4]);
			UITrigger_Bind(&element2->actions[4],&element2->actions[2],1,0);
			UITrigger_Bind(&element2->actions[4],&element2->actions[4],1,0);
			UIElement_Reparent(element2,element);
		UIConfigure_PercPositionV(element2,&element2->actions[5],1.0,0.0,-x_offset+199,y_offset,1,&element2->actions[1]);
		UIConfigure_SlideWithMouseWheel(element2,&element2->actions[6],0,1,1,&element2->actions[1]);
			element2->actions[6].response = NONE;
		/* Set up the main panel again */
	UIConfigure_GetDifferenceInChildYOffset(element,&element->actions[2],0.0,y_offset,element2,1,&element->actions[3]);
	UIConfigure_PassThrough(element,&element->actions[5],MOUSEWHEEL,1,&element2->actions[6]);
	/* Set up the options */
	while(template!=NULL){
		element2 = UIElement_Create(x_offset, y_offset + y_offset2, 200,50,6);
		UIConfigure_FillAndBorderRect(element2,&element2->actions[0],248,221,35,0,0,0);
		UIConfigure_ShrinkFitToParentWithYShift(element2,&element2->actions[1],&element->actions[3]);
		UIConfigure_DisplayString(element2,&element2->actions[2],template->name,0,UISTRING_ALIGN_CENTER);
		UIConfigure_LeftClickRect(element2,&element2->actions[3]);
			UITrigger_Bind(&element2->actions[3],&element2->actions[4],-1,1);
		UIConfigure_AddAiBlock(element2,&element2->actions[4],template,blockHolder);
		UIConfigure_PercPosition(element2,&element2->actions[5],1.0,0.0,-x_offset,y_offset+y_offset2,1,&element2->actions[1]);
		UIElement_Reparent(element2,element);
		template = template->next;
		y_offset2 += 60;
	}
	UIConfigure_CalculateScrollListOffset(element,&element->actions[3],y_offset2+60);
	UIConfigure_PercOffsetRect(element,&element->actions[4],1.0,0.0,1.0,1.0,-x_offset,y_offset,-50,-50,1,&element->actions[1]);
	UIElement_Reparent(element,parent);
	return element;
}

UI_Element *makeAIBlock(int x_offset, int y_offset, char *aiString, UI_Element *parent){
	UI_Element *element2;
	element2 = UIElement_Create(x_offset,y_offset,200,50,19);
	UIConfigure_FillAndBorderRect(element2,&element2->actions[0],248,221,35,0,0,0);
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
	printf("|%s|\n",aiString);
	UIConfigure_DisplayString(element2, &element2->actions[5],aiString,0,UISTRING_ALIGN_CENTER);

	/* Line 1 */
	UIConfigure_RenderLine(element2, &element2->actions[6],BL_CORNER,NULL);
	UIConfigure_LeftClickRect(element2, &element2->actions[7]);
		element2->actions[7].status = 1;
		element2->actions[7].new_status = 1;
		UITrigger_Bind(&element2->actions[7],&element2->actions[6],-1,1);
		UITrigger_Bind(&element2->actions[7],&element2->actions[7],1,0);
		UITrigger_Bind(&element2->actions[7],&element2->actions[8],0,1);
		UITrigger_Bind(&element2->actions[7],&element2->actions[10],0,1);
	UIConfigure_StoreMousePosition(element2, &element2->actions[8],2,&element2->actions[6],&element2->actions[9]);
		element2->actions[8].status = 0;
		element2->actions[8].new_status = 0;
	UIConfigure_CalculateSibling(element2, &element2->actions[9],1,&element2->actions[6]);
		element2->actions[9].status = 0;
		element2->actions[9].new_status = 0;
	UIConfigure_LeftReleaseAnywhere(element2, &element2->actions[10]);
		element2->actions[10].status = 0;
		element2->actions[10].new_status = 0;
		UITrigger_Bind(&element2->actions[10],&element2->actions[6],1,2);
		UITrigger_Bind(&element2->actions[10],&element2->actions[12],0,1);
		UITrigger_Bind(&element2->actions[10],&element2->actions[8],1,0);
		UITrigger_Bind(&element2->actions[10],&element2->actions[9],0,1);
		UITrigger_Bind(&element2->actions[10],&element2->actions[10],1,0);
		UITrigger_Bind(&element2->actions[10],&element2->actions[18],0,1);

	/* Line 2 */
	UIConfigure_RenderLine(element2, &element2->actions[11],BR_CORNER,NULL);
	UIConfigure_LeftClickRect(element2, &element2->actions[12]);
		element2->actions[12].status = 0;
		element2->actions[12].new_status = 0;
		UITrigger_Bind(&element2->actions[12],&element2->actions[11],-1,1);
		UITrigger_Bind(&element2->actions[12],&element2->actions[12],1,0);
		UITrigger_Bind(&element2->actions[12],&element2->actions[13],0,1);
		UITrigger_Bind(&element2->actions[12],&element2->actions[15],0,1);
	UIConfigure_StoreMousePosition(element2, &element2->actions[13],2,&element2->actions[11],&element2->actions[14]);
		element2->actions[13].status = 0;
		element2->actions[13].new_status = 0;
	UIConfigure_CalculateSibling(element2, &element2->actions[14],1,&element2->actions[11]);
		element2->actions[14].status = 0;
		element2->actions[14].new_status = 0;
	UIConfigure_LeftReleaseAnywhere(element2, &element2->actions[15]);
		element2->actions[15].status = 0;
		element2->actions[15].new_status = 0;
		UITrigger_Bind(&element2->actions[15],&element2->actions[11],1,2);
		UITrigger_Bind(&element2->actions[15],&element2->actions[16],0,1);
		UITrigger_Bind(&element2->actions[15],&element2->actions[13],1,0);
		UITrigger_Bind(&element2->actions[15],&element2->actions[14],0,1);
		UITrigger_Bind(&element2->actions[15],&element2->actions[15],1,0);
		UITrigger_Bind(&element2->actions[15],&element2->actions[18],0,1);

	/* Lines Clear */
	UIConfigure_LeftClickRect(element2, &element2->actions[16]);
		element2->actions[16].status = 0;
		element2->actions[16].new_status = 0;
		UITrigger_Bind(&element2->actions[16],&element2->actions[6],-1,4);
		UITrigger_Bind(&element2->actions[16],&element2->actions[11],-1,4);
		UITrigger_Bind(&element2->actions[16],&element2->actions[7],0,1);
		UITrigger_Bind(&element2->actions[16],&element2->actions[16],1,0);
		UITrigger_Bind(&element2->actions[16],&element2->actions[18],0,1);

	UIConfigure_DeleteKeyFlagDestroy(element2, &element2->actions[17]);
	UIConfigure_SetUpAiBlock(element2,&element2->actions[18],3,&element2->actions[5],&element2->actions[6],&element2->actions[11]);

	UIElement_Reparent(element2,parent);
	return element2;
}

UI_Element *UIElement_CreateByPercentage(float rx, float ry, float rw, float rh, int x, int y, int num_of_actions){
	   UI_Element *element;
		 element  = calloc(1,sizeof(UI_Element));


	   element->rect.x = percWindowDimension(x,rx);
	   element->rect.y = percWindowDimension(y,ry);
	   element->rect.w = percWindowDimension(x,rw);
	   element->rect.h = percWindowDimension(y,rh);
	   element->actions = malloc(sizeof(UI_Action)*num_of_actions);
	   element->num_of_actions = num_of_actions;
	   element->parent = NULL;
	   element->child = NULL;
	   element->sibling = NULL;
	   element->exposed_data_types = NULL;
	   element->exposed_data = NULL;
	   element->exposed_data_count = 0;
	return element;
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

int UIAction_MinimapMouseMove(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	SDL_Point p;
	int x,y;
	if(action->status != 0){
		va_copy(vargs,copy_from);
		graphicsData = va_arg(vargs,GraphicsData*);
		va_end(vargs);
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
	action->response = RENDER;
	action->function = UIAction_MinimapMouseMove;
}

int UIAction_Minimap(UI_Action *action, va_list copy_from){
	va_list vargs;
	GameObjectData *gameObjectData;
	GraphicsData *graphicsData;
	SDL_Point point;
	SDL_Rect rect;
	int i = 0,j = 0;
	double relx, rely;
	ProgrammableWorker *p;
	if(action->status != 0){
		va_copy(vargs, copy_from);
		gameObjectData = va_arg(vargs,GameObjectData*);
		graphicsData = va_arg(vargs,GraphicsData*);
		va_end(vargs);
		/* Then we draw all the game object stuff into our element's rectangle */
		SDL_SetRenderDrawColor(graphicsData->renderer,0xFE,0x2E,0xF7,0xFF);
		while(i < gameObjectData->resourceNodeSpawnerCount){
			j = 0;
			while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
				if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){
					point = getCenterOfRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect);
					relx = (double)point.x / X_SIZE_OF_WORLD;
					rely = (double)point.y / Y_SIZE_OF_WORLD;
					//printf("drawing flower at %lf,%lf\n",relx,rely);
					rect.x = relx*action->element->rect.w+action->element->rect.x;
					rect.y = rely*action->element->rect.h+action->element->rect.y;
					rect.w = ((double)X_SIZE_OF_NODE / X_SIZE_OF_WORLD) * action->element->rect.w;
					rect.h = ((double)Y_SIZE_OF_NODE / Y_SIZE_OF_WORLD) * action->element->rect.h;
					shrinkRectToFit(&rect,&action->element->rect);
					SDL_RenderFillRect(graphicsData->renderer,&rect);
				}
				j++;
			}
			i++;
		}
		SDL_SetRenderDrawColor(graphicsData->renderer,0x0B,0x61,0x0B,255);
		i = 0;
		while(i < NUMBER_OF_TREES){
			point = getCenterOfRect(gameObjectData->tree[i].stumpRect);
			relx = (double)point.x / X_SIZE_OF_WORLD;
			rely = (double)point.y / Y_SIZE_OF_WORLD;
			rect.x = relx*action->element->rect.w+action->element->rect.x;
			rect.y = rely*action->element->rect.h+action->element->rect.y;
			rect.w = ((double)SIZE_OF_TREE/2 / X_SIZE_OF_WORLD) * action->element->rect.w;
			rect.h = ((double)SIZE_OF_TREE/2 / Y_SIZE_OF_WORLD) * action->element->rect.h;
			//printf("drawing flower at %lf,%lf\n",relx,rely);
			shrinkRectToFit(&rect,&action->element->rect);
			SDL_RenderFillRect(graphicsData->renderer,&rect);
			i++;
		}
		SDL_SetRenderDrawColor(graphicsData->renderer,0x8A,0x4B,0x08,0xFF);
		point = getCenterOfRect(gameObjectData->hive.rect);
		relx = (double)point.x / X_SIZE_OF_WORLD;
		rely = (double)point.y / Y_SIZE_OF_WORLD;
		rect.x = relx*action->element->rect.w+action->element->rect.x;
		rect.y = rely*action->element->rect.h+action->element->rect.y;
		rect.w = ((double)X_SIZE_OF_HIVE / X_SIZE_OF_WORLD) * action->element->rect.w;
		rect.h = ((double)Y_SIZE_OF_HIVE / Y_SIZE_OF_WORLD) * action->element->rect.h;
		shrinkRectToFit(&rect,&action->element->rect);
		SDL_RenderFillRect(graphicsData->renderer,&rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,0x00,0x00,0x00,0xFF);
		p = gameObjectData->first_programmable_worker;
		while(p != NULL){
			point = getCenterOfRect(p->rect);
			relx = (double)point.x / X_SIZE_OF_WORLD;
			rely = (double)point.y / Y_SIZE_OF_WORLD;
			rect.x = relx*action->element->rect.w+action->element->rect.x;
			rect.y = rely*action->element->rect.h+action->element->rect.y;
			rect.w = ((double)X_SIZE_OF_WORKER / X_SIZE_OF_WORLD) * action->element->rect.w;
			rect.h = ((double)Y_SIZE_OF_WORKER / Y_SIZE_OF_WORLD) * action->element->rect.h;
			shrinkRectToFit(&rect,&action->element->rect);
			SDL_RenderFillRect(graphicsData->renderer,&rect);
			p = p->next;
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
	action->response = MINIMAP;
	action->function = UIAction_Minimap;
}

int UIAction_SlideWithMouseWheel(UI_Action *action, va_list copy_from){
	va_list vargs;
	int i = 0;
	int x,y;
	SDL_Event *event;
	if(action->status != 0){
		va_copy(vargs, copy_from);
		event = va_arg(vargs,SDL_Event*);
		va_end(vargs);
		printf("mouse wheel did %d,%d\n",event->wheel.x, event->wheel.y);
		while(i < action->num_of_companions){
			if(action->integers[0]!=0){
				x = action->companions[i]->integers[0] + event->wheel.x * -14;

				if(x < action->element->parent->rect.x){
					x = action->element->parent->rect.x;
				}
				else if(x + action->element->rect.h > action->element->parent->rect.x + action->element->parent->rect.w){
					x = action->element->parent->rect.x + action->element->parent->rect.w - action->element->rect.w;
				}
				action->companions[i]->integers[0] = x;
			}
			if(action->integers[1]!=0){
				y = action->companions[i]->integers[1] + event->wheel.y * -14;

				if(y < action->element->parent->rect.y){
					y = action->element->parent->rect.y;
				}
				else if(y + action->element->rect.h > action->element->parent->rect.y + action->element->parent->rect.h){
					y = action->element->parent->rect.y + action->element->parent->rect.h - action->element->rect.h;
				}
				action->companions[i]->integers[1] = y;
			}
			i++;
		}
		va_end(vargs);
		return 1;
	}
	return 0;
}

void UIConfigure_SlideWithMouseWheel(UI_Element *element, UI_Action *action, int x, int y, int num_of_companions,...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = MOUSEWHEEL;
	action->function = UIAction_SlideWithMouseWheel;
	action->integers = calloc(2,sizeof(int));
	action->num_of_integers = 2;
	action->integers[0] = x;
	action->integers[1] = y;
	action->companions = calloc(num_of_companions,sizeof(UI_Action*));
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		printf("adding companion %p\n",action->companions[i]);
		i++;
	}
	va_end(vargs);
}

int UIAction_PassThrough(UI_Action *action, va_list copy_from){
	va_list vargs;
	int i = 0;
	if(action->status != 0){
		va_copy(vargs,copy_from);
		while(i < action->num_of_companions){
			printf("%d\n",action->companions[i]->response);
			action->companions[i]->function(action->companions[i],vargs);
			i++;
		}
		va_end(vargs);
		return 1;
	}
	return 0;
}

void UIConfigure_PassThrough(UI_Element *element, UI_Action *action, enum Response response, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = response;
	action->function = UIAction_PassThrough;
	action->companions = calloc(num_of_companions,sizeof(UI_Action*));
	action->num_of_companions = num_of_companions;
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		printf("adding companion %p\n",action->companions[i]);
		i++;
	}
	va_end(vargs);
}

int UIAction_GetAnnouncement(UI_Action *action, va_list copy_from){
		AnnouncementsData* announcementsData;
		va_list vargs;
		va_copy(vargs,copy_from);
		announcementsData = va_arg(vargs,AnnouncementsData*);
		va_end(vargs);
		if(action->status != 0){
			action->companions[0]->strings[1] = realloc(action->companions[0]->strings[1], strlen(announcementsData->announcement)+1);
			strcpy(action->companions[0]->strings[1], announcementsData->announcement);
		}
		return 0;
}

void UIConfigure_GetAnnouncement(UI_Element *element, UI_Action *action, UI_Action *placeToPut){
	UIAction_Init(element,action);
	action->response = ANNOUNCEMENTS;
	action->function = UIAction_GetAnnouncement;
	action->companions = calloc(1,sizeof(void*));
	action->companions[0] = placeToPut;
}

int UIAction_ToggleObjectSelection(UI_Action *action, va_list copy_from){
	va_list vargs;
	int *integer;
	va_copy(vargs,copy_from);
	integer = va_arg(vargs,int*);
	va_end(vargs);
	if(action->status == 1){
		*integer = !(*integer);
		action->new_status = 0;
	}
	return 0;
}

void UIConfigure_ToggleObjectSelection(UI_Element *element, UI_Action *action){
	UIAction_Init(element,action);
	action->response = CONTROLS;
	action->function = UIAction_ToggleObjectSelection;
	action->status = 0;
	action->new_status = 0;
}

int UIAction_PercOffsetRect(UI_Action *action, va_list copy_from){
		va_list vargs;
		SDL_Event *event;
		SDL_Point point;
		SDL_Point point2;
		int i = 0;
		va_copy(vargs, copy_from);
		event = va_arg(vargs,SDL_Event*);
		va_end(vargs);
		if(action->status == 1){
			point = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
													                             action->floats[0],
													                             action->floats[1]);
			point2 = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
		                            action->floats[2],
															  action->floats[3]);
			action->element->rect.x = point.x + action->integers[0];
			action->element->rect.y = point.y + action->integers[1];
			action->element->rect.w = point2.x + action->integers[2] - point.x - action->integers[0];
			action->element->rect.h = point2.y + action->integers[3] - point.y - action->integers[1];
			while(i < action->num_of_companions){
				action->companions[i]->integers[0] = point.x + action->integers[0];
				action->companions[i]->integers[1] = point.y + action->integers[1];
				action->companions[i]->integers[2] = point2.x + action->integers[2] - point.x - action->integers[0];
				action->companions[i]->integers[3] = point2.y + action->integers[3] - point.y - action->integers[1];
				i++;
			}
			return 1;
		}
		return 0;

}

int UIAction_PercPositionV(UI_Action *action, va_list copy_from){
		va_list vargs;
		SDL_Event *event;
		SDL_Point point;
		int i = 0;
		va_copy(vargs, copy_from);
		event = va_arg(vargs,SDL_Event*);
		va_end(vargs);
		if(action->status == 1){
			point = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
													                             action->floats[0],
													                             action->floats[1]);
			action->element->rect.x = point.x + action->integers[0];
			action->element->rect.y = point.y + action->integers[1];
			while(i < action->num_of_companions){
				action->companions[i]->integers[0] = point.x + action->integers[0];
				action->companions[i]->integers[1] = point.y + action->integers[1];
				i++;
			}
			return 1;
		}
		return 0;
}

int UIAction_PercPosition(UI_Action *action, va_list copy_from){
		va_list vargs;
		SDL_Event *event;
		SDL_Point point;
		int i = 0;
		va_copy(vargs, copy_from);
		event = va_arg(vargs,SDL_Event*);
		va_end(vargs);
		if(action->status == 1){
			point = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
													                             action->floats[0],
													                             action->floats[1]);
			action->element->rect.x = point.x + action->integers[0];
			action->element->rect.y = point.y + action->integers[1];
			while(i < action->num_of_companions){
				action->companions[i]->integers[0] = point.x + action->integers[0];
				action->companions[i]->integers[1] = point.y + action->integers[1];
				i++;
			}
			return 1;
		}
		return 0;
}

int UIAction_InversePosition(UI_Action *action, va_list copy_from){
	va_list vargs;
	SDL_Event *event;
	SDL_Point point;
	va_copy(vargs, copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);
	if(action->status == 1){
		point = getPointFromInvPoint(SDL_GetWindowFromID(event->window.windowID),
												                             action->integers[0],
												                             action->integers[1]);
		action->element->rect.x = point.x;
		action->element->rect.y = point.y;
		return 1;
	}
	return 0;
}

int UIAction_CalculateScrollListOffset(UI_Action *action, va_list copy_from){
	if(action->status == 1){
		if(action->integers[1] <= action->element->rect.h){
			action->integers[0] = 0;
			return 1;
		}
		action->integers[0] = -(float)((float)action->integers[0]/(float)action->element->rect.h) * (action->integers[1] - action->element->rect.h);
		return 1;
	}
	return 0;
}

int UIAction_ShrinkFitToParentWithYShift(UI_Action *action, va_list copy_from){
	SDL_Rect temp_rect;
	SDL_Rect *parent_rect = &action->element->parent->rect;
	if(action->status == 1){
			temp_rect.x = action->integers[0];
			temp_rect.y = action->integers[1] + action->companions[0]->integers[0];
			temp_rect.w = action->integers[2];
			temp_rect.h = action->integers[3];
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

int UIAction_GetDifferenceInChildYOffset(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	int y,i=0,win_x,win_y,base_offset;
	if(action->status == 1 && action->external != NULL){
		va_copy(vargs,copy_from);
		graphicsData = va_arg(vargs,GraphicsData*);
		SDL_GetWindowSize(graphicsData->window,&win_x,&win_y);

		base_offset = (int)(win_y * action->floats[0]) + action->integers[0];


		y = action->external->rect.y - base_offset;
		action->integers[1] = -y;
		while(i < action->num_of_companions){
			action->companions[i]->integers[0] = y;
			i++;
		}
		va_end(vargs);
		return 1;
	}
	return 0;
}

int UIAction_DraggableVerticalOverride(UI_Action *action, va_list copy_from){
	SDL_Event *event;
	int x,y,i=0;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);

	/* UIACTION_INTS is the status, 0 = rooted, 1 = grabbed */

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
			action->companions[i]->integers[1] = y;
			i++;
		}
		return 1;
	}
	return 0;
}

int UIAction_NullifyAI(UI_Action *action, va_list copy_from){
	va_list vargs;
	AIData *aiData;
	if(action->status == 1){
		va_copy(vargs,copy_from);
		aiData = va_arg(vargs,AIData*);
		va_end(vargs);
		nullifyBlockFunctionRoot(&aiData->blockFunctionRoots[0]);
		action->new_status = 0;
		return 1;
	}
	return 0;
}

int UIAction_RecallWorkers(UI_Action *action, va_list copy_from){
	va_list vargs;
	int i = 0;
	GameObjectData *gameObjectData;
	ProgrammableWorker *programmableWorker;
	if(action->status == 1){
		va_copy(vargs, copy_from);
		gameObjectData = va_arg(vargs, GameObjectData*);
		va_end(vargs);
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

int UIAction_ReadAiBlocks(UI_Action *action, va_list copy_from){
	char *workingSpace;
	char *childElementExposedString;
	va_list vargs;
	AIData *aiData;
	BlockFunctionRoot blockFunctionRoot;
	BlockFunction *erroneousBlockFunction;
	int i;
	UI_Element *childElement;
	int childCount;
	if(action->status == 1){
		childElement = action->element->child;
		childCount = 0;
		workingSpace = calloc(1,sizeof(char));
		va_copy(vargs,copy_from);
		aiData = va_arg(vargs,AIData*);
		va_end(vargs);

		i = 0;
		while(childElement != NULL){
			childCount++;
			childElementExposedString = ((char**)childElement->exposed_data)[0];
			assert(childElement->exposed_data_count > 0);
			assert(childElement->exposed_data_types[0] == UI_STRING);
			workingSpace = realloc(workingSpace,1+strlen(workingSpace)+strlen(childElementExposedString));
			strcat(workingSpace,childElementExposedString);
			childElement = childElement->sibling;
		}
		blockFunctionRoot = makeBlockFunctionRootFromString(workingSpace,childCount);
		printf("strlen working space %d\n",strlen(workingSpace));
		free(workingSpace);
	    erroneousBlockFunction = testBlockFunctionRootForLoops(&blockFunctionRoot.blockFunctions[0],NULL,0);
	    if(erroneousBlockFunction != NULL){
			while(erroneousBlockFunction != &blockFunctionRoot.blockFunctions[i]){
			  i++;
			}
			nullifyBlockFunctionRoot(&blockFunctionRoot);
			if(action->companions[0] != NULL){
				action->companions[0]->strings[1] = realloc(action->companions[0]->strings[1],strlen("ERROR: Loop detected at Block ###: \'BlockFunctionGenericNameThatMakesSureWeHaveSpace\'")+1);
				sprintf(action->companions[0]->strings[1],"ERROR: Loop detected at Block %d: \'%s\'",i,erroneousBlockFunction->name);
			}
	    }
	    else if(testBlockFunctionRootForStart(&blockFunctionRoot) == 0){
			nullifyBlockFunctionRoot(&blockFunctionRoot);
			action->companions[0]->strings[1] = realloc(action->companions[0]->strings[1],strlen("ERROR: No block linked to START")+1);
			sprintf(action->companions[0]->strings[1],"ERROR: No block linked to START");
	    }
	    else{
			action->companions[0]->strings[1] = realloc(action->companions[0]->strings[1],strlen("Compilation Complete")+1);
			strcpy(action->companions[0]->strings[1], "Compilation Complete");
			nullifyBlockFunctionRoot(&aiData->blockFunctionRoots[0]);
			aiData->blockFunctionRoots[0] = blockFunctionRoot;
	    }
		action->new_status = 0;
	}
	return action->status;
}

int UIAction_SetUpAiBlock(UI_Action *action, va_list copy_from){
	int i = 0;
	int stringLength = 1;
	char workingSpace[30] = {0};
	char *exposedDataString;
	if(action->status == 1){
		UIElement_RemoveExposedData(action->element);

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
			stringLength = stringLength + 16;
			action->element->exposed_data[0] = realloc(action->element->exposed_data[0],
													stringLength);
			sprintf(workingSpace,"\tprimary = %d\n",action->companions[1]->external->sibling_position + 1);
			strcat(action->element->exposed_data[0],workingSpace);
		}
		/* This should be the action holding the secondary link */
		if(action->num_of_companions > 2 && action->companions[2] != NULL && action->companions[2]->external != NULL){
			stringLength = stringLength + 18;
			action->element->exposed_data[0] = realloc(action->element->exposed_data[0],
													stringLength);
			sprintf(workingSpace,"\tsecondary = %d\n",action->companions[2]->external->sibling_position + 1);
			strcat(action->element->exposed_data[0],workingSpace);
		}
		return 1;
	}
	return 0;
}

int UIAction_DeleteKeyFlagDestroy(UI_Action *action, va_list copy_from){
	int x,y;
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

int UIAction_AddAiBlock(UI_Action *action, va_list copy_from){
	va_list vargs;
	UI_Element *element2;
	BlockFunctionTemplate *template;
	char c = 0;
	char aiString[30];
	if(action->status == 1){
		template = action->extra;
		makeAIBlock(action->external->rect.x,action->external->rect.y,template->name,action->external);
		action->new_status = 0;
		return 1;
	}
	return 0;
}

int UIAction_PercRect(UI_Action *action, va_list copy_from){
	va_list vargs;
	SDL_Event *event;
	va_copy(vargs, copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);
	if(action->status == 1){
		action->element->rect = getRectFromPercRect(SDL_GetWindowFromID(event->window.windowID),
												   action->floats[0],
												   action->floats[1],
												   action->floats[2],
												   action->floats[3]);
		return 1;
	}
	return 0;
}

int UIAction_InverseRect(UI_Action *action, va_list copy_from){
	va_list vargs;
	int i = 0;
	SDL_Event *event;
	va_copy(vargs, copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);
	if(action->status == 1){
		action->element->rect = getRectFromInvRect(SDL_GetWindowFromID(event->window.windowID),
												                       action->integers[0],
												                       action->integers[1],
												                       action->integers[2],
												                       action->integers[3]);
 		while(i < action->num_of_companions){
 			action->companions[i]->integers[0] = action->element->rect.x;
 			action->companions[i]->integers[1] = action->element->rect.y;
			action->companions[i]->integers[2] = action->element->rect.w;
			action->companions[i]->integers[3] = action->element->rect.h;
 			i++;
 		}
		return 1;
	}
	return 0;
}

int UIAction_DisplayImage(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	va_copy(vargs, copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);

	if(action->status == 1){
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&action->element->rect);
		return 1;
	}
	return 0;
}

int UIAction_Auto(UI_Action *action, va_list copy_from){
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

int UIAction_External(UI_Action *action, va_list copy_from){
	if(action->status == 1 && action->external != NULL){
		UITrigger_Execute(action);
		action->new_status = 0;
		return 1;
	}
	return 0;
}

int UIAction_ShrinkFitToParent(UI_Action *action, va_list copy_from){
	SDL_Rect temp_rect;
	SDL_Rect *parent_rect = &action->element->parent->rect;
	if(action->status == 1){
			temp_rect.x = action->integers[0];
			temp_rect.y = action->integers[1];
			temp_rect.w = action->integers[2];
			temp_rect.h = action->integers[3];
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

int UIAction_DisplayNumber(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	int w,h;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);
	if(action->integers[0] != action->integers[1]){
		action->integers[1] = action->integers[0];
		sprintf(action->strings[0],"%d",action->integers[1]);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Blended(graphicsData->fonts[action->integers[2]],action->strings[0],colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
		TTF_SizeText(graphicsData->fonts[action->integers[2]],action->strings[0],&w,&h);
		action->integers[4] = w;

	}
	if(UIElement_isVisible(action->element) && action->element->rect.h > TTF_FontHeight(graphicsData->fonts[0]) && action->status != 0 && action->texture != NULL){
		if(action->integers[3] == UISTRING_ALIGN_CENTER){
			temp_rect.x = action->element->rect.x + action->element->rect.w/2 - action->integers[4]/2;
		}
		else if(action->integers[3] == UISTRING_ALIGN_RIGHT){
			temp_rect.x = action->element->rect.x + action->element->rect.w - action->integers[4];
		}
		else{
			temp_rect.x = action->element->rect.x;
		}
		temp_rect.y = action->element->rect.y;
		TTF_SizeText(graphicsData->fonts[action->integers[2]], action->strings[0], &temp_rect.w, &temp_rect.h);
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
		return 1;
	}
	return 0;
}

int UIAction_DisplayString(UI_Action *action, va_list copy_from){
	va_list vargs;
	GraphicsData *graphicsData;
	SDL_Color colour;
	SDL_Surface *temp;
	SDL_Rect temp_rect;
	int w,h;
	colour.r = 0;
	colour.g = 0;
	colour.b = 0;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);
	if(action->strings[0] != NULL && action->strings[1] != NULL && strcmp(action->strings[0],action->strings[1]) != 0){
		action->strings[0] = realloc(action->strings[0], strlen(action->strings[1])+1);
		strcpy(action->strings[0],action->strings[1]);
		SDL_DestroyTexture(action->texture);
		temp = TTF_RenderText_Blended(graphicsData->fonts[action->integers[0]],action->strings[0],colour);
		action->texture = SDL_CreateTextureFromSurface(graphicsData->renderer,temp);
		TTF_SizeText(graphicsData->fonts[action->integers[0]],action->strings[0],&w,&h);
		action->integers[3] = w;
	}
	if(UIElement_isVisible(action->element) && action->element->rect.h > TTF_FontHeight(graphicsData->fonts[action->integers[0]]) && action->strings[0] != NULL && action->status != 0 && action->texture != NULL){
		if(action->integers[1] == UISTRING_ALIGN_CENTER){
			temp_rect.x = action->element->rect.x + action->element->rect.w/2 - action->integers[3]/2;
		}
		else if(action->integers[1] == UISTRING_ALIGN_RIGHT){
			temp_rect.x = action->element->rect.x + action->element->rect.w - action->integers[3];
		}
		else{
			temp_rect.x = action->element->rect.x;
		}
		temp_rect.y = action->element->rect.y;
		TTF_SizeText(graphicsData->fonts[action->integers[0]], action->strings[0], &temp_rect.w, &temp_rect.h);
		SDL_RenderCopy(graphicsData->renderer,action->texture,NULL,&temp_rect);
		return 1;
	}
	return 0;
}

int UIAction_ResourceCounter(UI_Action *action, va_list copy_from){
	va_list vargs;
	int i = 0;
	GameObjectData *gameObjectData;
	va_copy(vargs, copy_from);
	gameObjectData = va_arg(vargs, GameObjectData*);
	va_end(vargs);
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

int UIAction_Counter(UI_Action *action, va_list copy_from){
	action->new_status = 0;
	return 1;
}

int UIAction_RenderLine(UI_Action *action, va_list copy_from){
	int bx,by;
	GraphicsData *graphicsData;
	va_list vargs;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs, GraphicsData*);
	va_end(vargs);
	if(!UIElement_isVisible(action->element) || (action->external != NULL && !UIElement_isVisible(action->external))){
		return 0;
	}
	if(action->status == 3){
		if(action->external == NULL || isLinked(action->external,action->element)){
			action->new_status = 0;
			action->external = NULL;
			action->integers[0] = action->element->rect.x;
			action->integers[1] = action->element->rect.y;
		}
		else{
			action->new_status = 2;
		}
		return 1;
	}
	else if(action->status == 1){
		SDL_SetRenderDrawColor(graphicsData->renderer,0,
										0,
										0,
										255);
		SDL_RenderDrawLine(graphicsData->renderer,action->element->rect.x+action->integers[2],action->element->rect.y+action->integers[3],
									action->integers[0],action->integers[1]);
		return 1;
	}
	else if(action->status == 2){
		if(action->external == NULL){
			action->new_status = 0;
			action->external = NULL;
			action->integers[0] = action->element->rect.x;
			action->integers[1] = action->element->rect.y;
			return 0;
		}
		SDL_SetRenderDrawColor(graphicsData->renderer,0,
										0,
										0,
										255);
		bx = action->external->rect.x + action->element->rect.w/2;
		by = action->external->rect.y;
		SDL_RenderDrawLine(graphicsData->renderer,action->element->rect.x+action->integers[2],action->element->rect.y+action->integers[3],
									bx,by);
		return 1;
	}
	else if(action->status == 4){
		action->external = NULL;
		action->new_status = 0;
		action->integers[0] = action->element->rect.x;
		action->integers[1] = action->element->rect.y;
		return 0;
	}
	return 0;
}

int UIAction_ClickAnywhere(UI_Action *action, va_list copy_from){
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

int UIAction_ReleaseAnywhere(UI_Action *action, va_list copy_from){
	if(action->status == 1){
		UITrigger_Execute(action);
		return 1;
	}
	return 0;
}

int UIAction_CalculateSibling(UI_Action *action, va_list copy_from){
	UI_Element *element;
	int i = 0;
	if(action->status == 1){
		/* Ignore the first sibling, because that's the start block */
		element = action->element->parent->child->sibling;
		while(element != NULL){
			if(element != action->element){
				if(!(action->integers[0] < element->rect.x || action->integers[0] > element->rect.x + element->rect.w) &&
					 !(action->integers[1] < element->rect.y || action->integers[1] > element->rect.y + element->rect.h)){
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
		action->integers[0] = action->element->rect.x;
		action->integers[1] = action->element->rect.y;
	}
	return 1;
}

int UIAction_StoreMousePosition(UI_Action *action, va_list copy_from){
	SDL_Event *event;
	int i = 0;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);
	action->integers[0] = event->motion.x;
	action->integers[1] = event->motion.y;
	if(action->status == 1){
		while(i < action->num_of_companions){
			action->companions[i]->integers[0] = action->integers[0];
			action->companions[i]->integers[1] = action->integers[1];
			i++;
		}
		return 1;
	}
	return 0;
}

int UIAction_FillRect(UI_Action *action, va_list copy_from){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	va_list vargs;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);
	if(action->status == 1){
		SDL_SetRenderDrawColor(graphicsData->renderer,action->RENDERRECT_R,
		                                action->RENDERRECT_G,
		                                action->RENDERRECT_B,
		                                255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		return 1;
	}
	return 0;
}

int UIAction_FillAndBorderRect(UI_Action *action, va_list copy_from){
	/* This should be given the necessary rendering information, in this case,
	   a pointer to the SDL_Window */
	GraphicsData *graphicsData;
	va_list vargs;
	va_copy(vargs,copy_from);
	graphicsData = va_arg(vargs,GraphicsData*);
	va_end(vargs);
	if(action->status == 1 && UIElement_isVisible(action->element)){
		SDL_SetRenderDrawColor(graphicsData->renderer,action->integers[0],
		                                action->integers[1],
		                                action->integers[2],
		                                255);
		SDL_RenderFillRect(graphicsData->renderer,&action->element->rect);
		SDL_SetRenderDrawColor(graphicsData->renderer,action->integers[3],action->integers[4],action->integers[5],255);
		SDL_RenderDrawRect(graphicsData->renderer,&action->element->rect);
		return 1;
	}
	return 0;
}

int UIAction_ClickRect(UI_Action *action, va_list copy_from){
	/* This should get a SDL_Event pointer */
	SDL_Event *event;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);

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

int UIAction_DraggableRectOverride(UI_Action *action, va_list copy_from){
	SDL_Event *event;
	int x,y,i=0;
	va_list vargs;
	va_copy(vargs,copy_from);
	event = va_arg(vargs,SDL_Event*);
	va_end(vargs);

	/* UIACTION_INTS is the status, 0 = rooted, 1 = grabbed */

	if(action->status == 1){
		x = event->motion.x;
		y = event->motion.y;
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

int UIAction_TwoRectOverride(UI_Action *action, va_list copy_from){
	int dt;
	float transition_multiplier;
	va_list vargs;
	va_copy(vargs,copy_from);
	dt = va_arg(vargs,int);
	va_end(vargs);

	if(action->status == 0){
		action->element->rect.x = action->TWORECTOVERRIDE_SX;
		action->element->rect.y = action->TWORECTOVERRIDE_SY;
		action->element->rect.w = action->TWORECTOVERRIDE_SW;
		action->element->rect.h = action->TWORECTOVERRIDE_SH;
		return 1;
	}
	else if(action->status == 1){
	  /* We're going big! */
	  action->TWORECTOVERRIDE_COUNTER+=dt;
	  if(action->TWORECTOVERRIDE_COUNTER > action->TWORECTOVERRIDE_TRANSITION){
	    /* We've become big */
	    action->new_status = 3;
	    action->TWORECTOVERRIDE_COUNTER = action->TWORECTOVERRIDE_TRANSITION;
	  }
	  transition_multiplier = ((float) action->TWORECTOVERRIDE_COUNTER)/((float) action->TWORECTOVERRIDE_TRANSITION);
	  action->element->rect.x = action->TWORECTOVERRIDE_SX + (action->TWORECTOVERRIDE_BX - action->TWORECTOVERRIDE_SX) * transition_multiplier;
	  action->element->rect.y = action->TWORECTOVERRIDE_SY + (action->TWORECTOVERRIDE_BY - action->TWORECTOVERRIDE_SY) * transition_multiplier;
	  action->element->rect.w = action->TWORECTOVERRIDE_SW + (action->TWORECTOVERRIDE_BW - action->TWORECTOVERRIDE_SW) * transition_multiplier;
	  action->element->rect.h = action->TWORECTOVERRIDE_SH + (action->TWORECTOVERRIDE_BH - action->TWORECTOVERRIDE_SH) * transition_multiplier;
	  return 1;
	}
	else if(action->status == 2){
	  /* We're going small! */
	  action->TWORECTOVERRIDE_COUNTER-=dt;
	  if(action->TWORECTOVERRIDE_COUNTER < 0){
	    /* We've become small */
	    action->new_status = 0;
	    action->TWORECTOVERRIDE_COUNTER = 0;
	  }
	  transition_multiplier = ((float) action->TWORECTOVERRIDE_COUNTER)/((float) action->TWORECTOVERRIDE_TRANSITION);
	  action->element->rect.x = action->TWORECTOVERRIDE_SX + (action->TWORECTOVERRIDE_BX - action->TWORECTOVERRIDE_SX) * transition_multiplier;
	  action->element->rect.y = action->TWORECTOVERRIDE_SY + (action->TWORECTOVERRIDE_BY - action->TWORECTOVERRIDE_SY) * transition_multiplier;
	  action->element->rect.w = action->TWORECTOVERRIDE_SW + (action->TWORECTOVERRIDE_BW - action->TWORECTOVERRIDE_SW) * transition_multiplier;
	  action->element->rect.h = action->TWORECTOVERRIDE_SH + (action->TWORECTOVERRIDE_BH - action->TWORECTOVERRIDE_SH) * transition_multiplier;
	  return 1;
	}
	else if(action->status == 3){
		action->element->rect.x = action->TWORECTOVERRIDE_BX;
		action->element->rect.y = action->TWORECTOVERRIDE_BY;
		action->element->rect.w = action->TWORECTOVERRIDE_BW;
		action->element->rect.h = action->TWORECTOVERRIDE_BH;
		return 1;
	}
  return 0;
}

int UIAction_UpdateTwoRectOverrideOnWindowResize(UI_Action *action, va_list copy_from){
		va_list vargs;
		SDL_Event *event;
		SDL_Point point;
		va_copy(vargs, copy_from);
		event = va_arg(vargs,SDL_Event*);
		va_end(vargs);
		if(action->status == 1){
			point = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
													                             action->floats[0],
													                             action->floats[1]);
			action->companions[0]->TWORECTOVERRIDE_BX = point.x + action->integers[0];
			action->companions[0]->TWORECTOVERRIDE_BY = point.y + action->integers[1];

			point = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
		                           action->floats[2],
														   action->floats[3]);
			action->companions[0]->TWORECTOVERRIDE_BW = point.x + action->integers[2];
			action->companions[0]->TWORECTOVERRIDE_BH = point.y + action->integers[3];

			point = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
													                             action->floats[4],
													                             action->floats[5]);
			action->companions[0]->TWORECTOVERRIDE_SX = point.x + action->integers[4];
			action->companions[0]->TWORECTOVERRIDE_SY = point.y + action->integers[5];

			point = getPointFromPerc(SDL_GetWindowFromID(event->window.windowID),
		                           action->floats[6],
														   action->floats[7]);
			action->companions[0]->TWORECTOVERRIDE_SW = point.x + action->integers[6];
			action->companions[0]->TWORECTOVERRIDE_SH = point.y + action->integers[7];

			return 1;
	}
	return 0;
}
/* These argument names are horrible, but they stand for Big/Small X/Y Int/Float Point/Dimension */
void UIConfigure_UpdateTwoRectOverrideOnWindowResize(UI_Element *element, UI_Action *action, UI_Action *twoRectOverride, int bxip, int byip, float bxfp, float byfp,
	                                                                                                                      int bxid, int byid, float bxfd, float byfd,
                                                                                                                        int sxip, int syip, float sxfp, float syfp,
																																																											  int sxid, int syid, float sxfd, float syfd){
	UIAction_Init(element,action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_UpdateTwoRectOverrideOnWindowResize;
	action->integers = malloc(sizeof(int) * 8);
	action->num_of_integers = 8;
	action->integers[0] = bxip;
	action->integers[1] = byip;
	action->integers[2] = bxid;
	action->integers[3] = byid;
	action->integers[4] = sxip;
	action->integers[5] = syip;
	action->integers[6] = sxid;
	action->integers[7] = syid;

	action->floats = malloc(sizeof(float) * 8);
	action->num_of_floats = 8;
	action->floats[0] = bxfp;
	action->floats[1] = byfp;
	action->floats[2] = bxfd;
	action->floats[3] = byfd;
	action->floats[4] = sxfp;
	action->floats[5] = syfp;
	action->floats[6] = sxfd;
	action->floats[7] = syfd;
	action->companions = malloc(sizeof(void*));
	action->companions[0] = twoRectOverride;
	action->num_of_companions = 1;
}

void UIConfigure_FillRect(UI_Element *element, UI_Action *action, int r, int g, int b){
	#if DEBUGGING==1
	printf("UIConfigure_FillRect\n");
	#endif
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_FillRect;
	action->integers = malloc(sizeof(int) * 3);
	action->num_of_integers = 3;

	action->RENDERRECT_R = r;
	action->RENDERRECT_G = g;
	action->RENDERRECT_B = b;
}

void UIConfigure_FillAndBorderRect(UI_Element *element, UI_Action *action, int fr, int fg, int fb, int br, int bg, int bb){
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_FillAndBorderRect;
	action->integers = malloc(sizeof(int) * 6);
	action->num_of_integers = 6;

	action->integers[0] = fr;
	action->integers[1] = fg;
	action->integers[2] = fb;
	action->integers[3] = br;
	action->integers[4] = bg;
	action->integers[5] = bb;
}

void UIConfigure_LeftClickRect(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_LeftClickRect\n");
	#endif
	UIAction_Init(element,action);
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickRect;
}

void UIConfigure_RightClickRect(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_RightClickRect\n");
	#endif
	UIAction_Init(element,action);
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickRect;
}

void UIConfigure_TwoRectOverride(UI_Element *element, UI_Action *action, int sx, int sy, int sw, int sh, int bx, int by, int bw, int bh, int delay, int small_triggers, int big_triggers,...){
	#if DEBUGGING==1
	printf("UIConfigure_TwoRectOverride\n");
	#endif
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_TwoRectOverride;
	action->element = element;
	action->status = 0;
	action->new_status = 0;
	action->integers = malloc(sizeof(int) * 10);
	action->num_of_integers = 10;
	/* Small */
	action->TWORECTOVERRIDE_SX = sx;
	action->TWORECTOVERRIDE_SY = sy;
	action->TWORECTOVERRIDE_SW = sw;
	action->TWORECTOVERRIDE_SH = sh;
	/* Big */
	action->TWORECTOVERRIDE_BX = bx;
	action->TWORECTOVERRIDE_BY = by;
	action->TWORECTOVERRIDE_BW = bw;
	action->TWORECTOVERRIDE_BH = bh;
	/* State  0 = SMALL*/
	/* Time */
	action->TWORECTOVERRIDE_COUNTER = 0;
	/* Transition time */
	action->TWORECTOVERRIDE_TRANSITION = delay;
}

void UIConfigure_DraggableRectOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	#if DEBUGGING==1
	printf("UIConfigure_DraggableRectOverride\n");
	#endif
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

void UIConfigure_RenderLine(UI_Element *element, UI_Action *action, enum LineOrigins origin, UI_Element *external){
	#if DEBUGGING==1
	printf("UIConfigure_RenderLine\n");
	#endif
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_RenderLine;
	action->external = external;
	action->status = 2;
	action->new_status = 2;
	action->integers = calloc(4,sizeof(int));
	action->num_of_integers = 4;

	action->integers[0] = element->rect.x;
	action->integers[1] = element->rect.y;
	switch(origin){
		case CENTER:
			action->integers[2] = (element->rect.w/2);
			action->integers[3] = (element->rect.h/2);
			break;
		case BL_CORNER:
			action->integers[2] = 0;
			action->integers[3] = element->rect.h;
			break;
		case BR_CORNER:
			action->integers[2] = element->rect.w;
			action->integers[3] = element->rect.h;
			break;
		case TL_CORNER:
			action->integers[2] = 0;
			action->integers[3] = 0;
			break;
		case TR_CORNER:
			action->integers[2] = element->rect.w;
			action->integers[3] = 0;
			break;
	}
}

void UIConfigure_StoreMousePosition(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	#if DEBUGGING==1
	printf("UIConfigure_StoreMousePosition\n");
	#endif
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
	action->integers = malloc(sizeof(int) * 2);
	action->num_of_integers = 2;
	va_end(vargs);
}

void UIConfigure_CalculateSibling(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	#if DEBUGGING==1
	printf("UIConfigure_CalculateSibling\n");
	#endif
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
	action->integers = malloc(sizeof(int) * 2);
	action->num_of_integers = 2;
	va_end(vargs);
}

void UIConfigure_LeftClickAnywhere(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_LeftClickAnywhere\n");
	#endif
	UIAction_Init(element,action);
	action->response = LEFT_CLICK;
	action->function = UIAction_ClickAnywhere;
}

void UIConfigure_RightClickAnywhere(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_RightClickAnywhere\n");
	#endif
	UIAction_Init(element,action);
	action->response = RIGHT_CLICK;
	action->function = UIAction_ClickAnywhere;
}

void UIConfigure_LeftReleaseAnywhere(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_LeftReleaseAnywhere\n");
	#endif
	UIAction_Init(element,action);
	action->response = LEFT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
}

void UIConfigure_RightReleaseAnywhere(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_RightReleaseAnywhere\n");
	#endif
	UIAction_Init(element,action);
	action->response = RIGHT_RELEASE;
	action->function = UIAction_ReleaseAnywhere;
}

void UIConfigure_Counter(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_Counter\n");
	#endif
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_Counter;
	action->status = 0;
	action->new_status = 0;
}

void UIConfigure_External(UI_Element *element, UI_Action *action, UI_Element *external){
	#if DEBUGGING==1
	printf("UIConfigure_External\n");
	#endif
	UIAction_Init(element,action);
	action->response = EXTERNAL;
	action->function = UIAction_External;
	action->external = external;
	action->status = 0;
	action->new_status = 0;
}

void UIConfigure_ResourceCounter(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	#if DEBUGGING==1
	printf("UIConfigure_ResourceCounter\n");
	#endif
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = GAME_OBJECT_UPDATE;
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

void UIConfigure_DisplayString(UI_Element *element, UI_Action *action, char *string, int font, enum UIString_Align align){
	#if DEBUGGING==1
	printf("UIConfigure_DisplayString\n");
	#endif
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_DisplayString;
	action->strings = malloc(sizeof(char*) * 2);
	if(string != NULL){
		action->strings[0] = calloc(strlen(string)+1,sizeof(char));
		action->strings[1] = calloc(strlen(string)+1,sizeof(char));
		strcpy(action->strings[1], string);
	}
	else{
		action->strings[0] = NULL;
		action->strings[1] = NULL;
	}
	action->new_status = 1;
	action->status = 1;
	action->num_of_strings = 2;
	action->integers = calloc(4,sizeof(int));
	action->integers[0] = font;
	action->integers[1] = align;
}

void UIConfigure_DisplayNumber(UI_Element *element, UI_Action *action, int number, int font, enum UIString_Align align){
	#if DEBUGGING==1
	printf("UIConfigure_DisplayNumber\n");
	#endif
	UIAction_Init(element,action);
	action->response = RENDER;
	action->status = 1;
	action->new_status = 1;
	action->function = UIAction_DisplayNumber;
	action->integers = calloc(6, sizeof(int));
	action->integers[0] = number;
	action->integers[2] = font;
	action->integers[3] = align;
	action->num_of_integers = 4;
	action->strings = malloc(sizeof(char*) * 1);
	action->strings[0] = calloc(30,sizeof(char));
	action->num_of_strings = 1;
}

void UIConfigure_ShrinkFitToParent(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_ShrinkFitToParent\n");
	#endif
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_ShrinkFitToParent;
	action->integers = calloc(4,(sizeof(int)));
	action->integers[0] = element->rect.x;
	action->integers[1] = element->rect.y;
	action->integers[2] = element->rect.w;
	action->integers[3] = element->rect.h;
}

void UIConfigure_Auto(UI_Element *element, UI_Action *action, enum Response response){
	#if DEBUGGING==1
	printf("UIConfigure_Auto\n");
	#endif
	UIAction_Init(element,action);
	action->response = response;
	action->function = UIAction_Auto;
}

void UIConfigure_DisplayImage(UI_Element *element, UI_Action *action, SDL_Texture *image){
	#if DEBUGGING==1
	printf("UIConfigure_DisplayImage\n");
	#endif
	UIAction_Init(element,action);
	action->response = RENDER;
	action->function = UIAction_DisplayImage;
	action->texture = image;
}

void UIConfigure_InverseRect(UI_Element *element, UI_Action *action, int from_left, int from_top, int from_right, int from_bot, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	#if DEBUGGING==1
	printf("UIConfigure_InverseRect\n");
	#endif
	va_start(vargs, num_of_companions);
	UIAction_Init(element, action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_InverseRect;
	action->integers = malloc(sizeof(int) * 4);
	action->integers[0] = from_left;
	action->integers[1] = from_top;
	action->integers[2] = from_right;
	action->integers[3] = from_bot;
	action->num_of_integers = 4;
	action->num_of_companions = num_of_companions;
	action->companions = malloc(sizeof(void*) * num_of_companions);
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

void UIConfigure_PercRect(UI_Element *element, UI_Action *action, float from_left, float from_top, float width, float height){
	#if DEBUGGING==1
	printf("UIConfigure_PercRect\n");
	#endif
	UIAction_Init(element,action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_PercRect;
	action->floats = malloc(sizeof(float) * 4);
	action->floats[0] = from_left;
	action->floats[1] = from_top;
	action->floats[2] = width;
	action->floats[3] = height;
	action->num_of_floats = 4;
}

void UIConfigure_AddAiBlock(UI_Element *element, UI_Action *action, BlockFunctionTemplate *template, UI_Element *target){
	#if DEBUGGING==1
	printf("UIConfigure_AddAiBlock\n");
	#endif
	UIAction_Init(element,action);
	action->response = EXTERNAL;
	action->function = UIAction_AddAiBlock;
	action->external = target;
	action->status = 0;
	action->new_status = 0;
	action->extra = template;
}

void UIConfigure_ReadAiBlocks(UI_Element *element, UI_Action *action, UI_Action *infoOutput){
	#if DEBUGGING==1
	printf("UIConfigure_ReadAiBlocks\n");
	#endif
	UIAction_Init(element,action);
	action->response = AI_RESPONSE;
	action->function = UIAction_ReadAiBlocks;
	action->status = 0;
	action->new_status = 0;
	action->companions = malloc(sizeof(void*));
	action->companions[0] = infoOutput;
	action->num_of_companions = 1;
}

void UIConfigure_SetUpAiBlock(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	#if DEBUGGING==1
	printf("UIConfigure_SetUpAiBlock\n");
	#endif
	UIAction_Init(element,action);
	va_start(vargs,num_of_companions);
	action->response = AI_RESPONSE;
	action->function = UIAction_SetUpAiBlock;
	action->num_of_companions = num_of_companions;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	va_end(vargs);
}

void UIConfigure_DeleteKeyFlagDestroy(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_DeleteKeyFlagDestroy\n");
	#endif
	UIAction_Init(element,action);
	action->response = RESPONSE_DELETE;
	action->function = UIAction_DeleteKeyFlagDestroy;
}

void UIConfigure_RecallWorkers(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_RecallWorkers\n");
	#endif
	UIAction_Init(element,action);
	action->response = GAME_OBJECT_UPDATE;
	action->function = UIAction_RecallWorkers;
	action->status = 0;
	action->new_status = 0;
}

void UIConfigure_NullifyAI(UI_Element *element, UI_Action *action){
	#if DEBUGGING==1
	printf("UIConfigure_NullifyAI\n");
	#endif
	UIAction_Init(element,action);
	action->response = AI_RESPONSE;
	action->function = UIAction_NullifyAI;
	action->status = 0;
	action->new_status = 0;
}

void UIConfigure_DraggableVerticalOverride(UI_Element *element, UI_Action *action, int num_of_companions, ...){
	int i = 0;
	va_list vargs;
	va_start(vargs,num_of_companions);
	#if DEBUGGING==1
	printf("UIConfigure_DraggableVerticalOverride\n");
	#endif
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

void UIConfigure_GetDifferenceInChildYOffset(UI_Element *element, UI_Action *action, float initial_perc, int initial_offset, UI_Element *child, int num_of_companions, ...){
		int i = 0;
		va_list vargs;
		va_start(vargs,num_of_companions);
		#if DEBUGGING==1
		printf("UIConfigure_GetDifferenceInChildYOffset\n");
		#endif
		UIAction_Init(element,action);
		action->response = RENDER;
		action->function = UIAction_GetDifferenceInChildYOffset;
		action->num_of_companions = num_of_companions;
		action->integers = malloc(2*sizeof(int));
		action->integers[0] = initial_offset;
		action->integers[1] = 0;
		action->num_of_integers = 2;
		action->floats = malloc(sizeof(float));
		action->floats[0] = initial_perc;
		action->num_of_floats = 1;
		action->companions = calloc(num_of_companions, sizeof(UI_Action*));
		while(i < num_of_companions){
			action->companions[i] = va_arg(vargs,UI_Action*);
			i++;
		}
		action->external = child;
		va_end(vargs);
}

void UIConfigure_ShrinkFitToParentWithYShift(UI_Element *element, UI_Action *action, UI_Action *shiftSource){
	#if DEBUGGING==1
	printf("UIConfigure_ShrinkFitToParent\n");
	#endif
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_ShrinkFitToParentWithYShift;
	action->integers = calloc(4,(sizeof(int)));
	action->integers[0] = element->rect.x;
	action->integers[1] = element->rect.y;
	action->integers[2] = element->rect.w;
	action->integers[3] = element->rect.h;
	action->companions = malloc(sizeof(UI_Action*));
	action->companions[0] = shiftSource;
	action->num_of_companions = 1;
}

void UIConfigure_CalculateScrollListOffset(UI_Element *element, UI_Action *action, int total_possible_offset){
	UIAction_Init(element,action);
	action->response = UPDATE;
	action->function = UIAction_CalculateScrollListOffset;
	action->integers = calloc(3, sizeof(int));
	action->integers[0] = 0;
	action->integers[1] = total_possible_offset;
	action->num_of_integers = 2;
}

void UIConfigure_InversePosition(UI_Element *element, UI_Action *action, int x_offset, int y_offset){
	UIAction_Init(element,action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_InversePosition;
	action->integers = calloc(2,sizeof(int));
	action->integers[0] = x_offset;
	action->integers[1] = y_offset;
	action->num_of_integers = 2;
}

void UIConfigure_PercPosition(UI_Element *element, UI_Action *action, float x_offset, float y_offset, int xi_offset, int yi_offset, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_PercPosition;
	action->floats = calloc(2,sizeof(float));
	action->floats[0] = x_offset;
	action->floats[1] = y_offset;
	action->num_of_floats = 2;
	action->integers = calloc(2,sizeof(int));
	action->integers[0] = xi_offset;
	action->integers[1] = yi_offset;
	action->num_of_integers = 2;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->num_of_companions = num_of_companions;
	va_end(vargs);
}

void UIConfigure_PercPositionV(UI_Element *element, UI_Action *action, float x_offset, float y_offset, int xi_offset, int yi_offset, int num_of_companions, ...){
	va_list vargs;
	int i = 0;
	va_start(vargs,num_of_companions);
	UIAction_Init(element,action);
	action->response = WINDOW_RESIZE;
	action->function = UIAction_PercPositionV;
	action->floats = calloc(2,sizeof(float));
	action->floats[0] = x_offset;
	action->floats[1] = y_offset;
	action->num_of_floats = 2;
	action->integers = calloc(2,sizeof(int));
	action->integers[0] = xi_offset;
	action->integers[1] = yi_offset;
	action->num_of_integers = 2;
	action->companions = calloc(num_of_companions, sizeof(UI_Action*));
	while(i < num_of_companions){
		action->companions[i] = va_arg(vargs,UI_Action*);
		i++;
	}
	action->num_of_companions = num_of_companions;
	va_end(vargs);
}

void UIConfigure_PercOffsetRect(UI_Element *element, UI_Action *action, float xfp, float yfp, float xfd, float yfd,
                                                                        int xip, int yip, int xid, int yid,
																																			  int num_of_companions, ...){
		va_list vargs;
		int i = 0;
		va_start(vargs,num_of_companions);
		UIAction_Init(element,action);
		action->response = WINDOW_RESIZE;
		action->function = UIAction_PercOffsetRect;
		action->floats = calloc(4,sizeof(float));
		action->floats[0] = xfp;
		action->floats[1] = yfp;
		action->floats[2] = xfd;
		action->floats[3] = yfd;
		action->integers = calloc(4,sizeof(int));
		action->integers[0] = xip;
		action->integers[1] = yip;
		action->integers[2] = xid;
		action->integers[3] = yid;
		action->companions = malloc(sizeof(void*) * num_of_companions);
		while(i < num_of_companions){
			action->companions[i] = va_arg(vargs,UI_Action*);
			i++;
		}
		action->num_of_companions = num_of_companions;
		va_end(vargs);
}

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
	UI_Element *sibling = element->parent;
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
	free(element->actions);
	free(element);
}

static void UIAction_Free(UI_Action *action){
	UI_Trigger *movingPointer2;
	UI_Trigger *movingPointer = action->triggers;
	int i = 0;

	while(action->triggers!=NULL){
		UITrigger_Destroy(action,action->triggers);
	}

	while(i < action->num_of_strings){
		free(action->strings[i]);
		i++;
	}
	free(action->strings);
	free(action->companions);
	free(action->integers);
	free(action->floats);

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
	while(i < root->num_of_actions){
		if(root->actions[i].external == external){
			root->actions[i].external = NULL;
			trigger = root->actions[i].triggers;
			while(trigger != NULL){
				trigger2 = trigger->next;
				if(trigger->action->element == external){
					UITrigger_Destroy(&root->actions[i],trigger);
				}
				trigger = trigger2;
			}
		}
		i++;
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

int UIElement_Execute(UI_Element *element, enum Response response, va_list vargs){
	int i = 0;
	int ret = 0;
	int result = 0;
	while(i < element->num_of_actions){
		if(element->actions[i].response == response){
			result = element->actions[i].function(&element->actions[i],vargs);
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

void UIRoot_Execute(UIData *uiData, enum Response response, int stopAtFirst, ...){
	int shouldStop;
	va_list vargs;
	UI_Element *queue[255] = {NULL};
	UI_Element *queue_element;
	UI_Element *queue_element_child;
	int front = 0, back = 1;
	va_start(vargs, stopAtFirst);
	queue[0] = uiData->root;

	while(front != back){
		queue_element = queue[front];
		front = (front + 1) % 255;
		shouldStop = UIElement_Execute(queue_element,response,vargs);
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
	va_end(vargs);
}

/* Does a depth based traversal of the tree backwards (i.e. going from the deepest layer to
   the top layer) */
void UIRoot_ExecuteUpwards(UIData *uiData, enum Response response, int stopAtFirst, ...){
	static int guesstimated_queue_length = 2;
	va_list vargs;
	UI_Element **queue = calloc(guesstimated_queue_length,sizeof(UI_Element*));
	UI_Element *queue_element;
	UI_Element *queue_element_child;
	int front = 0,back = 1;
	va_start(vargs, stopAtFirst);
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
		if(response == DISPOSAL && queue_element->destroy){
			UIElement_Free(queue_element);
		}
		else{
			if(UIElement_Execute(queue_element,response,vargs) && stopAtFirst){
				break;
			}
		}
	}
	va_end(vargs);
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

static int percWindowDimension(int dimension, float perc){
	return (int) ((float) dimension * perc);
}

void UIRoot_Destroy(UIData *uiData){
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
		UIElement_Free(queue_element);
	}
	free(queue);
}

void UIRoot_Pack(UIData *uiData, GraphicsData *graphicsData){
	UIRoot_Execute(uiData,WINDOW_RESIZE,0,graphicsData->window);
}

static void UIElement_RemoveExposedData(UI_Element *element){
	/* I'm going to do some hax with void pointers here */
	int i = 0;
	free(element->exposed_data_types);
	while(i < element->exposed_data_count){
		free(element->exposed_data[i]);
		i++;
	}
	free(element->exposed_data);
	element->exposed_data_types = NULL;
	element->exposed_data = NULL;
	element->exposed_data_count = 0;
}
