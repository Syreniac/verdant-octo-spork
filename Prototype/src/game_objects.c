#include "game_objects.h"

static ResourceNode *chooseNodeRandomly(ResourceNodeSpawner *resourceNodeSpawner);
static void nullifyProgrammableWorkerBrain(ProgrammableWorkerBrain *brain);

void initAudio(GameObjectData *gameObjectData, AudioData audioData){
	gameObjectData->audioData = audioData;
}

static ResourceNode *chooseNodeRandomly(ResourceNodeSpawner *resourceNodeSpawner){
	int i = 0, r = rand();
	if(resourceNodeSpawner->currentNodeCount > 0){
	r = r % (resourceNodeSpawner->currentNodeCount);
	while(i < resourceNodeSpawner->currentNodeCount){
		if(resourceNodeSpawner->resourceNodes[i].alive){
		if(r == 0){
			return &resourceNodeSpawner->resourceNodes[i];
		}
		r--;
		}
		i++;
	}
	}
	return NULL;
}

ProgrammableWorker *getNearestWorker(GameObjectData *gameObjectData, int x, int y, ProgrammableWorker *ignore){
	int i = 0;
	SDL_Point point;
	double d2;
	double d22;
	ProgrammableWorker *pw = NULL;
	ProgrammableWorker *p;
	printf("%lf\n",d2);
	p = gameObjectData->first_programmable_worker;
	while(p!=NULL){
		point = getCenterOfRect(p->rect);
		d22 = getDistance2BetweenPoints(point.x,point.y,x,y);
		if(p!= ignore && (d22 < d2 || pw == NULL)){
			d2 = d22;
			pw = p;
		}
		p = p->next;
	}
	return pw;
}

Tree *getNearestTree(GameObjectData *gameObjectData, int x, int y){
	int i = 0;
	SDL_Point p;
	double d2;
	double d22;
	Tree *tree = NULL;
	printf("%lf\n",d2);
	while(i < NUMBER_OF_TREES){
		p = getCenterOfRect(gameObjectData->tree[i].stumpRect);
		d22 = getDistance2BetweenPoints(p.x,p.y,x,y);
		if(d22 < d2 || tree == NULL){
			d2 = d22;
			tree = &gameObjectData->tree[i];
		}
		i++;
	}
	return tree;
}

int countResourceNodesInRadius(GameObjectData *gameObjectData, int x, int y, double radius){
	int count = 0;
	int i = 0, j;
	SDL_Point p;
	while(i < gameObjectData->resourceNodeSpawnerCount){
		if(getDistance2BetweenPoints(x,y,(int)gameObjectData->resourceNodeSpawners[i].xPosition,(int)gameObjectData->resourceNodeSpawners[i].yPosition) < radius * radius){
			j = 0;
			while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
				p = getCenterOfRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect);
				if(getDistance2BetweenPoints(x,y,p.x,p.y) < radius * radius){
					switch(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].type){
						case 0:
							count += 1;
							break;
						case 1:
							count += 2;
							break;
						case 2:
							count += 5;
							break;
            default:
              break;
					}
				}
				j++;
			}
		}
		i++;
	}
	return count;
}

ResourceNode *checkResourceNodeCollision(ResourceNodeSpawner **resourceNodeSpawnerPointer, GameObjectData *gameObjectData, ProgrammableWorker *programmableWorker){
	/* ResourceNodeSpawner **resourceNodeSpawnerPoint
									= this is a pointer to a pointer (I hate it
										too). We need it so we return both the
										ResourceNode pointer and the
										ResourceNodeSpawner pointer.
	 GameObjectData *gameObjectData = a pointer to the GameObjectData struct
										which we will be drawing object data from

	 This is probably the most complicated part of what I've done here. Rather
	 than checking collision against every ResourceNode we check against every
	 ResourceNodeSpawner and then if that comes out as a hit, then we check
	 against every ResourceNode attached to the ResourceNodeSpawner.

	 As an example, if we have 1000 ResourceSpawners with 1000 ResourceNodes
	 attached, doing it simply by checking ResourceNodes will be 1 million
	 checks at worst whilst this way will be 2000 checks at worst.*/
	int i = 0, j = 0;
	SDL_Point worker_p;
	SDL_Point node_p;
	double d2;
	worker_p = getCenterOfRect(programmableWorker->rect);
	/* Loop through all the resourceNodeSpawners in the gameObjectData we got
	 passed */
	while(i < gameObjectData->resourceNodeSpawnerCount){
		/* Get the distance2 between the resourceNodeSpawner and the point to check */
		/* This checks if we are close enough to a resourceNodeSpawner that we
			 should check against the individual nodes*/

		d2 = getDistance2BetweenPoints(worker_p.x,
											 					 worker_p.y,
											 				   (int)gameObjectData->resourceNodeSpawners[i].xPosition,
											 				   (int)gameObjectData->resourceNodeSpawners[i].yPosition);
		if(d2 <= (WORKER_SENSE_RANGE + gameObjectData->resourceNodeSpawners[i].spawnRadius) * (WORKER_SENSE_RANGE + gameObjectData->resourceNodeSpawners[i].spawnRadius)){
			/* Reset j every time */
			j = 0;
			/* Loop through the ResourceNodes on the resourceNodeSpawner and check
			 collisions */
			while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
				/* Get the distance2 between the ResourceNode and the point to check */
				/* Check whether we are close enough to a ResourceNode to collide and
					 that the node we're testing is actually alive.*/
				node_p = getCenterOfRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect);
				if(isPointInRect(node_p.x,node_p.y,programmableWorker->rect)
					&& gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){
						*resourceNodeSpawnerPointer = &gameObjectData->resourceNodeSpawners[i];
						return(&gameObjectData->resourceNodeSpawners[i].resourceNodes[j]);
				}
				else if(getDistance2BetweenPoints(node_p.x,node_p.y,worker_p.x,worker_p.y)){
					*resourceNodeSpawnerPointer = &gameObjectData->resourceNodeSpawners[i];
				}
				j++;
			}
		}
		i++;
	}

	return(NULL);
}

int countProgrammableWorkersInRange(GameObjectData *gameObjectData, SDL_Point center, double radius){
	int i = 0;
	ProgrammableWorker *worker = gameObjectData->first_programmable_worker;
	SDL_Point point;
	radius = radius * radius;
	while(worker != NULL){
	point = getCenterOfRect(worker->rect);
	if(getDistance2BetweenPoints(center.x,center.y,point.x,point.y) <= radius){
		i++;
	}
	worker = worker->next;
	}
	return i;
}

int isPointInRangeOf(SDL_Point point, SDL_Point center, double radius){
	radius = radius * radius;
	if(getDistance2BetweenPoints(center.x,center.y,point.x,point.y) <= radius){
		return 1;
	}
	return 0;
}

int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner){
	/* ResourceNodeSpawner *resourceNodeSpawner = the pointer to the
												ResourceNodeSpawner we're
												checking
	 This function checks to see where the first dead resourceNode is inside a
	 ResourceNodeSpawner and returns the first index that it finds where
	 the ResourceNode is dead.

	 It returns -1 if there is no dead ResourceNode. */
	int i = 0;
	/* Run through the attached ResourceNodes */
	while(i<resourceNodeSpawner->maximumNodeCount){
	/* And check if alive */
	if(resourceNodeSpawner->resourceNodes[i].alive == 0){
		/* If not, return the index */
		return(i);
	}
	i++;
	}
	/* We'll return -1 if we don't find one, but this shouldn't happen */
	return(-1);
}

void killAllBees(ProgrammableWorker **programmableWorker){
	ProgrammableWorker *p;
	while(*programmableWorker != NULL){

		p = *programmableWorker;
		*programmableWorker = (*programmableWorker)->next;

		free(p);
	}
}

void killProgrammableWorker(GameObjectData *gameObjectData, ProgrammableWorker **programmableWorker){

	ProgrammableWorker *p = NULL;
	if((*programmableWorker)->next == NULL){
		p = *programmableWorker;
	    for(*programmableWorker = gameObjectData->first_programmable_worker;
	    (*programmableWorker)->next != p ;
	    *programmableWorker = (*programmableWorker)->next){
	    }
	    (*programmableWorker)->next = NULL;
	    *programmableWorker = p;
		free(*programmableWorker);
		*programmableWorker = NULL;
	}else if(*programmableWorker == gameObjectData->first_programmable_worker){
		p = *programmableWorker;
		gameObjectData->first_programmable_worker = gameObjectData->first_programmable_worker->next;
		*programmableWorker = gameObjectData->first_programmable_worker;
		free(p);
	}else{
		p = *programmableWorker;
	    for(*programmableWorker = gameObjectData->first_programmable_worker;
	    (*programmableWorker)->next != p ;
	    *programmableWorker = (*programmableWorker)->next){
	    }
	    (*programmableWorker)->next = p->next;
	    *programmableWorker = p;
		free(*programmableWorker);
		*programmableWorker = NULL;
	}
}

