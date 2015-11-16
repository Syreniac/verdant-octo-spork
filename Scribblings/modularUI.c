#include "modularUI.h"

int main(int argc, char *argv[]){
  goSDL();
  return 0;
}

void swapVisibilityAndTangibilityA(UI_Element *element){
	UI_Element *child = element->generic.child;
	
	if(element->generic.visibility == VISIBLE){
		element->generic.visibility = INVISIBLE;
	}
	else if(element->generic.visibility == INVISIBLE){
		element->generic.visibility = TEMPORARILY_INVISIBLE_A;
	}

    if(element->generic.tangibility == TANGIBLE){
      element->generic.tangibility = INTANGIBLE;
    }
    else if(element->generic.tangibility == INTANGIBLE){
      element->generic.tangibility = TEMPORARILY_INTANGIBLE_A;
    }
	
	while(child != NULL){
		swapVisibilityAndTangibilityA(child);
		child = child->generic.sibling;
	}
}

int UI_buttonToggleExpansion(UI_Element *element){
  UI_Element *parent = element->generic.parent;
  UI_Element *child = parent->generic.child;
  
  if(parent->expandable.status == BIG){
    parent->expandable.status = BIG2SMALL;
  }
  else if(parent->expandable.status == SMALL){
    parent->expandable.status = SMALL2BIG;
  }
  while(child != NULL){
	swapVisibilityAndTangibilityA(child);
	child = child->generic.sibling;
  }
  return 0;
}

int UI_toggleChildrenVisibilityTangibility(UI_Element *element){
  UI_Element *child = element->generic.child;
  int i = 0;
  while(child != NULL){

    if(child->generic.visibility == TEMPORARILY_INVISIBLE_A){
      child->generic.visibility = VISIBLE;
    }

    if(child->generic.tangibility == TEMPORARILY_INTANGIBLE_A){
      child->generic.tangibility = TANGIBLE;
    }
	
	/* make sure that any children are properly rooted before shrinking or growing */
	
	if(child->generic.type == DRAGGABLE){
		child->draggable.status = ROOTED;
	}

	UI_toggleChildrenVisibilityTangibility(child);
	
    child = child->generic.sibling;
    i++;
  }

  return 0;
}

int UI_toggleDragged(UI_Element *element){
  assert(element->generic.type == DRAGGABLE);
  if(element->draggable.status == ROOTED){
    element->draggable.status = GRABBED;
	element->generic.response = CLICK_UP;
  }
  else if(element->draggable.status == GRABBED){
    element->draggable.status = ROOTED;
	element->generic.response = CLICK_DOWN;
    preventOverlap(element);
  }
  return 0;
}

int UI_increaseOffset(UI_Element *element){
	UI_Element *controlled_element = *element->generic.controller.elements;
	assert(controlled_element->type == PANEL);
	controlled_element->panel.x_offset+=10;
	
	return 0;
}

int UI_pokingAround(UI_Element *element){
	/* this just prints how many children you'd need to step through to find this from its parent. I'm using it to test stuff */
	UI_Element *child = element->generic.parent->generic.child;
	UI_Element *panel = (element->generic.controller.elements[0]);
	int i = 0;
	printf("clicked on list button thing\n");
	while(child != element){
		i++;
		child = child->generic.sibling;
	}
	child = createUI_Draggable(panel->panel.rect.x,panel->panel.rect.y,20,20,element->panel.colour,&UI_toggleDragged,NULL);
	printf("created draggable\n");
	addChild(panel,child);
	
	printf("My parent has at least %d children!\n",i);
	printf("My name is %s\n",element->generic.name);
	return 0;
}

int UI_getBothOffsetsFromSliders(UI_Element *element){
	UI_Element *slider;
	SDL_Rect sliderRect, parentRect,elementRect = *getElementRect(element),boundingBox = getTheoreticalBoundingBox(element,20);
	float progress, progress_max;
	/* Get x, if we need to */
	if(boundingBox.x != elementRect.x || boundingBox.w != elementRect.w || 1){
		slider = element->generic.controller.elements[0];
		sliderRect = getElementRectWithOffset(slider);
		parentRect = getElementRectWithOffset(slider->generic.parent);
		progress = (float) (sliderRect.x - parentRect.x);
		progress_max = (float) (parentRect.w - sliderRect.w);
		element->panel.x_offset = -((boundingBox.x+boundingBox.w) - (elementRect.x+elementRect.w)) * (progress/progress_max);
	}
	/* Get y if we need to */
	if(boundingBox.y != elementRect.y || boundingBox.h != elementRect.h){
		slider = element->generic.controller.elements[1];
		sliderRect = getElementRectWithOffset(slider);
		parentRect = getElementRectWithOffset(slider->generic.parent);
		progress = (float) (sliderRect.y - parentRect.y);
		progress_max = (float) (parentRect.h - sliderRect.h);
		element->panel.y_offset = -((boundingBox.y+boundingBox.h) - (elementRect.y+elementRect.h)) * (progress/progress_max);
	}
	return 0;
}

int UI_getVerticalOffsetsFromSlider(UI_Element *element){
	UI_Element *slider;
	SDL_Rect sliderRect, parentRect,elementRect = *getElementRect(element),boundingBox;
	float progress, progress_max;
	boundingBox = getTheoreticalBoundingBox(element,0);
	// printf("vertical slider control\n");
	// /* Get x, if we need to */
	// printf("element y/h %d/%d\n",elementRect.y,elementRect.h);
	// printf("tbb y/h %d/%d\n",boundingBox.y,boundingBox.h);
	// printf("current offset: %d\n",element->panel.y_offset);
	if(boundingBox.y != elementRect.y || boundingBox.h != elementRect.h){
		slider = element->generic.controller.elements[0];
		sliderRect = getElementRectWithOffset(slider);
		parentRect = getElementRectWithOffset(slider->generic.parent);
		progress = (float) (sliderRect.y - parentRect.y);
		progress_max = (float) (parentRect.h - sliderRect.h);
		element->panel.y_offset = -((boundingBox.y+boundingBox.h) - (elementRect.y+elementRect.h)) * ((progress/progress_max));
	}
	return 0;
}