ProgrammableWorker *createProgrammableWorker(GameObjectData *gameObjectData){
	/* This function creates a ProgrammableWorker struct and fills in the default
	 values. Many of these are defined in generic.h */
	ProgrammableWorker *programmableWorker = malloc(sizeof(ProgrammableWorker));
	ProgrammableWorker *p = gameObjectData->first_programmable_worker;

	if(p == NULL){
	gameObjectData->first_programmable_worker = programmableWorker;
	}
	else{
	for(p = gameObjectData->first_programmable_worker; p->next != NULL ; p = p -> next){}
	p -> next = programmableWorker;
	}

	/* Because we're using trigonometry to move things around, if work purely with
	 integers then Bad Things happen. Instead, we work first in doubles then
	 copy the information to the integers in the rectangle. */
	programmableWorker->rawX = X_SIZE_OF_WORLD/2;
	programmableWorker->rawY = Y_SIZE_OF_WORLD/2;
	programmableWorker->rect.x = 100;
	programmableWorker->rect.y = 100;
	programmableWorker->rect.w = X_SIZE_OF_WORKER;
	programmableWorker->rect.h = Y_SIZE_OF_WORKER;

	programmableWorker->beeStatus = (char*) calloc(LENGTH_OF_STATUS_STRING, sizeof(char));



	programmableWorker->wet_and_cant_fly = 0;
	programmableWorker->cold_and_about_to_die = 0;
	programmableWorker->flapTimer = rand() % 100;

	programmableWorker->stunned_after_sting = 0;
	programmableWorker->insideHive = 0;
	programmableWorker->fighting_spider = 0;



	programmableWorker->currentGraphicIndex = BEE_FLAP_GRAPHIC_1;
	/* heading is measured in radians because maths in C all take radians */
	programmableWorker->heading = 0.0;
	/* This is basically pixels/milliseconds */
	programmableWorker->speed = WORKER_SPEED;
	/* I don't think this is used yet... */
	programmableWorker->type = 1;
	programmableWorker->cargo = 0;
	programmableWorker->displayInfo = 0;
	/* This is an enum detailed in game_objects.h */
	programmableWorker->status = IDLE;
	programmableWorker->next = NULL;
	programmableWorker->brain.is_point_remembered = 0;
	programmableWorker->brain.followTarget = NULL;
	programmableWorker->brain.foundNode = NULL;
	programmableWorker->brain.aiStartPoint = 0;
	programmableWorker->brain.waitTime = -1;
	return(programmableWorker);
}

IceCreamPerson *createIceCreamPerson(void){
	IceCreamPerson *iceCreamPerson = (IceCreamPerson*) malloc(sizeof(IceCreamPerson));
	iceCreamPerson->rect.w = PERSON_HEIGHT;
	iceCreamPerson->rect.h = PERSON_WIDTH;
	/*set initial location to ensure no interaction or existence inside world*/
	/* (when the time is right for the person to appear, x y co-ordinates*/
	/*can be reassigned values that exist inside the world boundaries)*/
	iceCreamPerson->rect.x = X_SIZE_OF_WORLD * 2;
	iceCreamPerson->rect.y = Y_SIZE_OF_WORLD * 2;
	iceCreamPerson->currently_on_screen = 0;
	iceCreamPerson->has_ice_cream = 0;
	iceCreamPerson->speed = 0.2;
	iceCreamPerson->stung = 0;
	iceCreamPerson->displayInfo = 0;

	return iceCreamPerson;

}

DroppedIceCream *createDroppedIceCream(void){
	DroppedIceCream *droppedIceCream = (DroppedIceCream*) malloc(sizeof(DroppedIceCream));
	droppedIceCream->rect.w = DROPPED_ICECREAM_HEIGHT;
	droppedIceCream->rect.h = DROPPED_ICECREAM_WIDTH;
	/*set initial location to ensure no interaction or existence inside world*/
	/* (when the time is right for the person to appear, x y co-ordinates*/
	/*can be reassigned values that exist inside the world boundaries)*/
	droppedIceCream->dropped = 0;
	droppedIceCream->droppedTimer = 0;
	droppedIceCream->sizeOscillator = 1;
	droppedIceCream->displayInfo = 0;
	return droppedIceCream;
}

RoamingSpider *createRoamingSpider(void){
	RoamingSpider *roamingSpider = (RoamingSpider*) malloc(sizeof(RoamingSpider));
	roamingSpider->rect.w = PERSON_HEIGHT;
	roamingSpider->rect.h = PERSON_WIDTH;
	/*set initial location to ensure no interaction or existence inside world*/
	/* (when the time is right for the person to appear, x y co-ordinates*/
	/*can be reassigned values that exist inside the world boundaries)*/
	roamingSpider->rect.x = X_SIZE_OF_WORLD * 2;
	roamingSpider->rect.y = Y_SIZE_OF_WORLD * 2;
	roamingSpider->currently_on_screen = 0;
	roamingSpider->eating_bee = 0;
	roamingSpider->eating_bee_complete = 0;
	roamingSpider->speed = SPIDER_SPEED;
	roamingSpider->stung = 0;
	roamingSpider->displayInfo = 0;
	roamingSpider->ticksSinceEating = 0;
	roamingSpider->ticksSinceStung = 0;
	roamingSpider->ticksSinceDead = 0;
	roamingSpider->deadSpider = 0;
	
	roamingSpider->countDownToStride = SPIDER_STRIDE_FREQUENCY;
	
	
	return roamingSpider;

}

Weather createWeatherLayer(void){
	/* This function creates a Weather struct and fills in the default
	 values. Many of these are defined in generic.h */
	Weather weather;
	weather.tickCount = 0;
	weather.present_weather = Sun;
	return(weather);
}


Hive createHive(void){
	/* This function creates a Hive struct and fills in the default
	 values. Many of these are defined in generic.h */
	Hive hive;
	int i = 0;
	hive.rect.w = X_SIZE_OF_HIVE;
	hive.rect.h = Y_SIZE_OF_HIVE;
	hive.rect.x = (X_SIZE_OF_WORLD/2 - hive.rect.w/2);
	hive.rect.y = (Y_SIZE_OF_WORLD/2 - hive.rect.h/2);
	hive.displayInfo = 0;
	hive.flowers_collected = 0;
	hive.winterCountdown = MAX_DAYS_TO_WINTER;
	hive.scoreBeforeWinter = 0;
	hive.years_survived = 0;
	hive.delayBeforeSummer = DELAY_BEFORE_SUMMER;
	hive.winterCountdownFloat = MAX_DAYS_TO_WINTER;
	while(i < NUMBER_OF_CELLS_IN_HIVE){
		hive.hiveCells[i].timer = HIVE_CELL_SPAWN_DELAY;
		i++;
	}
	return(hive);
}

Tree createTree(Hive *hive, int forceX, int forceY){
	Tree tree;

	tree.rect.w = SIZE_OF_TREE;
	tree.rect.h = SIZE_OF_TREE;
	tree.stumpRect.w = SIZE_OF_TREESTUMP;
	tree.stumpRect.h = SIZE_OF_TREESTUMP;
	tree.rect.x = forceX;
	tree.rect.y = forceY;
	/*if tree is too close to hive, rellocate*/
	while(isPointInRangeOf(getCenterOfRect(tree.rect), getCenterOfRect(hive->rect), 100)){

		if(rand()%2){
			tree.rect.x += SIZE_OF_TREE;
		}else{
			tree.rect.x -= SIZE_OF_TREE;
		}

		if(rand()%2){
			tree.rect.y += SIZE_OF_TREE;
		}else{
			tree.rect.y -= SIZE_OF_TREE;
		}
	}
	tree.stumpRect.x = tree.rect.x + (SIZE_OF_TREE / 2) - (SIZE_OF_TREESTUMP / 2);
	tree.stumpRect.y = tree.rect.y + (SIZE_OF_TREE / 2) - (SIZE_OF_TREESTUMP / 2);
	tree.bees_taking_shelter = 0;
	tree.displayInfo = 0;
	tree.currentGraphicIndex = 0;
	return tree;
}

int updateProgrammableWorker(ProgrammableWorker **programmableWorkerP, GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks){
	/* ProgrammableWorker *programmableWorker = the ProgrammableWorker we're going
												to be updating
	 GameObjectData *gameObjectData		 = the pointer to the GameObjectData
												that we will be using when
												determining collision
	 float ticks							= The number of ticks to update for.*/
	double newX,newY;
	double pX,pY;
	ResourceNodeSpawner *resourceNodeSpawner = NULL;
	ResourceNode *resourceNode = NULL;
	char announcement[256];
	ProgrammableWorker *programmableWorker = *programmableWorkerP;

		char tempString[255];

	switch(programmableWorker->status){
		case LEAVING:
			programmableWorker->beeStatus = "Leaving hive";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
			break;
		case RETURNING:
			programmableWorker->beeStatus = "Returning to hive";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
			break;
		case WANTING_TO_RETURN:
			programmableWorker->beeStatus = "Wants to return";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
			break;
		case NODE_FOUND:
			programmableWorker->beeStatus = "Found resource";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
			break;
		case IDLE:
			programmableWorker->beeStatus = "Idle";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
			break;
		default:
			programmableWorker->beeStatus = "Blank";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
			break;
	}


	if(gameObjectData->hive.winterCountdown < WINTER_THRESHOLD){
		if(!isPointInRangeOf(getCenterOfRect(programmableWorker->rect), getCenterOfRect(gameObjectData->hive.rect),
		HIVE_SHELTER_RADIUS)){

			programmableWorker->cold_and_about_to_die++;
			programmableWorker->beeStatus = "Freezing / dieing";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
		}
	}
	else{
		if(isPointInRangeOf(getCenterOfRect(programmableWorker->rect), getCenterOfRect(gameObjectData->hive.rect),HIVE_SHELTER_RADIUS)){
			programmableWorker->insideHive = 1;
			programmableWorker->beeStatus = "Inside hive";
			if(programmableWorker->displayInfo){
				sprintf(tempString," Status: %s", programmableWorker
				->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
		}
		else{
			programmableWorker->insideHive = 0;
		}
		programmableWorker->cold_and_about_to_die = 0;

		if(!programmableWorker->wet_and_cant_fly){ /*programmable worker has not been caught in rain recently*/
			programmableWorker->flapTimer += ticks;
			if(programmableWorker->flapTimer > MS_BETWEEN_FLAPPING){
				programmableWorker->flapTimer -= MS_BETWEEN_FLAPPING;
				programmableWorker->currentGraphicIndex = (programmableWorker->currentGraphicIndex + 1) % 2;
			}

			if(gameObjectData->weather.present_weather == Rain){
				int i, j = 0;
				for(i = 0; i < NUMBER_OF_TREES; i++){
					if(!testRectIntersection(programmableWorker->rect, gameObjectData->tree[i].rect)){
						j++;
					}
				}
				if(j == NUMBER_OF_TREES){
					programmableWorker->beeStatus = "Sheltered / under tree";

					if(programmableWorker->displayInfo){
						sprintf(tempString," Status: %s", programmableWorker
						->beeStatus);
						setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
					}
				}
				if(!programmableWorker->insideHive){
					j++;
				}
				if(j == NUMBER_OF_TREES + 1 /* + 1 includes the hive as shelter*/){
					programmableWorker->wet_and_cant_fly =  rand() % 100 < CHANCE_OF_FALLING_IN_RAIN ? 0 : programmableWorker->wet_and_cant_fly;/*true*/
				}
			}

			if(programmableWorker->stunned_after_sting){
				programmableWorker->beeStatus = "Stunned after sting";

				if(programmableWorker->displayInfo){
					sprintf(tempString," Status: %s", programmableWorker
					->beeStatus);
					setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
				}

				programmableWorker->heading += (rand()%3)-1;

				if(programmableWorker->stunned_after_sting++ > STUNNED_AFTER_STING_DURATION){
					programmableWorker->stunned_after_sting = 0;

				}

			}

			/* Because we're using a heading/velocity movement system here, we have to use
			some trigonometry to work out the new positions */
			newX = sin(programmableWorker->heading);
			newY = cos(programmableWorker->heading);
			newX *= programmableWorker->speed * ticks * (programmableWorker->cargo > 0 ? PROGRAMMABLE_WORKER_CARGO_SPEED_FACTOR : 1.0);
			newY *= programmableWorker->speed * ticks * (programmableWorker->cargo > 0 ? PROGRAMMABLE_WORKER_CARGO_SPEED_FACTOR : 1.0);;

			/* These are then the tentative final new positions for the ProgrammableWorker
			we're updating */
			programmableWorker->rawX += newX;
			programmableWorker->rawY += newY;
			programmableWorker->rect.x = (int)floor(programmableWorker->rawX);
			programmableWorker->rect.y = (int)floor(programmableWorker->rawY);


			if(!programmableWorker->stunned_after_sting){
				resourceNode = checkResourceNodeCollision(&resourceNodeSpawner,gameObjectData,programmableWorker);
			}



			if(programmableWorker->status == IDLE){
				if(getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect) > 100 * 100){
					pX = (double)(gameObjectData->hive.rect.x + gameObjectData->hive.rect.w/2 - 100 + rand() % 200);
					pY = (double)(gameObjectData->hive.rect.y + gameObjectData->hive.rect.h/2 - 100 + rand() % 200);
					programmableWorker->heading = atan2(pX - programmableWorker->rawX,pY - programmableWorker->rawY);
				}
			}
			else if(getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect) < 50.0 && programmableWorker->status
			== RETURNING){
				if(programmableWorker->cargo > 0){
					gameObjectData->hive.flowers_collected += programmableWorker->cargo;
					programmableWorker->cargo = 0;

					if(gameObjectData->hive.flowers_collected % CELEBRATION_THRESHOLD == 0){
						playSoundEffect(2, &gameObjectData->audioData, "returnFlower");
						sprintf(announcement,"Congratulations you have collected %d flowers!",gameObjectData->hive.flowers_collected);
						announce(announcementsData, announcement);
					}
				}

				programmableWorker->status = IDLE;
			}
			else if(programmableWorker->status == LEAVING){
  				/* status being 1 means that the bee heading away from the center */

  				/*if programmable worker is clsoe enough to dropped icecream, it is not the very same second that the person dropped it*/
  				/* then the worker picks up the iceCream*/
  				if(!programmableWorker->stunned_after_sting){

					if(isPointInRangeOf(getCenterOfRect(programmableWorker->rect),
					getCenterOfRect(gameObjectData->droppedIceCream->rect),
					(double)ICECREAM_PICKUP_RADIUS) && gameObjectData->droppedIceCream->dropped){

						programmableWorker->cargo += SUGAR_VALUE_OF_ICECREAM;
						gameObjectData->droppedIceCream->dropped = 0;
						gameObjectData->droppedIceCream->droppedTimer = 0;


					}else{
  						/* We want to get back the ResourceNode and ResourceNodeSpawner (if any)
  						that we are colliding with */
  						/* resourceNode will be NULL if there are no collisions, so test that */
  						if(resourceNode != NULL){
  			  				/* Kill the ResourceNode */
  			  				resourceNode->alive = 0;
  			  				/* Make sure the ResourceNodeSpawner knows that it's lost a ResourceNode */
  			  				resourceNodeSpawner->currentNodeCount--;
									switch(resourceNode->type){
										case 0:
											programmableWorker->cargo = SUGAR_VALUE_OF_BLUE_FLOWER;
											break;
										case 1:
											programmableWorker->cargo = SUGAR_VALUE_OF_RED_FLOWER;
											break;
										case 2:
											programmableWorker->cargo = SUGAR_VALUE_OF_YELLOW_FLOWER;
											break;
                    default:
                      break;
									}
  	  						programmableWorker->brain.foundNode = NULL;
  						}
  					}

  				}

			}
		}
		else{ /*programmable worker has been caught in rain and hasn't regained ability to fly yet*/
			int flapChance;
			programmableWorker->beeStatus = "Wet, can't fly";
			if(programmableWorker->displayInfo){
					sprintf(tempString," Status: %s", programmableWorker
					->beeStatus);
					setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
			}
			if(!(rand() % CHANCE_OF_REGAINING_FLIGHT)){
				programmableWorker->wet_and_cant_fly = 0;

			}
			if((flapChance = rand() % 100)){
				if(!(rand() % flapChance)){
				   programmableWorker->currentGraphicIndex = (programmableWorker->currentGraphicIndex + 1) % 2;
				}
			}
		}

		/*for battling with enemies*/
		if(isPointInRangeOf(getCenterOfRect(programmableWorker->rect), getCenterOfRect(gameObjectData->roamingSpider->rect),SPIDER_ATTACK_AREA)) {

			if(!gameObjectData->roamingSpider->stung){
				announce(announcementsData,"A spider is attacking one of your bees!");
				programmableWorker->beeStatus = "Fighting a spider!";
				if(programmableWorker->displayInfo){
						sprintf(tempString," Status: %s", programmableWorker
						->beeStatus);
						setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
				}
				programmableWorker->fighting_spider = 1;
				programmableWorker->wet_and_cant_fly = 1;
			}
			/*bee freeing his friend*/
			if(gameObjectData->roamingSpider->stung == 1) {
				programmableWorker->fighting_spider = 0;
				programmableWorker->stunned_after_sting = 1;
			}
			
			if(gameObjectData->roamingSpider->eating_bee_complete == 1) {
				killProgrammableWorker(gameObjectData, &programmableWorker);
				printf("DEBUG: A BEE HAS BEEN EATEN (BEE FUNCTION)\n\n");
				return 1;
			}

		}

		  /* Sensory Perception */
		if(programmableWorker->brain.foundNode != NULL &&
		(getDistance2BetweenRects(programmableWorker->rect,programmableWorker->brain.foundNode->rect) >= WORKER_SENSE_RANGE * WORKER_SENSE_RANGE ||
		!programmableWorker->brain.foundNode->alive)){
		   programmableWorker->brain.foundNode = NULL;
		}
		if(programmableWorker->brain.foundNode == NULL && resourceNodeSpawner != NULL){
			 resourceNode = chooseNodeRandomly(resourceNodeSpawner);
			 if(resourceNode != NULL && getDistance2BetweenRects(programmableWorker->rect,resourceNode->rect) < WORKER_SENSE_RANGE * WORKER_SENSE_RANGE){
			   programmableWorker->brain.foundNode = resourceNode;
			 }
		}
	}

	if(programmableWorker->cargo != 0){
		programmableWorker->speed = WORKER_SPEED/1.5;
		if(programmableWorker->displayInfo){
				sprintf(tempString,"%s %s",programmableWorker->beeStatus , "& has cargo");
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, tempString, STATUS);
		}
	}else{
		programmableWorker->speed = WORKER_SPEED;
	}
	*programmableWorkerP = programmableWorker;
	return 0;
}

void centerViewOnHive(GraphicsData *graphicsData, GameObjectData *gameObjectData){
	SDL_Point p = getCenterOfRect(gameObjectData->hive.rect);
	int win_x, win_y;
	SDL_GetWindowSize(graphicsData->window,&win_x,&win_y);
	p.x -= win_x/2;
	p.y -= win_y/2;
	setNavigationOffset(graphicsData, p.x, p.y);
}