UI_Element *createExpandablePanel(void){
  UI_Element *expandable = createUI_Expandable(0,0,50,50,
                                    50,50,500,400,
                                    0xb00000,&UI_toggleChildrenVisibilityTangibility,NULL);
  UI_Element *button = createUI_Button(0,0,50,50,0x00b000,&UI_buttonToggleExpansion,NULL);
  UI_Element *button2 = createUI_Button(530,50,20,20,0x0000b0,&UI_buttonToggleExpansion,NULL);
  UI_Element *draggable;
  UI_Element *button3;
  UI_Element *panel = createUI_OffsetPanel(70,70,480,380,0xb0b0b0,&UI_getBothOffsetsFromSliders,calloc(2,sizeof(&draggable)));
  UI_Element *panel2 = createUI_Panel(70,50,380,20,0xa0fa00);
  UI_Element *panel3 = createUI_Panel(50,70,20,380,0xa0fa00);
  UI_Element *panel4 = createUI_OffsetPanel(450,70,80,380,0x0404B4,&UI_getVerticalOffsetsFromSlider,calloc(1,sizeof(&draggable)));
  UI_Element *panel5 = createUI_Panel(530,70,20,380,0xF7FE2E);
  int i = 0;

  addChild(expandable,button);
  addChild(expandable,button2);
  addChild(expandable,panel);
  addChild(expandable,panel2);
  addChild(expandable,panel3);
  addChild(expandable,panel4);
  addChild(expandable,panel5);
  while(i < 3){
	draggable = createUI_Draggable(290,140+i*30,20,20,0xb000b0,&UI_toggleDragged,NULL);
	  draggable->generic.visibility = INVISIBLE;
	  draggable->generic.tangibility = INTANGIBLE;
	addChild(panel,draggable);
	i++;
  }
  draggable = createUI_Draggable(70,50,20,20,0xffffff,&UI_toggleDragged,NULL);
  draggable->generic.visibility = INVISIBLE;
  draggable->generic.tangibility = INTANGIBLE;
  addChild(panel2,draggable);
  panel->generic.controller.elements[0] = draggable;
  draggable = createUI_Draggable(50,70,20,20,0xffffff,&UI_toggleDragged,NULL);
  draggable->generic.visibility = INVISIBLE;
  draggable->generic.tangibility = INTANGIBLE;
  addChild(panel3,draggable);
  panel->generic.controller.elements[1] = draggable;
  draggable = createUI_Draggable(530,70,20,20,0xE0E6F8,&UI_toggleDragged,NULL);
  draggable->generic.visibility = INVISIBLE;
  draggable->generic.tangibility = INTANGIBLE;
  addChild(panel5,draggable); 
  panel4->generic.controller.elements[0] = draggable;
  
  i = 0;
  while(i < 20){
	button3 = createUI_Button(470,70+i*50,40,40,(rand() % 0xff << 16) + (rand() % 0xff << 8) + (rand() % 0xff),&UI_pokingAround,calloc(1,sizeof(&button3)));
	
	button3->generic.controller.elements[0] = panel;
	if(i==1){printf("%p\n",button3);}
	button3->generic.visibility = INVISIBLE;
	button3->generic.tangibility = INTANGIBLE;
	sprintf(button3->generic.name,"Button %d",i);
	addChild(panel4,button3);
	i++;
  }

  button->generic.visibility = FIXED_INVISIBILITY;
  button2->generic.visibility = INVISIBLE;
  button2->generic.tangibility = INTANGIBLE;
  panel->generic.visibility = INVISIBLE;
  panel2->generic.visibility = INVISIBLE;
  panel3->generic.visibility = INVISIBLE;
  panel4->generic.visibility = INVISIBLE;
  panel5->generic.visibility = INVISIBLE;

  return(expandable);
}

void goSDL(void){
  UI_Element *root;
  UI_Element *expand;
  SDL_Window *window;
  SDL_Event event;
  int i = SDL_GetTicks();
  int i2;

  SDL_Init(SDL_INIT_EVERYTHING);

  window = SDL_CreateWindow(
   "An SDL2 window",
   SDL_WINDOWPOS_UNDEFINED,
   SDL_WINDOWPOS_UNDEFINED,
   640,
   480,
   SDL_WINDOW_OPENGL);


  root = createRoot();
  expand = createExpandablePanel();
  addChild(root,expand);
  while(1){
    i2 = SDL_GetTicks() - i;
    i = SDL_GetTicks();
    SDL_FillRect(SDL_GetWindowSurface(window),
                 NULL,
                 0x000000);
    renderUI(root,window,i2);
    SDL_UpdateWindowSurface(window);
    while(SDL_PollEvent(&event)){
      switch(event.type){
	    case SDL_MOUSEMOTION:
		  motiononUI(root,&event);
		  break;
        case SDL_MOUSEBUTTONDOWN:
          clickonUI(root,&event);
          break;
		case SDL_MOUSEBUTTONUP:
		  clickuponUI(root,&event);
		  break;
        case SDL_KEYDOWN:
          exit(0);
          break;
      }
    }
    SDL_Delay(20);
  }

}