void updateIceCreamPerson(GameObjectData *gameObjectData, int ticks){

	double newX,newY;
	int distanceFromYBorder;
	int distanceFromXBorder;
	ProgrammableWorker *programmableWorker;

	/*set iceCreamPerson to going home if sun has gone, or he has lost his ice cream*/
	if(!(gameObjectData->weather.present_weather == Sun &&
	gameObjectData->iceCreamPerson->has_ice_cream)){
		gameObjectData->iceCreamPerson->going_home = 1;
	}
	/*set iceCreamPerson to running if it is raining*/
	if(gameObjectData->weather.present_weather == Rain){
		gameObjectData->iceCreamPerson->speed = 0.1;
	}else{
  		/*if he doesn't have his icecream (he's been stung) he'll keep running even when rain stops*/
  		if(gameObjectData->iceCreamPerson->has_ice_cream){
    		gameObjectData->iceCreamPerson->speed = 0.05;
   	    }
	}

	/*set iceCreamPerson->currently_on_screen to false if he has walked off screen*/
	if(gameObjectData->iceCreamPerson->xPosition > (X_SIZE_OF_WORLD + PERSON_WIDTH) ||
	gameObjectData->iceCreamPerson->yPosition > (Y_SIZE_OF_WORLD + PERSON_HEIGHT) ||
	gameObjectData->iceCreamPerson->xPosition < 0 - (PERSON_WIDTH*2) ||
	gameObjectData->iceCreamPerson->yPosition < 0 - (PERSON_HEIGHT*2)){
	gameObjectData->iceCreamPerson->currently_on_screen = 0;
	}

	/*decrement countDownToStride*/
	gameObjectData->iceCreamPerson->countDownToStride--;

	/*if countDownToStride equals zero, reset count, and change stride image*/
	if(gameObjectData->iceCreamPerson->countDownToStride <= 0){
		gameObjectData->iceCreamPerson->countDownToStride =
        (int)((double)STRIDE_FREQUENCY / gameObjectData->iceCreamPerson->speed);


		switch(gameObjectData->iceCreamPerson->currentGraphicIndex){
			case 0:
				gameObjectData->iceCreamPerson->currentGraphicIndex = WITH_ICE_CREAM_STRIDE2;
			break;
			case 1:
				gameObjectData->iceCreamPerson->currentGraphicIndex = WITH_ICE_CREAM_STRIDE1;
				break;
      default:
        break;
		}
	}

	/*use trig to find new locations based on heading angle (radians)*/
	newX = sin(gameObjectData->iceCreamPerson->heading);
	newY = cos(gameObjectData->iceCreamPerson->heading);
	newX *= gameObjectData->iceCreamPerson->speed * ticks;
	newY *= gameObjectData->iceCreamPerson->speed * ticks;

	/*update new position*/
	gameObjectData->iceCreamPerson->xPosition += newX;
	gameObjectData->iceCreamPerson->yPosition += newY;
	gameObjectData->iceCreamPerson->rect.x = (int)floor(gameObjectData->iceCreamPerson->xPosition);
	gameObjectData->iceCreamPerson->rect.y = (int)floor(gameObjectData->iceCreamPerson->yPosition);

	/*if iceCreamPerson not yet stung, and bee is close enough to sting*/
	/*STING_HIT_RADIUS can be made so small that this would never actually happen unless it's behaviour*/
	/*that is explicitly programmed into the bees, or (as it currently is) bit enough to happy on an occatinal collision*/
	for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL;
	programmableWorker = programmableWorker->next){

  		if(isPointInRangeOf(getCenterOfRect(programmableWorker->rect),
  		getCenterOfRect(gameObjectData->iceCreamPerson->rect), STING_HIT_RADIUS) != 0){

	  		gameObjectData->iceCreamPerson->stung = 1;
	  		/*drop iceCream!*/
	  		gameObjectData->iceCreamPerson->has_ice_cream = 0;

	  		gameObjectData->droppedIceCream->xPosition = gameObjectData->iceCreamPerson->xPosition;
	  		gameObjectData->droppedIceCream->yPosition = gameObjectData->iceCreamPerson->yPosition;

		  	gameObjectData->droppedIceCream->dropped = 1;

	  		/*run for your life!*/
	  		gameObjectData->iceCreamPerson->speed = 0.1;

	  		/*set programmable worker to stunned after sting*/
	  		programmableWorker->stunned_after_sting = 1;

  		}

  	}

	if(countProgrammableWorkersInRange(gameObjectData, getCenterOfRect(gameObjectData->iceCreamPerson->rect), 250.0) == 0 && !gameObjectData->iceCreamPerson->going_home){

		if(gameObjectData->iceCreamPerson->xPosition >= X_SIZE_OF_WORLD - PERSON_WIDTH/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->iceCreamPerson->heading = 1.571;

		}else if(gameObjectData->iceCreamPerson->xPosition <= -PERSON_WIDTH/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->iceCreamPerson->heading = 4.713;

		}else if(gameObjectData->iceCreamPerson->yPosition >= Y_SIZE_OF_WORLD - PERSON_HEIGHT/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->iceCreamPerson->heading = 3.142;

		}else if(gameObjectData->iceCreamPerson->yPosition <= -PERSON_HEIGHT/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->iceCreamPerson->heading = 0;

		}else if(rand() % 1000 == 0){
			/*randomly change direction, just for the hell of it*/
	 		gameObjectData->iceCreamPerson->heading += ((double)(rand() % 30) / (double)10) - 1.5;
	 	}

	}else{
		distanceFromYBorder = (int)(gameObjectData->iceCreamPerson->yPosition - Y_SIZE_OF_WORLD/2);
		distanceFromXBorder = (int)(gameObjectData->iceCreamPerson->xPosition - X_SIZE_OF_WORLD/2);
		if(abs(distanceFromXBorder) > abs(distanceFromYBorder)){
			if(distanceFromXBorder > 0){
				gameObjectData->iceCreamPerson->heading = PI * 0.75 + 0.069813;
			}else{
				gameObjectData->iceCreamPerson->heading = PI * 0.25 + 0.069813;
			}
		}else{
			if(distanceFromYBorder > 0){
				gameObjectData->iceCreamPerson->heading = 0 + 0.069813;
			}else{
				gameObjectData->iceCreamPerson->heading = PI * 0.5 + 0.069813;
			}
		}
	}


}

void updateRoamingSpider(GameObjectData *gameObjectData, int ticks){

	double newX,newY;
	int distanceFromYBorder;
	int distanceFromXBorder;

	/*set roamingSpider->currently_on_screen to false if he has walked off screen*/
	if(gameObjectData->roamingSpider->xPosition > X_SIZE_OF_WORLD ||
	gameObjectData->roamingSpider->yPosition > Y_SIZE_OF_WORLD ||
	gameObjectData->roamingSpider->xPosition < 0 - PERSON_WIDTH ||
	gameObjectData->roamingSpider->yPosition < 0 - PERSON_HEIGHT){
	gameObjectData->roamingSpider->currently_on_screen = 0;
	}

	/*decrement countDownToStride*/
	gameObjectData->roamingSpider->countDownToStride -= ticks;

	if(gameObjectData->roamingSpider->deadSpider != 1) {	
		/*if countDownToStride equals zero, reset count, and change stride image*/
		if(gameObjectData->roamingSpider->countDownToStride <= 0){
			gameObjectData->roamingSpider->countDownToStride =
			SPIDER_STRIDE_FREQUENCY*100;

			switch(gameObjectData->roamingSpider->currentGraphicIndex){
				case SPIDER2:
					gameObjectData->roamingSpider->currentGraphicIndex = SPIDER15;
					break;
				case SPIDER:
					gameObjectData->roamingSpider->currentGraphicIndex = SPIDER15b;
					break;
				case SPIDER15:
					gameObjectData->roamingSpider->currentGraphicIndex = SPIDER;
					break;
				case SPIDER15b:
					gameObjectData->roamingSpider->currentGraphicIndex = SPIDER2;
					break;
				case SPIDER_FIGHTING:
					gameObjectData->roamingSpider->currentGraphicIndex = SPIDER;
					break;
			}
		}
		
	/*}else if(gameObjectData->roamingSpider->eating_bee){
		gameObjectData->roamingSpider->currentGraphicIndex = SPIDER_FIGHTING;*/
	}else{
		gameObjectData->roamingSpider->currentGraphicIndex = SPIDER_DEAD;
	}

	/*use trig to find new locations based on heading angle (radians)*/
	newX = sin(gameObjectData->roamingSpider->heading);
	newY = cos(gameObjectData->roamingSpider->heading);
	newX *= gameObjectData->roamingSpider->speed * ticks;
	newY *= gameObjectData->roamingSpider->speed * ticks;

	/*update new position*/
	gameObjectData->roamingSpider->xPosition += newX;
	gameObjectData->roamingSpider->yPosition += newY;
	gameObjectData->roamingSpider->rect.x = (int)floor(gameObjectData->roamingSpider->xPosition);
	gameObjectData->roamingSpider->rect.y = (int)floor(gameObjectData->roamingSpider->yPosition);
	
	if(gameObjectData->roamingSpider->deadSpider != 1) {	
		/*if roamingSpider not yet stung, and bee is close enough to sting*/
		/*STING_HIT_RADIUS can be made so small that this would never actually happen unless it's behaviour*/
		/*if any bees are within the attack radius, the state of the spider is set to eating bees.*/
		switch(countProgrammableWorkersInRange(gameObjectData,getCenterOfRect(gameObjectData->roamingSpider->rect), SPIDER_ATTACK_AREA)){
			case 0:
				if(gameObjectData->roamingSpider->eating_bee_complete){
					gameObjectData->roamingSpider->eating_bee_complete = 0;
			  		/*spider starts moving to having finished his tasty snack*/
					gameObjectData->roamingSpider->speed = SPIDER_SPEED;
				}
				break;
				
			case 1:
				if(!gameObjectData->roamingSpider->stung){
					/*eat bee!*/
					gameObjectData->roamingSpider->eating_bee = 1;
			  		/*spider stops moving to eat*/
					gameObjectData->roamingSpider->speed = 0.0;
				}
				break;
				
			case 2:
				gameObjectData->roamingSpider->stung = 1;
				/*stop eating bee*/
				gameObjectData->roamingSpider->eating_bee = 0;
				/*spider back to movingn*/
				gameObjectData->roamingSpider->speed = SPIDER_SPEED;
				gameObjectData->roamingSpider->ticksSinceEating = 0;
				break;
				
			case 3:
				gameObjectData->roamingSpider->stung = 1;
				/*stop eating bee*/
				gameObjectData->roamingSpider->eating_bee = 0;
				/*spider back to movingn*/
				gameObjectData->roamingSpider->speed = SPIDER_SPEED;
				gameObjectData->roamingSpider->ticksSinceEating = 0;
				break;
				
			default: /*If there are more than 3 bees converging on the spider he dies*/
				gameObjectData->roamingSpider->speed = 0.0;	
				gameObjectData->roamingSpider->deadSpider = 1;
		}
		

	
		/*fight duration / kill time */
		if(gameObjectData->roamingSpider->eating_bee == 1) {
			gameObjectData->roamingSpider->ticksSinceEating += ticks;
			if(gameObjectData->roamingSpider->ticksSinceEating >= 10000) {
				gameObjectData->roamingSpider->eating_bee_complete = 1;
				gameObjectData->roamingSpider->eating_bee = 0;	
				gameObjectData->roamingSpider->ticksSinceEating = 0;			
				printf("DEBUG: A BEE HAS BEEN EATEN (SPIDER FUNCTION)\n");
		
			}
		}
		
		/*stung duration / time till active again */
		if(gameObjectData->roamingSpider->stung == 1) {
			gameObjectData->roamingSpider->ticksSinceStung += ticks;
			if(gameObjectData->roamingSpider->ticksSinceStung >= 20000) {
				gameObjectData->roamingSpider->stung = 0;
				gameObjectData->roamingSpider->ticksSinceStung = 0;			
				printf("DEBUG: SPIDER UNSTUNG AND ACTIVE AGAIN (SPIDER FUNCTION)\n");
			}
		}
		
	}else {
		gameObjectData->roamingSpider->ticksSinceDead += ticks;
		if(gameObjectData->roamingSpider->ticksSinceDead >= 60000) {
			printf("DEBUG: SPIDER REINITIALIZING\n");
			reInitialiseRoamingSpider(gameObjectData->roamingSpider);
		}
	}


	if(countProgrammableWorkersInRange(gameObjectData, getCenterOfRect(gameObjectData->roamingSpider->rect), 250.0) <= 3 && !gameObjectData->roamingSpider->going_home){

		if(gameObjectData->roamingSpider->xPosition >= X_SIZE_OF_WORLD - PERSON_WIDTH/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->roamingSpider->heading = 1.571;

		}else if(gameObjectData->roamingSpider->xPosition <= -PERSON_WIDTH/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->roamingSpider->heading = 4.713;

		}else if(gameObjectData->roamingSpider->yPosition >= Y_SIZE_OF_WORLD - PERSON_HEIGHT/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->roamingSpider->heading = 3.142;

		}else if(gameObjectData->roamingSpider->yPosition <= -PERSON_HEIGHT/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->roamingSpider->heading = 0;

		}else if(rand() % 500 == 0){
			/*randomly change direction, just for the hell of it*/
	 		gameObjectData->roamingSpider->heading += ((double)(rand() % 30) / (double)10) - 1.5;
	 	}

	}else{
		distanceFromYBorder = (int)(gameObjectData->roamingSpider->yPosition - Y_SIZE_OF_WORLD/2);
		distanceFromXBorder = (int)(gameObjectData->roamingSpider->xPosition - X_SIZE_OF_WORLD/2);
		if(abs(distanceFromXBorder) > abs(distanceFromYBorder)){
			if(distanceFromXBorder > 0){
				gameObjectData->roamingSpider->heading = PI * 0.75 + 0.069813;
			}else{
				gameObjectData->roamingSpider->heading = PI * 0.25 + 0.069813;
			}
		}else{
			if(distanceFromYBorder > 0){
				gameObjectData->roamingSpider->heading = 0 + 0.069813;
			}else{
				gameObjectData->roamingSpider->heading = PI * 0.5 + 0.069813;
			}
		}
	}


}

ResourceNodeSpawner createResourceNodeSpawner(GameObjectData *gameObjectData,int maximumNodeCount, float xPosition, float yPosition, float radius){
	/* int maximumNodeCount = the maximum number of ResourceNodes for this spawner
							to create
	 float xPosition		= the x coordinates that the spawner will be created
							at.
	 float yPosition		= like above.
	 float radius		 = the radius around the ResourceNodeSpawner that we
							want it to spawn ResourceNodes in

	 This function will create a new ResourceNodeSpawner for us to use. Values
	 here can be found in generic.h.
	 Because we use a calloc here, we'll need to be sure to clean it up!*/
	ResourceNodeSpawner resourceNodeSpawner;
	int i = 0;

	resourceNodeSpawner.maximumNodeCount = maximumNodeCount + rand() % maximumNodeCount;
	/* Set the currentNodeCount to 0 because no nodes have been spawned yet */
	resourceNodeSpawner.currentNodeCount = 0;

	resourceNodeSpawner.xPosition = xPosition;
	resourceNodeSpawner.yPosition = yPosition;

	/* Set the ticksSinceSpawn to 0 because we haven't spawned anything yet */
	resourceNodeSpawner.spawnDelay = DEFAULT_SPAWNDELAY;
	resourceNodeSpawner.ticksSinceSpawn = rand() % resourceNodeSpawner.spawnDelay;
	resourceNodeSpawner.spawnRadius = radius + rand() % RANDOMISE_SPAWNRADIUS;
	resourceNodeSpawner.collisionRect.x = (int)floor(xPosition - radius/2);
	resourceNodeSpawner.collisionRect.y = (int)floor(yPosition - radius/2);
	resourceNodeSpawner.collisionRect.w = (int)floor(radius);
	resourceNodeSpawner.collisionRect.h = (int)floor(radius);

	/* calloc up an array for us to use here */
	resourceNodeSpawner.resourceNodes = calloc((size_t)resourceNodeSpawner.maximumNodeCount, sizeof(ResourceNode));
	while(i<resourceNodeSpawner.maximumNodeCount){
	/* run through it and init the resourceNodes */
		initResourceNode(&resourceNodeSpawner.resourceNodes[i]);
	 if(rand() % 2 != 0){
		 resourceNodeSpawner.resourceNodes[i] = createResourceNode(gameObjectData,&resourceNodeSpawner,0);
	 }
	 i++;
	}
	return(resourceNodeSpawner);
}

void initResourceNode(ResourceNode *resourceNode){
	/* ResourceNodeSpawner *node = the pointer to the node we are initialising

	 Initialising resourceNodes makes sure that we can be sure of what the
	 values in the memory for the resourceNode are before they are formally
	 created. There was a weird bug where it would crash testing whether things
	 are alive without this.*/
	resourceNode->alive = 0;
	resourceNode->resourceUnits = 0;
	resourceNode->type = 0;
	resourceNode->rect.x = 0;
	resourceNode->rect.y = 0;
	resourceNode->rect.w = X_SIZE_OF_NODE;
	resourceNode->rect.h = Y_SIZE_OF_NODE;
}

void updateResourceNodeSpawner(GameObjectData *gameObjectData, ResourceNodeSpawner *spawner, int ticks){
	/* ResourceNodeSpawner *spawner = the spawner that we are updating
	 float ticks = the number of ticks we are updating over.

	 This function will just update a ResourceNodeSpawner as part of the main
	 game loop. */
	int i = 0;
	ResourceNode resNode;
	/* If the amount of nodes attached to the ResourceNodeSpawner is less than
	 the maximumNode count, then we want to spawn more in. */
	if(spawner->currentNodeCount < spawner->maximumNodeCount){

	/* We use a timer to delay respawning ResourceNodes, so add the new ticks
		 to it. */
	spawner->ticksSinceSpawn += ticks;
	if(spawner->ticksSinceSpawn > spawner->spawnDelay){
		/* Now we spawn in a new ResourceNode to replace the dead one */
		/* get the index we're replacing */
		i = getFirstDeadResourceNode(spawner);
		/* create the new ResourceNode */
		resNode = createResourceNode(gameObjectData,spawner,DEFAULT_RESOURCEUNITS);
		/* attach it to the spawner by inserting it into the resourceNodes array */
		spawner->resourceNodes[i] = resNode;
		/* Set the timer back to 0 */
		spawner->ticksSinceSpawn = 0;
		/* Update the currentNodeCount */
		spawner->currentNodeCount++;
	}
	}
}

void updateWeather(GameObjectData *gameObjectData, AudioData *audioData, Weather *weather, int ticks){
	/* Advance weather every TICKSPERWEATHER ticks; this may be semi-random due to tick-skipping. */
	int weatherChannel = 3;
	int ticksPerWeather;

	weather->tickCount += ticks;

	if(weather->tickCount > TICKSPERWEATHER && !gameObjectData->pause_status){
		weather->tickCount = 0;

		switch (weather->present_weather)
		{
		/* Closing the Window will exit the program */
		case Sun:
			weather->present_weather = (rand() % CHANCE_OF_CLOUD == 0) ? Cloud : Sun;
			break;
		case Cloud:
			weather->present_weather = (rand() % CHANCE_OF_RAIN == 0) ? Rain : Sun;
			fadeInChannel(weatherChannel, &gameObjectData->audioData, "thunder");
			audioData->weatherSoundActive = 1;
			break;
		case Rain:
			weather->present_weather = (rand() % CHANCE_OF_CLOUD == 0) ? Cloud : Sun;
			fadeOutChannel(weatherChannel, audioData);
			audioData->weatherSoundActive = 0;
			break;
		case Snow:
			/*honey stocks should be built up first. WINTER IS COMING.. (haha game of drones).*/
			break;
		default:
			fprintf(stderr,"Weather wasn't recognised in updateWeather.\n");
			fflush(stderr);
			exit(1);
		}
	}

	if(gameObjectData->gameOver){
		weather->present_weather = Snow;
	}
}

void reInitialiseIceCreamPerson(IceCreamPerson *iceCreamPerson){
	iceCreamPerson->currently_on_screen = 1;



	/*random chance of appearing at the edge of the world*/
	if(rand()%2){
		if(rand()%2){
			iceCreamPerson->xPosition = X_SIZE_OF_WORLD - (PERSON_WIDTH*2);
			iceCreamPerson->heading = 1.571;
		}else{
			iceCreamPerson->xPosition = 0 - PERSON_WIDTH;
			iceCreamPerson->heading = 4.713;
		}
		iceCreamPerson->yPosition = rand() % Y_SIZE_OF_WORLD;

	}else{
		if(rand()%2){
			iceCreamPerson->yPosition = Y_SIZE_OF_WORLD - (PERSON_HEIGHT*2);
			iceCreamPerson->heading = 3.142;
		}else{
			iceCreamPerson->yPosition = 0 - PERSON_HEIGHT;
			iceCreamPerson->heading = 0;
		}
		iceCreamPerson->xPosition = rand() % X_SIZE_OF_WORLD;
	}

	iceCreamPerson->rect.x = iceCreamPerson->xPosition;
	iceCreamPerson->rect.y = iceCreamPerson->yPosition;

	iceCreamPerson->has_ice_cream = 1;
	iceCreamPerson->going_home = 0;
	iceCreamPerson->speed = 0.2; /*pixels per millisecond*/
	iceCreamPerson->stung = 0;

	iceCreamPerson->countDownToStride = (double)STRIDE_FREQUENCY / iceCreamPerson->speed;

	iceCreamPerson->currentGraphicIndex = 0;





}

void reInitialiseRoamingSpider(RoamingSpider *roamingSpider){
	roamingSpider->currently_on_screen = 1;



	/*random chance of appearing at the edge of the world*/
	if(rand()%2){
		if(rand()%2){
			roamingSpider->xPosition = X_SIZE_OF_WORLD - PERSON_WIDTH;
			roamingSpider->heading = 1.571;
		}else{
			roamingSpider->xPosition = 0;
			roamingSpider->heading = 4.713;
		}
		roamingSpider->yPosition = rand() % Y_SIZE_OF_WORLD;

	}else{
		if(rand()%2){
			roamingSpider->yPosition = Y_SIZE_OF_WORLD - PERSON_HEIGHT;
			roamingSpider->heading = 3.142;
		}else{
			roamingSpider->yPosition = 0;
			roamingSpider->heading = 0;
		}
		roamingSpider->xPosition = rand() % X_SIZE_OF_WORLD;
	}

	roamingSpider->rect.x = (int)(roamingSpider->xPosition);
	roamingSpider->rect.y = (int)(roamingSpider->yPosition);

	roamingSpider->eating_bee = 0;
	roamingSpider->eating_bee_complete = 0;
	roamingSpider->going_home = 0;
	roamingSpider->speed = SPIDER_SPEED; /*pixels per millisecond*/
	roamingSpider->stung = 0;
	roamingSpider->ticksSinceEating = 0;
	roamingSpider->ticksSinceStung = 0;
	roamingSpider->ticksSinceDead = 0;
	roamingSpider->deadSpider = 0;

	roamingSpider->countDownToStride = (int)((double)STRIDE_FREQUENCY / roamingSpider->speed);

	roamingSpider->currentGraphicIndex = 0;

}

ResourceNode createResourceNode(GameObjectData *gameObjectData, ResourceNodeSpawner *parentSpawner, int resourceUnits){
	/* ResourceNodeSpawner *parentSpawner = the spawner which this resource node
											is attached to
	 int resourceUnits = the amount of resources to put into this node

	 This creates a resource node attached to a spawner.*/
	ResourceNode resourceNode;
	resourceNode.alive = 1;
	/* The amout of resources (honey, etc...) the node holds */
	resourceNode.resourceUnits = resourceUnits;
	/* We use a randomly generated offset value for now to distribute the
	 ResourceNode around the ResourceNodeSpawner. This can be improved. */
	resourceNode.rect.x = (int)floor(parentSpawner->xPosition + generateRandomCoordOffset(parentSpawner->spawnRadius) - X_SIZE_OF_NODE/2);
	resourceNode.rect.y = (int)floor(parentSpawner->yPosition + generateRandomCoordOffset(parentSpawner->spawnRadius) - Y_SIZE_OF_NODE/2);
	resourceNode.rect.w = SIZE_OF_FLOWER;
	resourceNode.rect.h = SIZE_OF_FLOWER;

	long int r = rand();
	if(r < getDistance2BetweenRects(resourceNode.rect,gameObjectData->hive.rect) * 1.5){
		resourceNode.type = 2;
	}
	else if(r < getDistance2BetweenRects(resourceNode.rect,gameObjectData->hive.rect) * 4.5){
		resourceNode.type = 1;
	}
	else{
		resourceNode.type = 0;
	}

	resourceNode.displayInfo = 0;
	fitRectToWorld(&resourceNode.rect);
	return resourceNode;
}

void updateHiveCell(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, HiveCell *hiveCell, int ticks){
	if(hiveCell->timer >= 0){
		hiveCell->timer -= ticks;
		if(hiveCell->timer <= 0){
			announce(announcementsData,"A new worker was created!");
			createProgrammableWorker(gameObjectData);
			hiveCell->timer = -1;
		}
	}
}

void updateHive(GameObjectData *gameObjectData, AnnouncementsData *announcementsData,  int ticks){
	int i = 0;

	while(i < NUMBER_OF_CELLS_IN_HIVE){
		updateHiveCell(gameObjectData, announcementsData,&gameObjectData->hive.hiveCells[i],ticks);
		i++;
	}
}

void updateGameObjects(GameObjectData *gameObjectData, AudioData *audioData, GraphicsData *graphicsData, AnnouncementsData *announcementsData, int ticks){

	/* GameObjectData *gameObjectData = the pointer to the GameObjectData struct
										that holds all the information about our
										GameObjects.
	 GraphicsData *graphicsData	 = the pointer to the GraphicsData struct
										that holds the information we need to draw
										things on the screen.
	 float ticks					= the number of milliseconds that we need to
										run the updates for this FRAME_RATE

	 This function makes all the GameObjects move around on the screen and show
	 up graphically. It gets called every frame from the main gameLoop function
	 in game.c.*/
	 static int paa = 0;
	int i, j;
	ProgrammableWorker *programmableWorker;
		char GOCause[256];
		char finalScore[256];

	if(!gameObjectData->gameOver){/*start of it not game over*/


	if(gameObjectData->hive.winterCountdown >= WINTER_THRESHOLD && !gameObjectData->pause_status){
  		if(!gameObjectData->pause_status){
  			gameObjectData->hive.winterCountdownFloat-= WINTER_COUNTDOWN_SPEED;
  			gameObjectData->hive.winterCountdown = (int) gameObjectData->hive.winterCountdownFloat;
  		}

  	}



	if(gameObjectData->hive.winterCountdown <= WINTER_THRESHOLD){
		if(gameObjectData->tree->currentGraphicIndex != WINTER_INDEX){
			announce(announcementsData,"Winter has come!");
		}
		gameObjectData->tree->currentGraphicIndex = WINTER_INDEX;
		if(gameObjectData->hive.winterCountdown < WINTER_THRESHOLD){
			SDL_SetRenderTarget(graphicsData->renderer, NULL);

			SDL_SetRenderDrawColor(graphicsData->renderer, 230, 230, 230, 255);
			SDL_RenderFillRect(graphicsData->renderer, NULL);

			/*check if scorebeforewinter has already been recorded, if not then record*/
			if(!gameObjectData->hive.scoreBeforeWinter){
				gameObjectData->hive.scoreBeforeWinter = gameObjectData->hive.flowers_collected;
			}

			if(gameObjectData->hive.flowers_collected > (gameObjectData->hive.scoreBeforeWinter -
			(HONEY_REQUIRED_FOR_WINTER-1 +
			((float)HONEY_REQUIRED_FOR_WINTER *
			((float)gameObjectData->hive.years_survived / (float)REQUIREMENT_YEAR_INCREASE_PERCENTAGE))))){
				/*decrease score over winter (as bees are eating honey), decremented by HONEY_REQUIRED_FOR_WINTER-1*/

				/*but only if there is at least one bee in the hive*/
				if(gameObjectData->hive.bees_taking_shelter > 0 && !gameObjectData->pause_status){
					gameObjectData->hive.flowers_collected -= (rand()%10) ? 0 : 1;
				}


				if(gameObjectData->hive.flowers_collected <= 0 && gameObjectData->hive.bees_taking_shelter){
					gameObjectData->gameOver = 1;
					gameObjectData->gameOverCause = STARVATION;
					gameObjectData->hive.years_survived++;
					sprintf(GOCause,"Your bees ran out of food over winter and died!");
					sprintf(finalScore,"YEARS SURVIVED = %d    SUGAR: %d", gameObjectData->hive.years_survived,
					gameObjectData->hive.flowers_collected);

					setGameOverInfo(&announcementsData->gameOverData, GOCause);
				    setFinalScore(&announcementsData->gameOverData, finalScore);
					killAllBees(&gameObjectData->first_programmable_worker);
					return;
				}
			}else{
				blitTiledBackground(graphicsData, graphicsData->grassTexture);
				gameObjectData->weather.present_weather = Cloud;
				/*AFTER A SHORT DELAY SET TREES BACK TO SUMMER TREES AND OTHER SUMMER STUFF*/
				if(!(gameObjectData->hive.delayBeforeSummer--)){
					gameObjectData->hive.delayBeforeSummer = DELAY_BEFORE_SUMMER;
					gameObjectData->tree->currentGraphicIndex = SUMMER_INDEX;
					gameObjectData->hive.winterCountdownFloat = MAX_DAYS_TO_WINTER;
					gameObjectData->hive.winterCountdown = (int) gameObjectData->hive.winterCountdownFloat;
					gameObjectData->hive.years_survived++;
				}
			}

		}else{
			blitTiledBackground(graphicsData, graphicsData->grassTexture);
		}
	}else if(gameObjectData->hive.winterCountdown < AUTUMN_THRESHOLD){
		if(gameObjectData->tree->currentGraphicIndex != AUTUMN_INDEX){
			announce(announcementsData,"Autumn has come!");
		}
		gameObjectData->hive.scoreBeforeWinter = 0;
		gameObjectData->tree->currentGraphicIndex = AUTUMN_INDEX;
		blitTiledBackground(graphicsData, graphicsData->grassTexture);
	}else{
		gameObjectData->hive.scoreBeforeWinter = 0;
		gameObjectData->tree->currentGraphicIndex = SUMMER_INDEX;
		blitTiledBackground(graphicsData, graphicsData->grassTexture);
	}

	}else{/*end of it not game over*/
			SDL_SetRenderDrawColor(graphicsData->renderer, 230, 230, 230, 255);
			SDL_RenderFillRect(graphicsData->renderer, NULL);
	}



	i = 0;
	/* Second, we loop through all the ResourceNodeSpawners */
	while(i < gameObjectData->resourceNodeSpawnerCount){
	j = 0;

	if(!gameObjectData->pause_status){
		 /* Update each one to reflect respawning nodes and so on. */
		 updateResourceNodeSpawner(gameObjectData,&gameObjectData->resourceNodeSpawners[i],ticks);
	}
	/* Then we need to loop through the attached ResourceNodes and draw them */
	 while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
		if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){

			if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].displayInfo){
				SDL_Point point = getCenterOfRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect);
				renderFillRadius(graphicsData, &point, SIZE_OF_FLOWER, 255,255,255, 80);
			}
		blitGameObject(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect,
						 graphicsData,
						 graphicsData->nodeTexture[gameObjectData->resourceNodeSpawners[i].resourceNodes[j].type],
						 0,
						 NULL,
						 SDL_FLIP_NONE);
		}
		j++;
	}
	i++;
	}



	if(gameObjectData->hive.displayInfo){
		SDL_Point point = getCenterOfRect(gameObjectData->hive.rect);
		renderFillRadius(graphicsData, &point, 40, 255,255,255, 80);
	}


	/* First, we need to draw the Hive in at the correct position. */
	blitGameObject(gameObjectData->hive.rect,
				 graphicsData,
				 graphicsData->hiveTexture,
				 0,
				 NULL,
				 SDL_FLIP_NONE);

	ProgrammableWorker *next = NULL;
	if(!gameObjectData->pause_status){
		for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL; programmableWorker = next){
			next = programmableWorker->next;
			updateProgrammableWorker(&programmableWorker,gameObjectData,announcementsData,ticks);
		}
	}

	if(!gameObjectData->gameOver){ /*start of if not game over*/
	next = NULL;
	/*BEES ON GROUND NEED RENDERING BEFORE PERSON*/
	for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL ; programmableWorker = next){
		ProgrammableWorker *progWork;
		SDL_Rect smallerBeeRect;
		next = programmableWorker->next;
		smallerBeeRect.w = (int)((float)programmableWorker->rect.w/BEE_SHRINK_FACTOR_ON_GROUND);
		smallerBeeRect.h = (int)((float)programmableWorker->rect.h/BEE_SHRINK_FACTOR_ON_GROUND);
		smallerBeeRect.y = programmableWorker->rect.y;
		smallerBeeRect.x = programmableWorker->rect.x;

		if(programmableWorker->wet_and_cant_fly == 1 || programmableWorker->cold_and_about_to_die == 1){
			if(programmableWorker->displayInfo){
				SDL_Point point = getCenterOfRect(programmableWorker->rect);
				point.x -= 5;
				point.y -= 5;
				renderRadius(graphicsData, &point, WORKER_SENSE_RANGE, 255,255,255, 255);
			}

			blitGameObject(smallerBeeRect,
						graphicsData,
						graphicsData->bee->graphic[programmableWorker->currentGraphicIndex > 5 ? 0 : programmableWorker->currentGraphicIndex],
						DEGREESINCIRCLE-(programmableWorker->heading * RADIANSTODEGREES),
						NULL,
						SDL_FLIP_NONE);
		}



		if(programmableWorker->cold_and_about_to_die > COLD_DEATH_THRESHOLD){
			if(!(programmableWorker == gameObjectData->first_programmable_worker && programmableWorker->next == NULL)){
				ProgrammableWorker *worker;

				int i = 0;

				killProgrammableWorker(gameObjectData, &programmableWorker);
			}else{
				gameObjectData->gameOver = 1;
				gameObjectData->gameOverCause = COLD;
				gameObjectData->hive.years_survived++;
				sprintf(GOCause,"None of your bees took shelter in the hive over winter!");
				sprintf(finalScore,"YEARS SURVIVED = %d    SUGAR: %d", gameObjectData->hive.years_survived,
				gameObjectData->hive.flowers_collected);

				setGameOverInfo(&announcementsData->gameOverData, GOCause);
				setFinalScore(&announcementsData->gameOverData, finalScore);
				free(gameObjectData->first_programmable_worker);
				gameObjectData->first_programmable_worker = NULL;
				return;
			}
		}
	}

	/*if icecream is on the ground*/
	if(gameObjectData->droppedIceCream->dropped){
		/*but not yet melted*/
		if(gameObjectData->droppedIceCream->droppedTimer < MELT_TIME_THRESHOLD){

			if(!gameObjectData->pause_status){
				gameObjectData->droppedIceCream->droppedTimer++;
	 		}

			if(gameObjectData->droppedIceCream->rect.w >= MAX_DROPPED_ICECREAM_WIDTH){
				gameObjectData->droppedIceCream->sizeOscillator = -1;
			}else if(gameObjectData->droppedIceCream->rect.w <= DROPPED_ICECREAM_WIDTH){
				gameObjectData->droppedIceCream->sizeOscillator = 1;
			}
			gameObjectData->droppedIceCream->rect.w += gameObjectData->droppedIceCream->sizeOscillator;
			//gameObjectData->droppedIceCream->xPosition -= ((float)gameObjectData->droppedIceCream->sizeOscillator)/2.0;
			gameObjectData->droppedIceCream->rect.x = gameObjectData->droppedIceCream->xPosition;

			gameObjectData->droppedIceCream->rect.h += gameObjectData->droppedIceCream->sizeOscillator;
			//gameObjectData->droppedIceCream->yPosition -= ((float)gameObjectData->droppedIceCream->sizeOscillator)/2.0;
			gameObjectData->droppedIceCream->rect.y = gameObjectData->droppedIceCream->yPosition;

			if(gameObjectData->droppedIceCream->displayInfo){
				SDL_Point point = getCenterOfRect(gameObjectData->droppedIceCream->rect);
				renderFillRadius(graphicsData, &point, gameObjectData->droppedIceCream->rect.w/2, 255,255,255, 80);
			}

  	 		blitGameObject(gameObjectData->droppedIceCream->rect,
            	      	graphicsData,
                	   	graphicsData->droppedIceCreamTexture,
                	   	0,
                	   	NULL,
                	   	SDL_FLIP_NONE);
        }else{
        	gameObjectData->droppedIceCream->droppedTimer++;
        	if(gameObjectData->droppedIceCream->droppedTimer < MELT_TIME_THRESHOLD + 40){
        		/*allow a bit of time to display the melted icecream (hence +40)*/
        		if(gameObjectData->droppedIceCream->displayInfo){
					SDL_Point point = getCenterOfRect(gameObjectData->droppedIceCream->rect);
					renderFillRadius(graphicsData, &point, gameObjectData->droppedIceCream->rect.w/2, 255,255,255, 80);
				}
        		blitGameObject(gameObjectData->droppedIceCream->rect,
            	      	graphicsData,
                	   	graphicsData->meltedIceCreamTexture,
                	   	0,
                	   	NULL,
                	   	SDL_FLIP_NONE);
        	}else{/*now get rid of icecream*/
        		gameObjectData->droppedIceCream->dropped = 0;
        		gameObjectData->droppedIceCream->droppedTimer = 0;
        	}
        }
	}

	 /*determine if iceCreamPerson is on screen and needs animating*/
	if(gameObjectData->iceCreamPerson->currently_on_screen){

	 	if(!gameObjectData->pause_status){
		 	updateIceCreamPerson(gameObjectData, ticks);
	 	}

	 	if(gameObjectData->iceCreamPerson->displayInfo){
			SDL_Point point = getCenterOfRect(gameObjectData->iceCreamPerson->rect);
			renderFillRadius(graphicsData, &point, PERSON_WIDTH/2, 255,255,255, 80);
		}

  	 	blitGameObject(gameObjectData->iceCreamPerson->rect,
                  	graphicsData,
                   	graphicsData->person->graphic[gameObjectData->iceCreamPerson->currentGraphicIndex +
                   	((gameObjectData->iceCreamPerson->has_ice_cream) ? 0 : NO_ICECREAM_INDEX_OFFSET)],
                   	DEGREESINCIRCLE-(gameObjectData->iceCreamPerson->heading * RADIANSTODEGREES),
                   	NULL,
                   	SDL_FLIP_NONE);

	}else{ /*small probability of re-initialising iceCreamPerson and setting location to on-screen*/
	 	if((gameObjectData->weather.present_weather == Sun) && gameObjectData->hive.winterCountdown >= AUTUMN_THRESHOLD &&
 		(rand() % ICE_CREAM_PERSON_PROB == 0) && !gameObjectData->droppedIceCream->dropped){
			announce(announcementsData,"The ice cream man appeared!");
	 		reInitialiseIceCreamPerson(gameObjectData->iceCreamPerson);

		}
 	}

	/*determine if roamingSpider is on screen and needs animating*/
	if(gameObjectData->roamingSpider->currently_on_screen){

	 	if(!gameObjectData->pause_status){
		 	updateRoamingSpider(gameObjectData, ticks);
	 	}

	 	if(gameObjectData->roamingSpider->displayInfo){
			SDL_Point point = getCenterOfRect(gameObjectData->roamingSpider->rect);
			renderFillRadius(graphicsData, &point, PERSON_WIDTH/2, 255,255,255, 80);
		}

  	 	/*if (gameObjectData->roamingSpider->eating_bee == 1) {
			SDL_Rect largerSpiderRect;
			
			largerSpiderRect.w = (int)((float)gameObjectData->roamingSpider->rect.w*1.4);
			largerSpiderRect.h = (int)((float)gameObjectData->roamingSpider->rect.h*1.4);
			largerSpiderRect.y = gameObjectData->roamingSpider->rect.y;
			largerSpiderRect.x = gameObjectData->roamingSpider->rect.x;
			
			blitGameObject(largerSpiderRect,
                  	graphicsData,
                   	graphicsData->roamingArachnid->graphic[gameObjectData->roamingSpider->currentGraphicIndex],
                   	DEGREESINCIRCLE-(gameObjectData->roamingSpider->heading * RADIANSTODEGREES),
                   	NULL,
                   	SDL_FLIP_NONE);
		}else{*/
			blitGameObject(gameObjectData->roamingSpider->rect,
                  	graphicsData,
                   	graphicsData->roamingArachnid->graphic[gameObjectData->roamingSpider->currentGraphicIndex],
                   	DEGREESINCIRCLE-(gameObjectData->roamingSpider->heading * RADIANSTODEGREES),
                   	NULL,
                   	SDL_FLIP_NONE);
		/*}*/

	}else{ /*small probability of re-initialising iceCreamPerson and setting location to on-screen*/
	 	if((rand() % ICE_CREAM_PERSON_PROB == 0)){

			announce(announcementsData,"A spider appeared!");
	 		reInitialiseRoamingSpider(gameObjectData->roamingSpider);

		}
 	}


	/* Thirdly, we loop through all the ProgrammableWorkers and update them */
	/* AI thinking has been moved to a seperate function to prevent some circular
	 inheritance issues. */
	/* Also, remember the important rule - AI should tell workers to do things rather
	 than directly doing things itself! */
			gameObjectData->hive.bees_taking_shelter = 0;
	ProgrammableWorker *nextt = NULL;
	int dead = 0;
	for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL ; programmableWorker = nextt){
		nextt = programmableWorker->next;
		if(!(programmableWorker->wet_and_cant_fly || programmableWorker->cold_and_about_to_die)){
			if(programmableWorker->displayInfo && graphicsData->trackingMode){
				int window_x,window_y, tempXOffset, tempYOffset;
				SDL_Point point = getCenterOfRect(programmableWorker->rect);

				renderRadius(graphicsData, &point, WORKER_SENSE_RANGE, 255,255,255, 180);
					SDL_GetWindowSize(graphicsData->window,&window_x,&window_y);
				/* I'm internally debating whether to center the screen or not here */
				tempXOffset = -programmableWorker->rawX + (programmableWorker->xRenderPosWhenSelected);
				tempYOffset = -programmableWorker->rawY  + (programmableWorker->yRenderPosWhenSelected);

				if(tempXOffset > -X_SIZE_OF_WORLD + window_x && tempXOffset < 0){
					graphicsData->navigationOffset.x = tempXOffset;
				}

				if(tempYOffset > -Y_SIZE_OF_WORLD + window_y && tempYOffset < 0){
					graphicsData->navigationOffset.y = tempYOffset;
				}
			}


			/* This needs to have some changes to adjust the rawX and rawY values too */
			//fitRectToWorld(&programmableWorker->rect);
			/*render if now currently just above (in) hive*/

			if(!isPointInRangeOf(getCenterOfRect(programmableWorker->rect),
			getCenterOfRect(gameObjectData->hive.rect), HIVE_SHELTER_RADIUS)){

				blitGameObject(programmableWorker->rect,
						graphicsData,
						graphicsData->bee->graphic[programmableWorker->currentGraphicIndex +
						((programmableWorker->cargo < SUGAR_VALUE_OF_ICECREAM && programmableWorker->cargo > 0) ? CARRYING_FLOWER_INDEX_OFFSET :
						((programmableWorker->cargo == SUGAR_VALUE_OF_ICECREAM) ? CARRYING_ICECREAM_INDEX_OFFSET : 0))],
						DEGREESINCIRCLE-(programmableWorker->heading * RADIANSTODEGREES),
						NULL,
						SDL_FLIP_NONE);
			}else{
				gameObjectData->hive.bees_taking_shelter++;
			}
			i++;
		}
	}


	}/*end of if not game over*/


	/* render tree tops last, so that they appear above everything else*/
	for(i = 0; i < NUMBER_OF_TREES; i++){

		if(gameObjectData->tree[i].displayInfo){
			SDL_Point point = getCenterOfRect(gameObjectData->tree[i].rect);
			renderFillRadius(graphicsData, &point, TREE_SHELTER_RADIUS, 0,0,0, 40);
		}

		blitGameObject(gameObjectData->tree[i].stumpRect,
					 	graphicsData,
					 	graphicsData->treeStumpTexture,
					 	0,
					 	NULL,
					 	SDL_FLIP_NONE);

	 	blitParallaxTreeTops(gameObjectData->tree[i].rect,
						graphicsData,
						graphicsData->shelter->graphic[gameObjectData->tree->currentGraphicIndex]);
	}

	/*finally render a layer or rain splatter if its raining*/
	if(gameObjectData->weather.present_weather == Rain){
		blitRainRandomly(graphicsData);
	}

	if(!gameObjectData->pause_status){
		updateHive(gameObjectData,announcementsData,ticks);
	}

	updateWeather(gameObjectData, audioData, &gameObjectData->weather, ticks);

	paintWeatherLayer(graphicsData, gameObjectData->weather.present_weather);


}

void objectInfoDisplay(GameObjectData *gameObjectData, GraphicsData *graphicsData, AnnouncementsData *announcementsData, SDL_MouseButtonEvent *mbEvent)
{
	static int objectDisplayShowing = 0;
    SDL_Event objectInfoDisplayEvent;
	ProgrammableWorker *worker;
	SDL_Point point;
	int displayInfoAlreadySet = 0, i;
	point.x = mbEvent->x - graphicsData->navigationOffset.x;
	point.y = mbEvent->y - graphicsData->navigationOffset.y;
	char displayString[255];

	graphicsData->trackingMode = 0;

	for(i = 0; i < NUMBER_OF_TREES; i++){
		if(isPointInRangeOf(getCenterOfRect(gameObjectData->tree[i].rect), point, SIZE_OF_TREE/2)){
			if(!displayInfoAlreadySet){
				gameObjectData->tree[i].displayInfo = 1;
				displayInfoAlreadySet = 1;
				sprintf(displayString," TREE ");
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, NAME);
				sprintf(displayString," Status: ");
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, STATUS);
			}else{
				gameObjectData->tree[i].displayInfo = 0;
			}
		}else{
			gameObjectData->tree[i].displayInfo = 0;
		}
	}

	for(worker = gameObjectData->first_programmable_worker; worker != NULL ;
		worker = worker->next){
		if(isPointInRangeOf(getCenterOfRect(worker->rect), point, worker->rect.w/2)){
			if(!displayInfoAlreadySet){
				worker->displayInfo = 1;
				worker->xRenderPosWhenSelected = worker->rect.x + graphicsData->navigationOffset.x;
				worker->yRenderPosWhenSelected = worker->rect.y + graphicsData->navigationOffset.y;
				displayInfoAlreadySet = 1;
				sprintf(displayString," BEE ");
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, NAME);
				sprintf(displayString," Status: %s", worker->beeStatus);
				setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, STATUS);
				graphicsData->trackingMode = 1;
			}else{
				worker->displayInfo = 0;
			}
		}else{
		    worker->displayInfo = 0;
		}
	}

	if(isPointInRangeOf(getCenterOfRect(gameObjectData->iceCreamPerson->rect), point, PERSON_WIDTH/2)){
		if(!displayInfoAlreadySet){
			gameObjectData->iceCreamPerson->displayInfo = 1;
			displayInfoAlreadySet = 1;
			sprintf(displayString," GARRY ");
			setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, NAME);
			sprintf(displayString," Status: ");
			setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, STATUS);
		}else{
			gameObjectData->iceCreamPerson->displayInfo = 0;
		}
	}else{
		gameObjectData->iceCreamPerson->displayInfo = 0;
	}

	if(isPointInRangeOf(getCenterOfRect(gameObjectData->droppedIceCream->rect), point, DROPPED_ICECREAM_WIDTH/2)){
		if(!displayInfoAlreadySet){
			gameObjectData->droppedIceCream->displayInfo = 1;
			displayInfoAlreadySet = 1;
			sprintf(displayString," ICECREAM ");
			setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, NAME);
			sprintf(displayString," Status: ");
			setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, STATUS);
		}else{
			gameObjectData->droppedIceCream->displayInfo = 0;
		}
	}else{
		gameObjectData->droppedIceCream->displayInfo = 0;
	}

	i = 0;
	while(i < gameObjectData->resourceNodeSpawnerCount){
		int j = 0;

	 	while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
			if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive &&
			isPointInRangeOf(getCenterOfRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect), point, SIZE_OF_FLOWER/2)){
				if(!displayInfoAlreadySet){
					gameObjectData->resourceNodeSpawners[i].resourceNodes[j].displayInfo = 1;
					displayInfoAlreadySet = 1;
					sprintf(displayString," FLOWER ");
			    	setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, NAME);
					sprintf(displayString," Status: ");
					setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, STATUS);

				}else{
					gameObjectData->resourceNodeSpawners[i].resourceNodes[j].displayInfo = 0;
				}
			}else{
				gameObjectData->resourceNodeSpawners[i].resourceNodes[j].displayInfo = 0;
			}
			j++;
		}
		i++;
	}

	if(isPointInRangeOf(getCenterOfRect(gameObjectData->hive.rect), point, 40)){

		if(!displayInfoAlreadySet){
			gameObjectData->hive.displayInfo = 1;
			displayInfoAlreadySet = 1;
			sprintf(displayString," HIVE ");
			setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, NAME);
			sprintf(displayString," Status: ");
			setObjectInfoDisplay(&announcementsData->objectInfoDisplay, displayString, STATUS);
		}else{
			gameObjectData->hive.displayInfo = 0;
		}
	}else{
		gameObjectData->hive.displayInfo = 0;
	}

	if(displayInfoAlreadySet){
		if(!objectDisplayShowing){
			objectDisplayShowing = 1;
			objectInfoDisplayEvent.type = gameObjectData->objectDisplayEventNum;
			SDL_PushEvent(&objectInfoDisplayEvent);
		}
	}else{
		if(objectDisplayShowing){
			objectDisplayShowing = 0;
			objectInfoDisplayEvent.type = gameObjectData->objectDisplayEventNum;
			SDL_PushEvent(&objectInfoDisplayEvent);
		}
	}
}

static void nullifyProgrammableWorkerBrain(ProgrammableWorkerBrain *brain){
	brain->aiStartPoint = 0;
}

void nullifyLocalAIInformation(GameObjectData *gameObjectData){
	ProgrammableWorker *p = gameObjectData->first_programmable_worker;

	while(p != NULL){
		nullifyProgrammableWorkerBrain(&p->brain);
		p = p->next;
	}
}
