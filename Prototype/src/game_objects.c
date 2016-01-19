#include "game_objects.h"

static void nullifyProgrammableWorkerBrain(ProgrammableWorkerBrain *brain);
/* Subfunction for initGameObjectDatas */
static GameObjectSelection createGameObjectSelection(void);
static Weather createWeather(void);
static Environment createEnvironment(void);
/* Subfunction for countResourceNodesInRadius */
static int valueNode(ResourceNode *resourceNode);
/* Subfunction for createProgrammableWorker */
static ProgrammableWorkerBrain createProgrammableWorkerBrain(void);
/* Subfunction for createResourceNodeSpawner */
static void initResourceNode(ResourceNode *resourceNode);
static ResourceNode createResourceNode(GameObjectData *gameObjectData, ResourceNodeSpawner *parentSpawner, int resourceUnits);
static void chooseResourceNodeType(GameObjectData *gameObjectData, ResourceNode *resourceNode);
/* Subfunctions for updateGameObjects */
static void updateWeather(GameObjectData *gameObjectData, AudioData *audioData, Weather *weather, int ticks);
static void reInitialiseRoamingSpider(RoamingSpider *roamingSpider);
static void updateEnvironment(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks);
static void updateResourceNodes(GameObjectData *gameObjectData, int ticks);
static void updateResourceNodeSpawner(GameObjectData *gameObjectData, ResourceNodeSpawner *spawner, int ticks);
static int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner);
static void updateRoamingSpider(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks);
static void reInitialiseRoamingSpider(RoamingSpider *roamingSpider);
static void updateIceCreamPerson(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks);
static void reInitialiseIceCreamPerson(IceCreamPerson *iceCreamPerson);
static void updateIceCream(GameObjectData *gameObjectData, int ticks);
static void updateHive(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks);
static void updateHiveCell(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, HiveCell *hiveCell, int ticks);
static void killAllBees(ProgrammableWorker **programmableWorker);
static void updateProgrammableWorkers(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, AudioData *audioData, int ticks);
static int updateProgrammableWorker(ProgrammableWorker **programmableWorkerP, GameObjectData *gameObjectData, AnnouncementsData *announcementsData, AudioData *audioData, int ticks);
static void updateProgrammableWorkerWeather(GameObjectData *gameObjectData, ProgrammableWorker *programmableWorker, int ticks);
static void updateProgrammableWorkerMove(ProgrammableWorker *programmableWorker, int ticks);
static ResourceNode *checkResourceNodeCollision(ResourceNodeSpawner **resourceNodeSpawnerPointer,
																				 				GameObjectData *gameObjectData,
																				 				ProgrammableWorker *programmableWorker,
																			   				double sense_range);
static void updateProgrammableWorkerIdle(GameObjectData *gameObjectData, ProgrammableWorker *programmableWorker);
static int updateProgrammableWorkerCombat(GameObjectData *gameObjectData, ProgrammableWorker *programmableWorker, AnnouncementsData *announcementsData);
static void killProgrammableWorker(GameObjectData *gameObjectData, ProgrammableWorker **toDeletePointer);
static void updateProgrammableWorkerSenses(ProgrammableWorker *programmableWorker, ResourceNodeSpawner *resourceNodeSpawner);
static ResourceNode *chooseNodeRandomly(ResourceNodeSpawner *resourceNodeSpawner);
/* Subfunctions for renderGameObjects */
static void blitBackground(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitTrees(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitNodes(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitHive(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitIceCreamPerson(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitIceCream(GameObjectData *gameObjectData,GraphicsData *graphicsData);
static void blitRoamingSpider(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitDisabledWorkers(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitProgrammableWorkers(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitWeather(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void blitSelection(GameObjectData *gameObjectData, GraphicsData *graphicsData);
static void getWorkerInfoString(ProgrammableWorker *programmableWorker, char *targetString);
static void getResourceNodeInfoString(ResourceNode *resourceNode, char *targetString);
static void getSpiderInfoString(RoamingSpider *roamingSpider, char *targetString);
static void getResourceNodeInfoString(ResourceNode *resourceNode, char *targetString);
static void getWorkerInfoString(ProgrammableWorker *programmableWorker, char *targetString);
/* Subfunctions for selectGameObject */
static ProgrammableWorker *selectWorker(GameObjectData *gameObjectData, SDL_Point point);
static Hive *selectHive(GameObjectData *gameObjectData, SDL_Point point);
static RoamingSpider *selectSpider(GameObjectData *gameObjectData, SDL_Point point);
static IceCreamPerson *selectIceCreamPerson(GameObjectData *gameObjectData, SDL_Point point);
static DroppedIceCream *selectDroppedIceCream(GameObjectData *gameObjectData, SDL_Point point);
static ResourceNode *selectResourceNode(GameObjectData *gameObjectData, SDL_Point point);
static Tree *selectTree(GameObjectData *gameObjectData, SDL_Point point);
/* Subfunction for nullifyLocalAIInformation */
static void nullifyProgrammableWorkerBrain(ProgrammableWorkerBrain *brain);

/* --------------------------------------------------------------------------- */

/*	 createGameObjectData

	 This function sets up all the initial values for a GameObjectData object and
	 returns it */

GameObjectData createGameObjectData(void){
	GameObjectData gameObjectData;
	gameObjectData.gameObjectSelection = createGameObjectSelection();
	gameObjectData.environment = createEnvironment();
	gameObjectData.years_survived = 0;
  gameObjectData.pause_status = 0;
  gameObjectData.first_programmable_worker = NULL;
  gameObjectData.gameOver = 0;
  gameObjectData.gameOverEventNum = SDL_RegisterEvents(1);
  gameObjectData.objectDisplayEventNum = SDL_RegisterEvents(1);
	return gameObjectData;
}
/* createGameObjectSelection

	This static function sets up the GameObjectSelection struct properly */

static GameObjectSelection createGameObjectSelection(void){
	GameObjectSelection gameObjectSelection;
	gameObjectSelection.enabled = 1;
	gameObjectSelection.visible = 0;
	gameObjectSelection.type = GOS_NONE;
	gameObjectSelection.selected = NULL;
	memset(gameObjectSelection.nameString,0,sizeof(gameObjectSelection.nameString));
	memset(gameObjectSelection.infoString,0,sizeof(gameObjectSelection.infoString));
	return gameObjectSelection;
}

static Environment createEnvironment(void){
	Environment environment;
	environment.delayBeforeSummer = DELAY_BEFORE_SUMMER;
	environment.winterCountdown = MAX_DAYS_TO_WINTER;
	environment.winterCountdownFloat = (double) environment.winterCountdown;
	environment.season = SUMMER;
	environment.years_survived = 0;
	environment.treeGraphic = SUMMER_INDEX;
	environment.weather = createWeather();
	return environment;
}

/* createWeatherLayer

	 This function creates a Weather struct and fills in the default
	 values. Many of these are defined in generic.h */

static Weather createWeather(void){
	Weather weather;
	weather.tickCount = 0;
	weather.present_weather = Sun;
	return weather;
}

/* -------------------------------------------------------------------------- */

/* getNearestWorker

	 This runs through our linked list and gets the worker that is specifically
	 the furthest from a given one. Passing a non-NULL argument as ignore will
	 allow you to prevent choosing a specific worker and therefore select from a
	 given worker's location without getting the obvious (but useless result) */

ProgrammableWorker *getNearestWorker(GameObjectData *gameObjectData, int x,int y,
																		 ProgrammableWorker *ignore){
	SDL_Point point;
	double d2 = 0.0;
	double d22 = 0.0;
	ProgrammableWorker *pw = NULL;
	ProgrammableWorker *p;
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

/* -------------------------------------------------------------------------- */

/* getNearestTree

   This searchs for the closest Tree to a given point. It's used by the AI block
	 GoToTree. */

Tree *getNearestTree(GameObjectData *gameObjectData, int x, int y){
	int i = 0;
	SDL_Point p;
	double d2 = 0.0;
	double d22 = 0.0;
	Tree *tree = NULL;
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

/* -------------------------------------------------------------------------- */

/* countResourceNodesInRadius

	 This counts the number of ResourceNodes in a radius around a point. It's used
	 by the AI functions.

	 To reduce the computational overhead of having potential several thousand
	 resourceNodes, we use a hierarchical data structure, where we have
	 ResourceNodeSpawners each holding an array of ResourceNodes. This means we
	 can iterate through the ResourceNodeSpawners (of which there may be less than
   100) and only if we are close enough to these do we need to check the
	 ResourceNodes attached to them */

int countResourceNodesInRadius(GameObjectData *gameObjectData, int x, int y,
															 double radius){
	int count = 0;
	int i = 0, j;
	SDL_Point p;
	while(i < gameObjectData->resourceNodeSpawnerCount){
		if(getDistance2BetweenPoints(x,y,(int)gameObjectData->resourceNodeSpawners[i].xPosition,
		   (int)gameObjectData->resourceNodeSpawners[i].yPosition) < radius * radius){
					j = 0;
					while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
						p = getCenterOfRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect);
						if(getDistance2BetweenPoints(x,y,p.x,p.y) < radius * radius){
							count += valueNode(&gameObjectData->resourceNodeSpawners[i].resourceNodes[j]);
						}
						j++;
					}
		}
		i++;
	}
	return count;
}

/* This static function just helps us to value different nodes.

	 I'm debating whether to have a separate function for counting the number
	 and counting the value of ResourceNodes in a radius, in which case one will
	 just not have this function included*/

static int valueNode(ResourceNode *resourceNode){
	switch(resourceNode->type){
		case 0:
			return 1;
		case 1:
			return 2;
		case 2:
			return 5;
		default:
			return 0;
	}
}

/* -------------------------------------------------------------------------- */

/* countProgrammableWorkersInRange

	 This simply counts the ProgrammableWorkers in a given radius around a point.

	 I'm debating whether to use consistently SDL_Points or x/y ints (leaning
	 towards the latter)*/

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

/* -------------------------------------------------------------------------- */

/* createProgrammableWorker

	 This just mallocs up space for a ProgrammableWorker and links into into the
	 linked list of workers. It sets up realistic initial values as well.

	 Most of these values are defined in generic.h*/

ProgrammableWorker *createProgrammableWorker(GameObjectData *gameObjectData){
	ProgrammableWorker *programmableWorker = malloc(sizeof(ProgrammableWorker));
	ProgrammableWorker *p = gameObjectData->first_programmable_worker;

	if(p == NULL){
		gameObjectData->first_programmable_worker = programmableWorker;
	}
	else{
		for(p = gameObjectData->first_programmable_worker; p->next != NULL ; p = p -> next){}
			p -> next = programmableWorker;
	}

	programmableWorker->rawX = X_SIZE_OF_WORLD/2;
	programmableWorker->rawY = Y_SIZE_OF_WORLD/2;
	programmableWorker->rect.x = 100;
	programmableWorker->rect.y = 100;
	programmableWorker->rect.w = X_SIZE_OF_WORKER;
	programmableWorker->rect.h = Y_SIZE_OF_WORKER;
	programmableWorker->beeStatus = NULL;
	programmableWorker->wet_and_cant_fly = 0;
	programmableWorker->cold_and_about_to_die = 0;
	programmableWorker->flapTimer = rand() % 100;
	programmableWorker->stunned_after_sting = 0;
	programmableWorker->insideHive = 0;
	programmableWorker->fighting_spider = 0;
	programmableWorker->currentGraphicIndex = BEE_FLAP_GRAPHIC_1;
	programmableWorker->heading = 0.0;
	programmableWorker->speed = WORKER_SPEED;
	programmableWorker->type = 1;
	programmableWorker->cargo = 0;
	programmableWorker->displayInfo = 0;
	programmableWorker->status = IDLE;
	programmableWorker->next = NULL;
	programmableWorker->brain = createProgrammableWorkerBrain();
	programmableWorker->weatherproofTimer = BASE_WEATHERPROOF + rand() % RANDOM_WEATHERPROOF;
	return(programmableWorker);
}

/* This just creates the initial values for a ProgrammableWorkerBrain */

static ProgrammableWorkerBrain createProgrammableWorkerBrain(void){
	ProgrammableWorkerBrain brain;
	brain.is_point_remembered = 0;
	brain.followTarget = NULL;
	brain.foundNode = NULL;
	brain.aiStartPoint = 0;
	brain.waitTime = -1;
	brain.nearestWorker = NULL;
	brain.nearestWorkerCacheTime = 0;
	return brain;
}

/* -------------------------------------------------------------------------- */

/* createIceCreamPerson

	 This creates an iceCreamPerson and fills it with sensible values. As I
	 understand it, this is only called once */

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

/* -------------------------------------------------------------------------- */

/* createHive

   This just initialises the data for our hive and places in in the middle of then
	 world. */

Hive createHive(void){
	/* This function creates a Hive struct and fills in the default
	 values. Many of these are defined in generic.h */
	Hive hive;
	int i = 0;
	hive.rect.w = X_SIZE_OF_HIVE;
	hive.rect.h = Y_SIZE_OF_HIVE;
	hive.rect.x = (X_SIZE_OF_WORLD/2 - hive.rect.w/2);
	hive.rect.y = (Y_SIZE_OF_WORLD/2 - hive.rect.h/2);
	hive.flowers_collected = 0;
	while(i < NUMBER_OF_CELLS_IN_HIVE){
		hive.hiveCells[i].timer = -1;
		i++;
	}
	return(hive);
}

/* -------------------------------------------------------------------------- */

/* createTree

   This just creates a Tree. The actual randomness to their positioning is determined
	 in world_generation.c */

Tree createTree(GameObjectData *gameObjectData, int forceX, int forceY){
	Tree tree;

	tree.rect.w = SIZE_OF_TREE;
	tree.rect.h = SIZE_OF_TREE;
	tree.stumpRect.w = SIZE_OF_TREESTUMP;
	tree.stumpRect.h = SIZE_OF_TREESTUMP;
	tree.rect.x = forceX;
	tree.rect.y = forceY;
	/*if tree is too close to hive, rellocate*/
	while(isPointInRangeOf(getCenterOfRect(tree.rect), getCenterOfRect(gameObjectData->hive.rect), 100)){

		if(rand()%2){
			tree.rect.x += SIZE_OF_TREE;
		}
		else{
			tree.rect.x -= SIZE_OF_TREE;
		}

		if(rand()%2){
			tree.rect.y += SIZE_OF_TREE;
		}
		else{
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

/* -------------------------------------------------------------------------- */

/* createResourceNodeSpawner

   This creates a ResourceNodeSpawner at a given location with a given radius
	 and initialises all the relevant bits of information */

ResourceNodeSpawner createResourceNodeSpawner(GameObjectData *gameObjectData,int maximumNodeCount, double xPosition, double yPosition, double radius){
	ResourceNodeSpawner resourceNodeSpawner;
	int i = 0;

	resourceNodeSpawner.maximumNodeCount = maximumNodeCount + rand() % maximumNodeCount;
	resourceNodeSpawner.currentNodeCount = 0;

	resourceNodeSpawner.xPosition = xPosition;
	resourceNodeSpawner.yPosition = yPosition;

	resourceNodeSpawner.spawnDelay = DEFAULT_SPAWNDELAY;
	resourceNodeSpawner.ticksSinceSpawn = rand() % resourceNodeSpawner.spawnDelay;
	resourceNodeSpawner.spawnRadius = radius + rand() % RANDOMISE_SPAWNRADIUS;
	resourceNodeSpawner.collisionRect.x = (int)floor(xPosition - radius/2);
	resourceNodeSpawner.collisionRect.y = (int)floor(yPosition - radius/2);
	resourceNodeSpawner.collisionRect.w = (int)floor(radius);
	resourceNodeSpawner.collisionRect.h = (int)floor(radius);

	resourceNodeSpawner.resourceNodes = calloc((size_t)resourceNodeSpawner.maximumNodeCount, sizeof(ResourceNode));
	while(i<resourceNodeSpawner.maximumNodeCount){
		initResourceNode(&resourceNodeSpawner.resourceNodes[i]);
		if(rand() % 2 != 0){
			resourceNodeSpawner.resourceNodes[i] = createResourceNode(gameObjectData,&resourceNodeSpawner,0);
	 }
	 i++;
	}
	return(resourceNodeSpawner);
}

/* This sets up the data for ResourceNodes, so that there's no weird edge cases
   where uninitialised data makes us all confused */

static void initResourceNode(ResourceNode *resourceNode){
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

static ResourceNode createResourceNode(GameObjectData *gameObjectData, ResourceNodeSpawner *parentSpawner, int resourceUnits){
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

	double angle = randPi() * 2;
	double r = (double)parentSpawner->spawnRadius * (double)rand()/(double)(RAND_MAX);
	if(r < 1){
		r = 2 - r;
	}

	resourceNode.rect.x = parentSpawner->xPosition + sin(angle) * r - X_SIZE_OF_NODE/2;
	resourceNode.rect.y = parentSpawner->yPosition + cos(angle) * r - Y_SIZE_OF_NODE/2;
	resourceNode.rect.w = SIZE_OF_FLOWER;
	resourceNode.rect.h = SIZE_OF_FLOWER;
	resourceNode.spawner = parentSpawner;

	chooseResourceNodeType(gameObjectData, &resourceNode);

	parentSpawner->currentNodeCount++;

	resourceNode.displayInfo = 0;
	fitRectToWorld(&resourceNode.rect);
	return resourceNode;
}

static void chooseResourceNodeType(GameObjectData *gameObjectData, ResourceNode *resourceNode){
	int r = 5 - (rand() % 11);
	SDL_Point p = getCenterOfRect(resourceNode->rect);
	SDL_Point p1 = getCenterOfRect(gameObjectData->hive.rect);
	double d = 0.05 * r + getDistance2BetweenPoints(p.x,p.y,p1.x,p1.y) / (X_SIZE_OF_WORLD*X_SIZE_OF_WORLD);
	if(d > 0.5 * 0.5){
		resourceNode->type = 2;
	}
	else if(d > 0.4 * 0.4){
		resourceNode->type = 1;
	}
	else{
		resourceNode->type = 0;
	}
}

/* -------------------------------------------------------------------------- */

/* createDroppedIceCream

   This just mallocs a space for a DroppedIceCream struct and initialises
	 important values */

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

/* -------------------------------------------------------------------------- */

/* createRoamingSpider

	 This just mallocs space for a RoamingSpider struct and initialises important
	 parts to known values */

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

/* -------------------------------------------------------------------------- */

/* updateGameObjects

	 This function is just an intermediatary between the various static functions
	 inside this module that actually update the game objects and draw them onto
	 the screen */

void updateGameObjects(GameObjectData *gameObjectData, AudioData *audioData, AnnouncementsData *announcementsData, int ticks){

	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateWeather(gameObjectData, audioData, &gameObjectData->environment.weather, ticks);
	}
	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateEnvironment(gameObjectData,announcementsData,ticks);
  }
	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateResourceNodes(gameObjectData,ticks);
	}
	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateRoamingSpider(gameObjectData,announcementsData,ticks);
	}
	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateIceCreamPerson(gameObjectData,announcementsData,ticks);
	}
	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateIceCream(gameObjectData,ticks);
	}
	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateHive(gameObjectData,announcementsData,ticks);
  }
	if(!gameObjectData->gameOver && !gameObjectData->pause_status){
		updateProgrammableWorkers(gameObjectData,announcementsData,audioData,ticks);
	}
}

/* updateWeather

   This does the weather updating based on the ticks (ms) of the current frame */

static void updateWeather(GameObjectData *gameObjectData, AudioData *audioData, Weather *weather, int ticks){
	/* Advance weather every TICKSPERWEATHER ticks; this may be semi-random due to tick-skipping. */
	int weatherChannel = 3;

	if(weather->tickCount != -1){
		weather->tickCount += ticks;
	}

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
			fadeInChannel(weatherChannel, audioData, "thunder");
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
}

/* updateEnvironment

   This is where we progress the seasons */

static void updateEnvironment(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks){
	char finalScore[255];
	switch(gameObjectData->environment.season){
		case WINTER:
			if(gameObjectData->first_programmable_worker == NULL){
				gameObjectData->gameOver = 1;
				gameObjectData->gameOverCause = COLD;
				gameObjectData->years_survived++;
				setGameOverInfo(&announcementsData->gameOverData,
					              "None of your bees took shelter in the hive over winter!");
				sprintf(finalScore,
					      "YEARS SURVIVED = %d    SUGAR: %d", gameObjectData->years_survived,
				        gameObjectData->hive.flowers_collected);
				setFinalScore(&announcementsData->gameOverData, finalScore);
				return;
			}
			if(gameObjectData->environment.delayBeforeSummer <= 0){
				/* Go to Spring/Summer */
				announce(announcementsData,"Spring has come!");
				gameObjectData->environment.season = SUMMER;
				gameObjectData->environment.winterCountdownFloat = MAX_DAYS_TO_WINTER;
				gameObjectData->environment.winterCountdown = (int) gameObjectData->environment.winterCountdownFloat;
				gameObjectData->environment.treeGraphic = SUMMER_INDEX;
				gameObjectData->years_survived++;
			}
			gameObjectData->environment.delayBeforeSummer -= ticks;
			break;
		case AUTUMN:
			if(gameObjectData->environment.winterCountdown <= 0){
				announce(announcementsData,"Winter has come!");
				gameObjectData->environment.treeGraphic = WINTER_INDEX;
				gameObjectData->environment.delayBeforeSummer = DELAY_BEFORE_SUMMER;
				gameObjectData->environment.season = WINTER;
			}
		case SUMMER:
			if(gameObjectData->environment.winterCountdown >= AUTUMN_THRESHOLD){
				gameObjectData->environment.treeGraphic = AUTUMN_INDEX;
				gameObjectData->environment.season = AUTUMN;
			}
		case SPRING:
			gameObjectData->environment.winterCountdownFloat-= WINTER_COUNTDOWN_SPEED * ticks;
			gameObjectData->environment.winterCountdown = (int) gameObjectData->environment.winterCountdownFloat;
	}
}

/* updateResourceNodes

   this is where we update the ResourceNodeSpawners and thereby update thereby
	 resource nodes */

static void updateResourceNodes(GameObjectData *gameObjectData, int ticks){
	int i;
	i = 0;
	/* Second, we loop through all the ResourceNodeSpawners */
	while(i < gameObjectData->resourceNodeSpawnerCount){
		updateResourceNodeSpawner(gameObjectData,&gameObjectData->resourceNodeSpawners[i],ticks);
		i++;
	}
}

static void updateResourceNodeSpawner(GameObjectData *gameObjectData, ResourceNodeSpawner *spawner, int ticks){
	/* ResourceNodeSpawner *spawner = the spawner that we are updating

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
		}
	}
}

static int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner){
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

/* updateRoamingSpider

	 This function updates the RoamingSpider.

	 TODO: shorten this */

static void updateRoamingSpider(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks){

	double newX,newY;
	int distanceFromYBorder;
	int distanceFromXBorder;
	SDL_Point spiderCentre;

	/*set roamingSpider->currently_on_screen to false if he has walked off screen*/
	if(!gameObjectData->roamingSpider->currently_on_screen){
	 	if((rand() % ICE_CREAM_PERSON_PROB == 0)){
			announce(announcementsData,"A spider appeared!");
	 		reInitialiseRoamingSpider(gameObjectData->roamingSpider);
		}
		else{
			return;
		}
	}

	spiderCentre = getCenterOfRect(gameObjectData->roamingSpider->rect);

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
	}
	else{
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
				gameObjectData->roamingSpider->speed = SPIDER_SPEED/2;
				gameObjectData->roamingSpider->ticksSinceEating = 0;
				break;

			case 3:
				gameObjectData->roamingSpider->stung = 1;
				/*stop eating bee*/
				gameObjectData->roamingSpider->eating_bee = 0;
				/*spider back to movinng*/
				gameObjectData->roamingSpider->speed = SPIDER_SPEED/2;
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

			}
		}

		/*stung duration / time till active again */
		if(gameObjectData->roamingSpider->stung == 1) {
			gameObjectData->roamingSpider->ticksSinceStung += ticks;
			if(gameObjectData->roamingSpider->ticksSinceStung >= 20000) {
				gameObjectData->roamingSpider->stung = 0;
				gameObjectData->roamingSpider->ticksSinceStung = 0;
				gameObjectData->roamingSpider->speed = SPIDER_SPEED;
			}
		}

	}
	else{
		gameObjectData->roamingSpider->ticksSinceDead += ticks;
		if(gameObjectData->roamingSpider->ticksSinceDead >= 60000) {
			reInitialiseRoamingSpider(gameObjectData->roamingSpider);
		}
	}

	/*If there are fewer than 3 bees perilously close*/
	if(countProgrammableWorkersInRange(gameObjectData, getCenterOfRect(gameObjectData->roamingSpider->rect), 250.0) <= 3 && !gameObjectData->roamingSpider->going_home){
	  ProgrammableWorker *pw = getNearestWorker(gameObjectData, spiderCentre.x, spiderCentre.y, NULL);
		SDL_Point pp = getCenterOfRect(pw->rect);
		if(isPointInRangeOf(pp, spiderCentre,2000.0 ) && !gameObjectData->roamingSpider->stung){
			gameObjectData->roamingSpider->heading = getAngleBetweenRects(&pw->rect, &gameObjectData->roamingSpider->rect);
		}
		else{
			if(gameObjectData->roamingSpider->xPosition >= X_SIZE_OF_WORLD - PERSON_WIDTH/2){
					/*world border has been reached and sun is still out, change direction*/
				gameObjectData->roamingSpider->heading = 1.571;
			}
			if(gameObjectData->roamingSpider->xPosition >= X_SIZE_OF_WORLD - PERSON_WIDTH/2){
				/*world border has been reached and sun is still out, change direction*/
			gameObjectData->roamingSpider->heading = 1.571;

			}
			else if(gameObjectData->roamingSpider->xPosition <= -PERSON_WIDTH/2){
				/*world border has been reached and sun is still out, change direction*/
			gameObjectData->roamingSpider->heading = 4.713;

			}
			else if(gameObjectData->roamingSpider->yPosition >= Y_SIZE_OF_WORLD - PERSON_HEIGHT/2){
				/*world border has been reached and sun is still out, change direction*/
			gameObjectData->roamingSpider->heading = 3.142;

			}
			else if(gameObjectData->roamingSpider->yPosition <= -PERSON_HEIGHT/2){
				/*world border has been reached and sun is still out, change direction*/
			gameObjectData->roamingSpider->heading = 0;

			}
			else if(rand() % 500 == 0){
				/*randomly change direction, just for the hell of it*/
		 		gameObjectData->roamingSpider->heading += ((double)(rand() % 30) / (double)10) - 1.5;
		 	}
		}

	}
	else{
		distanceFromYBorder = (int)(gameObjectData->roamingSpider->yPosition - Y_SIZE_OF_WORLD/2);
		distanceFromXBorder = (int)(gameObjectData->roamingSpider->xPosition - X_SIZE_OF_WORLD/2);
		if(abs(distanceFromXBorder) > abs(distanceFromYBorder)){
			if(distanceFromXBorder > 0){
				gameObjectData->roamingSpider->heading = PI * 0.75 + 0.069813;
			}
			else{
				gameObjectData->roamingSpider->heading = PI * 0.25 + 0.069813;
			}
		}
		else{
			if(distanceFromYBorder > 0){
				gameObjectData->roamingSpider->heading = 0 + 0.069813;
			}
			else{
				gameObjectData->roamingSpider->heading = PI * 0.5 + 0.069813;
			}
		}
	}
}


/* reInitialiseRoamingSpider

   This makes the spider "respawn" */

static void reInitialiseRoamingSpider(RoamingSpider *roamingSpider){
	roamingSpider->currently_on_screen = 1;



	/*random chance of appearing at the edge of the world*/
	if(rand()%2){
		if(rand()%2){
			roamingSpider->xPosition = X_SIZE_OF_WORLD - PERSON_WIDTH;
			roamingSpider->heading = 1.571;
		}
		else{
			roamingSpider->xPosition = 0;
			roamingSpider->heading = 4.713;
		}
		roamingSpider->yPosition = rand() % Y_SIZE_OF_WORLD;

	}
	else{
		if(rand()%2){
			roamingSpider->yPosition = Y_SIZE_OF_WORLD - PERSON_HEIGHT;
			roamingSpider->heading = 3.142;
		}
		else{
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


/* updateIceCreamPerson

	 updates the ice cream person */

static void updateIceCreamPerson(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks){

	double newX,newY;
	int distanceFromYBorder;
	int distanceFromXBorder;
	ProgrammableWorker *programmableWorker;

	if(!gameObjectData->iceCreamPerson->currently_on_screen){
	 	if(gameObjectData->environment.weather.present_weather == Sun &&
			 gameObjectData->environment.winterCountdown >= AUTUMN_THRESHOLD &&
			 (rand() % ICE_CREAM_PERSON_PROB == 0) && !gameObjectData->droppedIceCream->dropped){

			announce(announcementsData,"The ice cream man appeared!");
	 		reInitialiseIceCreamPerson(gameObjectData->iceCreamPerson);

		}
		else{
			/* If he's not reinitialised then we can stop trying to update him now */
			return;
		}
	}

	/*set iceCreamPerson to going home if sun has gone, or he has lost his ice cream*/
	if(!(gameObjectData->environment.weather.present_weather == Sun && gameObjectData->iceCreamPerson->has_ice_cream)){
		gameObjectData->iceCreamPerson->going_home = 1;
	}
	/*set iceCreamPerson to running if it is raining*/
	if(gameObjectData->environment.weather.present_weather == Rain){
		gameObjectData->iceCreamPerson->speed = 0.1;
	}
	else{
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

				if(gameObjectData->iceCreamPerson->has_ice_cream){
		  		gameObjectData->iceCreamPerson->stung = 1;
		  		/*drop iceCream!*/
		  		gameObjectData->iceCreamPerson->has_ice_cream = 0;

		  		gameObjectData->droppedIceCream->xPosition = gameObjectData->iceCreamPerson->xPosition;
		  		gameObjectData->droppedIceCream->yPosition = gameObjectData->iceCreamPerson->yPosition;

			  	gameObjectData->droppedIceCream->dropped = 1;
				}

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

		}
		else if(gameObjectData->iceCreamPerson->xPosition <= -PERSON_WIDTH/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->iceCreamPerson->heading = 4.713;

		}
		else if(gameObjectData->iceCreamPerson->yPosition >= Y_SIZE_OF_WORLD - PERSON_HEIGHT/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->iceCreamPerson->heading = 3.142;

		}
		else if(gameObjectData->iceCreamPerson->yPosition <= -PERSON_HEIGHT/2){
			/*world border has been reached and sun is still out, change direction*/
		gameObjectData->iceCreamPerson->heading = 0;

		}
		else if(rand() % 1000 == 0){
			/*randomly change direction, just for the hell of it*/
	 		gameObjectData->iceCreamPerson->heading += ((double)(rand() % 30) / (double)10) - 1.5;
	 	}

	}
	else{
		distanceFromYBorder = (int)(gameObjectData->iceCreamPerson->yPosition - Y_SIZE_OF_WORLD/2);
		distanceFromXBorder = (int)(gameObjectData->iceCreamPerson->xPosition - X_SIZE_OF_WORLD/2);
		if(abs(distanceFromXBorder) > abs(distanceFromYBorder)){
			if(distanceFromXBorder > 0){
				gameObjectData->iceCreamPerson->heading = PI * 0.75 + 0.069813;
			}
			else{
				gameObjectData->iceCreamPerson->heading = PI * 0.25 + 0.069813;
			}
		}
		else{
			if(distanceFromYBorder > 0){
				gameObjectData->iceCreamPerson->heading = 0 + 0.069813;
			}
			else{
				gameObjectData->iceCreamPerson->heading = PI * 0.5 + 0.069813;
			}
		}
	}
}

static void reInitialiseIceCreamPerson(IceCreamPerson *iceCreamPerson){
	iceCreamPerson->currently_on_screen = 1;
	/*random chance of appearing at the edge of the world*/
	if(rand()%2){
		if(rand()%2){
			iceCreamPerson->xPosition = X_SIZE_OF_WORLD - (PERSON_WIDTH*2);
			iceCreamPerson->heading = 1.571;
		}
		else{
			iceCreamPerson->xPosition = 0 - PERSON_WIDTH;
			iceCreamPerson->heading = 4.713;
		}
		iceCreamPerson->yPosition = rand() % Y_SIZE_OF_WORLD;
	}
	else{
		if(rand()%2){
			iceCreamPerson->yPosition = Y_SIZE_OF_WORLD - (PERSON_HEIGHT*2);
			iceCreamPerson->heading = 3.142;
		}
		else{
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

static void updateIceCream(GameObjectData *gameObjectData, int ticks){
	if(gameObjectData->droppedIceCream->dropped){
		/*but not yet melted*/
		if(gameObjectData->droppedIceCream->droppedTimer < MELT_TIME_THRESHOLD){

			gameObjectData->droppedIceCream->droppedTimer += ticks;
		}
		else{
  		gameObjectData->droppedIceCream->dropped = 0;
  		gameObjectData->droppedIceCream->droppedTimer = 0;
    }
	}
}

static void updateHive(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks){
	int i = 0;
	char finalScore[255];

	if(gameObjectData->environment.season == WINTER){
		gameObjectData->hive.flowers_collected -= (rand()%10) ? 0 : 1;
		if(gameObjectData->hive.flowers_collected <= 0){
			gameObjectData->gameOver = 1;
			gameObjectData->gameOverCause = STARVATION;
			gameObjectData->years_survived++;
			sprintf(finalScore,
				      "YEARS SURVIVED = %d    SUGAR: %d",
			        gameObjectData->years_survived,
			        gameObjectData->hive.flowers_collected);

			setGameOverInfo(&announcementsData->gameOverData, "Your bees ran out of food over winter and died!");
			setFinalScore(&announcementsData->gameOverData, finalScore);
			killAllBees(&gameObjectData->first_programmable_worker);
			return;
		}
	}

	while(i < NUMBER_OF_CELLS_IN_HIVE){
		updateHiveCell(gameObjectData, announcementsData,&gameObjectData->hive.hiveCells[i],ticks);
		i++;
	}
}

static void killAllBees(ProgrammableWorker **programmableWorker){
	ProgrammableWorker *p;
	while(*programmableWorker != NULL){

		p = *programmableWorker;
		*programmableWorker = (*programmableWorker)->next;

		free(p);
	}
}

static void updateHiveCell(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, HiveCell *hiveCell, int ticks){
	if(hiveCell->timer >= 0){
		hiveCell->timer -= ticks;
		if(hiveCell->timer <= 0){
			announce(announcementsData,"A new worker was created!");
			createProgrammableWorker(gameObjectData);
			hiveCell->timer = -1;
		}
	}
}

static void updateProgrammableWorkers(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, AudioData *audioData, int ticks){
	ProgrammableWorker *programmableWorker;
	ProgrammableWorker *next = NULL;
	if(!gameObjectData->pause_status){
		for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL; programmableWorker = next){
			/* This slightly awkward way of doing things means that we can delete from inside the loop */
			next = programmableWorker->next;
			updateProgrammableWorker(&programmableWorker,gameObjectData,announcementsData,audioData,ticks);
		}
	}
}

int updateProgrammableWorker(ProgrammableWorker **programmableWorkerP, GameObjectData *gameObjectData, AnnouncementsData *announcementsData, AudioData *audioData, int ticks){

	ResourceNodeSpawner *resourceNodeSpawner = NULL;
	ResourceNode *resourceNode = NULL;
	char announcement[256];
	ProgrammableWorker *programmableWorker = *programmableWorkerP;


	if(gameObjectData->environment.winterCountdown < WINTER_THRESHOLD && !isPointInRangeOf(getCenterOfRect(programmableWorker->rect), getCenterOfRect(gameObjectData->hive.rect),HIVE_SHELTER_RADIUS)){
		programmableWorker->cold_and_about_to_die++;
		if(programmableWorker->cold_and_about_to_die > COLD_DEATH_THRESHOLD){
			killProgrammableWorker(gameObjectData, programmableWorkerP);
			return 1;
		}
	}
	else{
		if(isPointInRangeOf(getCenterOfRect(programmableWorker->rect), getCenterOfRect(gameObjectData->hive.rect),HIVE_SHELTER_RADIUS)){
			programmableWorker->insideHive = 1;
		}
		else{
			programmableWorker->insideHive = 0;
		}
		programmableWorker->cold_and_about_to_die = 0;

		updateProgrammableWorkerWeather(gameObjectData, programmableWorker, ticks);

		if(!programmableWorker->wet_and_cant_fly){ /*programmable worker has not been caught in rain recently*/
			programmableWorker->flapTimer += ticks;
			if(programmableWorker->flapTimer > MS_BETWEEN_FLAPPING){
				programmableWorker->flapTimer -= MS_BETWEEN_FLAPPING;
				programmableWorker->currentGraphicIndex = (programmableWorker->currentGraphicIndex + 1) % 2;
			}

			if(programmableWorker->stunned_after_sting){
				/* Make bees dizzy after stinging */
				programmableWorker->heading += (rand()%3)-1;
				if(programmableWorker->stunned_after_sting++ > STUNNED_AFTER_STING_DURATION){
					programmableWorker->stunned_after_sting = 0;
				}
			}

			updateProgrammableWorkerMove(programmableWorker, ticks);

			if(!programmableWorker->stunned_after_sting){
				resourceNode = checkResourceNodeCollision(&resourceNodeSpawner,gameObjectData,programmableWorker,WORKER_SENSE_RANGE);
			}

			if(programmableWorker->status == IDLE){
					updateProgrammableWorkerIdle(gameObjectData, programmableWorker);
			}
			else if(getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect) < 50.0 && programmableWorker->status == RETURNING){
				if(programmableWorker->cargo > 0){
					gameObjectData->hive.flowers_collected += programmableWorker->cargo;
					programmableWorker->cargo = 0;
					if(gameObjectData->hive.flowers_collected % CELEBRATION_THRESHOLD == 0){
						playSoundEffect(2, audioData, "returnFlower");
						sprintf(announcement,"Congratulations you have collected %d flowers!",gameObjectData->hive.flowers_collected);
						announce(announcementsData, announcement);
					}
				}
				programmableWorker->status = IDLE;
			}
			else if(programmableWorker->status == LEAVING){
				if(!programmableWorker->stunned_after_sting){
					if(isPointInRangeOf(getCenterOfRect(programmableWorker->rect),getCenterOfRect(gameObjectData->droppedIceCream->rect),(double)ICECREAM_PICKUP_RADIUS) &&
						 gameObjectData->droppedIceCream->dropped){
			   				/* status being 1 means that the bee heading away from the center */

			   				/*if programmable worker is clsoe enough to dropped icecream, it is not the very same second that the person dropped it*/
			   				/* then the worker picks up the iceCream*/

						programmableWorker->cargo += SUGAR_VALUE_OF_ICECREAM;
						gameObjectData->droppedIceCream->dropped = 0;
						gameObjectData->droppedIceCream->droppedTimer = 0;
					}
					else{
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
		else{
		}

		/*for battling with enemies*/
		if(updateProgrammableWorkerCombat(gameObjectData, programmableWorker,announcementsData)){
			/* Returns 1 if its died from spider/enemy related injuries */
			return 1;
		};
		updateProgrammableWorkerSenses(programmableWorker, resourceNodeSpawner);
	}
	*programmableWorkerP = programmableWorker;
	return 0;
}

static void updateProgrammableWorkerWeather(GameObjectData *gameObjectData, ProgrammableWorker *programmableWorker, int ticks){
	int i, j = 0;
	if(gameObjectData->environment.weather.present_weather == Rain){
		if(programmableWorker->weatherproofTimer <= 0){
			for(i = 0; i < NUMBER_OF_TREES; i++){
				if(!testRectIntersection(programmableWorker->rect, gameObjectData->tree[i].rect)){
					j++;
				}
			}
			if(!programmableWorker->insideHive){
				j++;
			}
			if(j == NUMBER_OF_TREES + 1 /* + 1 includes the hive as shelter*/){
				programmableWorker->wet_and_cant_fly =  1;/*true*/
			}
		}
		else{
			programmableWorker->weatherproofTimer -= ticks;
		}
	}
  else if(programmableWorker->wet_and_cant_fly){
		/*programmable worker has been caught in rain and hasn't regained ability to fly yet*/
		if((rand() % CHANCE_OF_REGAINING_FLIGHT) == 1){
			programmableWorker->wet_and_cant_fly = 0;
			programmableWorker->weatherproofTimer = BASE_WEATHERPROOF + rand() % RANDOM_WEATHERPROOF;
			programmableWorker->currentGraphicIndex = 0;
		}
		if(!(rand() % 100)){
		   programmableWorker->currentGraphicIndex = (programmableWorker->currentGraphicIndex + 1) % 2;
		}
	}
}

static void updateProgrammableWorkerMove(ProgrammableWorker *programmableWorker, int ticks){
			double newX,newY;
			/* Because we're using a heading/velocity movement system here, we have to use
			some trigonometry to work out the new positions */
			newX = sin(programmableWorker->heading);
			newY = cos(programmableWorker->heading);
			newX *= programmableWorker->speed * ticks * (programmableWorker->cargo > 0 ? PROGRAMMABLE_WORKER_CARGO_SPEED_FACTOR : 1.0);
			newY *= programmableWorker->speed * ticks * (programmableWorker->cargo > 0 ? PROGRAMMABLE_WORKER_CARGO_SPEED_FACTOR : 1.0);

			/* These are then the tentative final new positions for the ProgrammableWorker
			we're updating */
			programmableWorker->rawX += newX;
			programmableWorker->rawY += newY;
			programmableWorker->rect.x = (int)floor(programmableWorker->rawX);
			programmableWorker->rect.y = (int)floor(programmableWorker->rawY);
}

/* checkResourceNodeCollision

	 This checks several things at once because it's computationally quicker than
	 doing them all independently.

	 For notes on our collision detection structure see the function documentation
	 above for countResourceNodesInRadius and/or the documentation for
	 ResourceNodeSpawners.

	 First, we check through the ResourceNodeSpawners which allows us to write any
	 found one's address into the pointer-to-a-pointer provided as an argument
	 (unless it's NULL).

	 Then if we have any cases where we are close enough to a ResourceNodeSpawner
	 enough that a collision between a programmableWorker and a ResourceNode could
	 occur, we then check all the ResourceNodes parented to that spawner, once
	 we've checked that the ResourceNode is alive */

static ResourceNode *checkResourceNodeCollision(ResourceNodeSpawner **resourceNodeSpawnerPointer,
																				 				GameObjectData *gameObjectData,
																				 				ProgrammableWorker *programmableWorker,
																			   				double sense_range){
	int j = 0;
	SDL_Point worker_p;
	SDL_Point node_p;
	double d2;
	volatile int startPoint = (rand() % gameObjectData->resourceNodeSpawnerCount);
	startPoint = startPoint == 0 ? gameObjectData->resourceNodeSpawnerCount - 1 : startPoint;
	int i = (startPoint == gameObjectData->resourceNodeSpawnerCount - 1) ? 0 : startPoint + 1;
	worker_p = getCenterOfRect(programmableWorker->rect);
	while(i != startPoint){
		d2 = getDistance2BetweenPoints(worker_p.x,
											 					 worker_p.y,
											 				   (int)gameObjectData->resourceNodeSpawners[i].xPosition,
											 				   (int)gameObjectData->resourceNodeSpawners[i].yPosition);
		if(d2 <= pow((sense_range + gameObjectData->resourceNodeSpawners[i].spawnRadius + X_SIZE_OF_NODE),2)){
			j = 0;
			while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
				node_p = getCenterOfRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect);
				if(isPointInRect(node_p.x,node_p.y,programmableWorker->rect)
					&& gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){
						*resourceNodeSpawnerPointer = &gameObjectData->resourceNodeSpawners[i];
						return(&gameObjectData->resourceNodeSpawners[i].resourceNodes[j]);
				}
				else if(resourceNodeSpawnerPointer != NULL &&
				        getDistance2BetweenPoints(node_p.x,node_p.y,worker_p.x,worker_p.y) <= pow(sense_range + X_SIZE_OF_NODE,2)){
					*resourceNodeSpawnerPointer = &gameObjectData->resourceNodeSpawners[i];
				}
				j++;
			}
		}
		i = (i + 1) % gameObjectData->resourceNodeSpawnerCount;
	}

	return(NULL);
}

static void updateProgrammableWorkerIdle(GameObjectData *gameObjectData, ProgrammableWorker *programmableWorker){
	double pX,pY;
	if(getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect) > 100 * 100){
		pX = (double)(gameObjectData->hive.rect.x + gameObjectData->hive.rect.w/2 - 100 + rand() % 200);
		pY = (double)(gameObjectData->hive.rect.y + gameObjectData->hive.rect.h/2 - 100 + rand() % 200);
		programmableWorker->heading = atan2(pX - programmableWorker->rawX,pY - programmableWorker->rawY);
	}
}

static int updateProgrammableWorkerCombat(GameObjectData *gameObjectData, ProgrammableWorker *programmableWorker, AnnouncementsData *announcementsData){
	if(isPointInRangeOf(getCenterOfRect(programmableWorker->rect), getCenterOfRect(gameObjectData->roamingSpider->rect),SPIDER_ATTACK_AREA)){

		if(!gameObjectData->roamingSpider->stung){
			announce(announcementsData,"A spider is attacking one of your bees!");
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
			return 1;
		}
	}
	return 0;
}

static void killProgrammableWorker(GameObjectData *gameObjectData, ProgrammableWorker **toDeletePointer){
	/* I tidied this up a bit by giving things new names - the compiler will optimise out
		 most of these variables anyway */
	ProgrammableWorker *p = NULL;
	ProgrammableWorker *toDelete = *toDeletePointer;
	/* Clean up if we have the bee selected */
	if(gameObjectData->gameObjectSelection.selected == toDelete){
		gameObjectData->gameObjectSelection.selected = NULL;
		gameObjectData->gameObjectSelection.visible = 0;
	}

	if(toDelete == gameObjectData->first_programmable_worker){
		/* Start case */
		gameObjectData->first_programmable_worker = gameObjectData->first_programmable_worker->next;
		*toDeletePointer = NULL;
		free(toDelete);
	}
	else if((toDelete)->next == NULL){
		/* End case */
    for(p = gameObjectData->first_programmable_worker;
			  p!=NULL && p->next != toDelete;
				p = p->next){;}
		if(p!=NULL){
	    p->next = NULL;
		}
		*toDeletePointer = NULL;
		free(toDelete);
	}
	else{
    for(p = gameObjectData->first_programmable_worker;
        p->next != toDelete ;
        p = p->next){;}
    p->next = toDelete->next;
		*toDeletePointer = NULL;
		free(toDelete);
	}
}

static void updateProgrammableWorkerSenses(ProgrammableWorker *programmableWorker, ResourceNodeSpawner *resourceNodeSpawner){
	/* Sensory Perception */
	ResourceNode *resourceNode;
	if(programmableWorker->brain.foundNode != NULL &&
		 (getDistance2BetweenRects(programmableWorker->rect,programmableWorker->brain.foundNode->rect) >= WORKER_SENSE_RANGE * WORKER_SENSE_RANGE ||
		 !programmableWorker->brain.foundNode->alive)){
			 programmableWorker->brain.foundNode = NULL;
	}
	if(programmableWorker->brain.foundNode == NULL && resourceNodeSpawner != NULL){
		 resourceNode = chooseNodeRandomly(resourceNodeSpawner);
		 if(resourceNode != NULL && getDistance2BetweenRects(programmableWorker->rect,resourceNode->rect) <= WORKER_SENSE_RANGE * WORKER_SENSE_RANGE){
			 programmableWorker->brain.foundNode = resourceNode;
		 }
	}
}

static ResourceNode *chooseNodeRandomly(ResourceNodeSpawner *resourceNodeSpawner){
	int i = 0, r = rand();
	if(resourceNodeSpawner->currentNodeCount > 0){
		r = (r % (resourceNodeSpawner->currentNodeCount));
		while(i < resourceNodeSpawner->maximumNodeCount){
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

/* -------------------------------------------------------------------------- */

void renderGameObjects(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	blitBackground(gameObjectData,graphicsData);
	blitNodes(gameObjectData,graphicsData);
	blitHive(gameObjectData,graphicsData);
	blitIceCreamPerson(gameObjectData,graphicsData);
	blitIceCream(gameObjectData,graphicsData);
	blitRoamingSpider(gameObjectData,graphicsData);
	blitDisabledWorkers(gameObjectData,graphicsData);
	blitProgrammableWorkers(gameObjectData,graphicsData);
	blitTrees(gameObjectData,graphicsData);
	blitWeather(gameObjectData,graphicsData);
	blitSelection(gameObjectData,graphicsData);
}

static void blitBackground(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	switch(gameObjectData->environment.season){
		case WINTER:
				SDL_SetRenderTarget(graphicsData->renderer, NULL);
				SDL_SetRenderDrawColor(graphicsData->renderer, 230, 230, 230, 255);
				SDL_RenderFillRect(graphicsData->renderer, NULL);
				break;
			case AUTUMN:
				blitTiledBackground(graphicsData,
														graphicsData->grass->graphic[SUMMER_INDEX + 1]);
				break;
			case SUMMER:
				blitTiledBackground(graphicsData,
					                  graphicsData->grass->graphic[SUMMER_INDEX]);
			case SPRING:
				break;

	}
}

static void blitTrees(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	int i = 0;
	for(i = 0; i < NUMBER_OF_TREES; i++){

		blitGameObject(gameObjectData->tree[i].stumpRect,
					         graphicsData,
					 	       graphicsData->treeStumpTexture,
					      	 0,
					 	       NULL,
					 	       SDL_FLIP_NONE);

	 	blitParallaxTreeTops(gameObjectData->tree[i].rect,
						             graphicsData,
						             graphicsData->shelter->graphic[gameObjectData->environment.treeGraphic]);
	}
}

static void blitNodes(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	int i,j;
	i = 0;
	/* Second, we loop through all the ResourceNodeSpawners */
	while(i < gameObjectData->resourceNodeSpawnerCount){
		j = 0;
		/* Then we need to loop through the attached ResourceNodes and draw them */
		p.x = (int) gameObjectData->resourceNodeSpawners[i].xPosition;
		p.y = (int) gameObjectData->resourceNodeSpawners[i].yPosition;
		 while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
			if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){
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
}

static void blitHive(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	blitGameObject(gameObjectData->hive.rect,
				 				 graphicsData,
				 			 	 graphicsData->hiveTexture,
				 			 	 0,
				 			 	 NULL,
				 			 	 SDL_FLIP_NONE);
}

static void blitIceCreamPerson(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	if(gameObjectData->iceCreamPerson->currently_on_screen){
		blitGameObject(gameObjectData->iceCreamPerson->rect,
								 	 graphicsData,
								 	 graphicsData->person->graphic[gameObjectData->iceCreamPerson->currentGraphicIndex +
								 	 ((gameObjectData->iceCreamPerson->has_ice_cream) ? 0 : NO_ICECREAM_INDEX_OFFSET)],
								 	 DEGREESINCIRCLE-(gameObjectData->iceCreamPerson->heading * RADIANSTODEGREES),
								 	 NULL,
								 	 SDL_FLIP_NONE);
	}
}

static void blitIceCream(GameObjectData *gameObjectData,GraphicsData *graphicsData){
	if(gameObjectData->droppedIceCream->dropped){


		if(gameObjectData->droppedIceCream->rect.w >= MAX_DROPPED_ICECREAM_WIDTH){
			gameObjectData->droppedIceCream->sizeOscillator = -1;
		}
		else if(gameObjectData->droppedIceCream->rect.w <= DROPPED_ICECREAM_WIDTH){
			gameObjectData->droppedIceCream->sizeOscillator = 1;
		}
		gameObjectData->droppedIceCream->rect.w += gameObjectData->droppedIceCream->sizeOscillator;
		gameObjectData->droppedIceCream->xPosition -= ((double)gameObjectData->droppedIceCream->sizeOscillator)/2.0;
		gameObjectData->droppedIceCream->rect.x = gameObjectData->droppedIceCream->xPosition;

		gameObjectData->droppedIceCream->rect.h += gameObjectData->droppedIceCream->sizeOscillator;
		gameObjectData->droppedIceCream->yPosition -= ((double)gameObjectData->droppedIceCream->sizeOscillator)/2.0;
		gameObjectData->droppedIceCream->rect.y = gameObjectData->droppedIceCream->yPosition;

		if(gameObjectData->droppedIceCream->droppedTimer < MELT_TIME_THRESHOLD + 40){
			blitGameObject(gameObjectData->droppedIceCream->rect,
								     graphicsData,
							       graphicsData->meltedIceCreamTexture,
								     0,
                     NULL,
								     SDL_FLIP_NONE);
		}
		else{
			blitGameObject(gameObjectData->droppedIceCream->rect,
										 graphicsData,
										 graphicsData->droppedIceCreamTexture,
										 0,
										 NULL,
										 SDL_FLIP_NONE);
		}
	}
}

static void blitRoamingSpider(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	if(gameObjectData->roamingSpider->currently_on_screen){
		blitGameObject(gameObjectData->roamingSpider->rect,
							 	 	 graphicsData,
							  	 graphicsData->roamingArachnid->graphic[gameObjectData->roamingSpider->currentGraphicIndex],
									 DEGREESINCIRCLE-(gameObjectData->roamingSpider->heading * RADIANSTODEGREES),
									 NULL,
									 SDL_FLIP_NONE);
	}
}

static void blitDisabledWorkers(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	SDL_Rect smallerBeeRect;
	ProgrammableWorker *programmableWorker;
	ProgrammableWorker *next;
	for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL ; programmableWorker = next){
		next = programmableWorker->next;

		if(programmableWorker->wet_and_cant_fly == 1 || programmableWorker->cold_and_about_to_die == 1){

			smallerBeeRect.w = (int)((double)programmableWorker->rect.w/BEE_SHRINK_FACTOR_ON_GROUND);
			smallerBeeRect.h = (int)((double)programmableWorker->rect.h/BEE_SHRINK_FACTOR_ON_GROUND);
			smallerBeeRect.y = programmableWorker->rect.y;
			smallerBeeRect.x = programmableWorker->rect.x;
			blitGameObject(smallerBeeRect,
										 graphicsData,
										 graphicsData->bee->graphic[programmableWorker->currentGraphicIndex > 5 ? 0 : programmableWorker->currentGraphicIndex],
										 DEGREESINCIRCLE-(programmableWorker->heading * RADIANSTODEGREES),
										 NULL,
										 SDL_FLIP_NONE);
		}
	}
}

static void blitProgrammableWorkers(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	ProgrammableWorker *programmableWorker;
	ProgrammableWorker *nextt = NULL;
	for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL ; programmableWorker = nextt){
	 nextt = programmableWorker->next;
	 if(!(programmableWorker->wet_and_cant_fly || programmableWorker->cold_and_about_to_die)){

		 /* This needs to have some changes to adjust the rawX and rawY values too */
		 //fitRectToWorld(&programmableWorker->rect);
		 /*render if now currently just above (in) hive*/

		 if(!isPointInRangeOf(getCenterOfRect(programmableWorker->rect),getCenterOfRect(gameObjectData->hive.rect), HIVE_SHELTER_RADIUS)){
			 blitGameObject(programmableWorker->rect,
					            graphicsData,
					            graphicsData->bee->graphic[programmableWorker->currentGraphicIndex +
					                                       ((programmableWorker->cargo < SUGAR_VALUE_OF_ICECREAM && programmableWorker->cargo > 0) ? CARRYING_FLOWER_INDEX_OFFSET :
					                                       ((programmableWorker->cargo == SUGAR_VALUE_OF_ICECREAM) ? CARRYING_ICECREAM_INDEX_OFFSET : 0))],
					            DEGREESINCIRCLE-(programmableWorker->heading * RADIANSTODEGREES),
					            NULL,
					            SDL_FLIP_NONE);
		 }
	 }
	}
}

static void blitWeather(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	if(gameObjectData->environment.weather.present_weather == Rain){
		blitRainRandomly(graphicsData);
	}
	paintWeatherLayer(graphicsData, gameObjectData->environment.weather.present_weather);
}

static void blitSelection(GameObjectData *gameObjectData, GraphicsData *graphicsData){
	SDL_Point point;

	if(gameObjectData->gameObjectSelection.selected==NULL){
		return;
	}

	switch(gameObjectData->gameObjectSelection.type){
		case GOS_PROGRAMMABLE_WORKER:
			sprintf(gameObjectData->gameObjectSelection.nameString,WORKER_NAME);
			getWorkerInfoString(gameObjectData->gameObjectSelection.selected,gameObjectData->gameObjectSelection.infoString);
			point = getCenterOfRect(((ProgrammableWorker*)gameObjectData->gameObjectSelection.selected)->rect);
			centerCameraOnPoint(graphicsData,point.x,point.y);
			renderRadius(graphicsData, &point, WORKER_SENSE_RANGE, 255,255,255, 255);
			break;
		case GOS_TREE:
			sprintf(gameObjectData->gameObjectSelection.nameString,"Tree");
			if(gameObjectData->gameObjectSelection.infoString[0] != '\0'){
				memset(gameObjectData->gameObjectSelection.infoString,0,strlen(gameObjectData->gameObjectSelection.infoString));
			}
			point = getCenterOfRect(((Tree*)gameObjectData->gameObjectSelection.selected)->rect);
			renderFillRadius(graphicsData, &point, TREE_SHELTER_RADIUS, 0,0,0, 40);
			break;
		case GOS_HIVE:
			sprintf(gameObjectData->gameObjectSelection.nameString,"Hive");
			if(gameObjectData->gameObjectSelection.infoString[0] != '\0'){
				memset(gameObjectData->gameObjectSelection.infoString,0,strlen(gameObjectData->gameObjectSelection.infoString));
			}
			point = getCenterOfRect(((Hive*)gameObjectData->gameObjectSelection.selected)->rect);
			renderFillRadius(graphicsData, &point, 40, 255,255,255, 80);
			break;
		case GOS_RESOURCE_NODE:
			sprintf(gameObjectData->gameObjectSelection.nameString,RESOURCE_NODE_NAME);
			getResourceNodeInfoString(gameObjectData->gameObjectSelection.selected,gameObjectData->gameObjectSelection.infoString);
			point.x = (int)((ResourceNode*)gameObjectData->gameObjectSelection.selected)->spawner->xPosition;
			point.y = (int)((ResourceNode*)gameObjectData->gameObjectSelection.selected)->spawner->yPosition;
			renderFillRadius(graphicsData, &point,((ResourceNode*)gameObjectData->gameObjectSelection.selected)->spawner->spawnRadius + WORKER_SENSE_RANGE, 255,255,255, 80);
			point = getCenterOfRect(((ResourceNode*)gameObjectData->gameObjectSelection.selected)->rect);
			renderFillRadius(graphicsData, &point,WORKER_SENSE_RANGE, 255,255,255, 80);
			break;
		case GOS_ICE_CREAM_PERSON:
			if(!((IceCreamPerson*)gameObjectData->gameObjectSelection.selected)->currently_on_screen) {
				gameObjectData->gameObjectSelection.selected = NULL;
			}
			else{
				sprintf(gameObjectData->gameObjectSelection.nameString,"Gary");
				sprintf(gameObjectData->gameObjectSelection.infoString,
					      ((IceCreamPerson*)gameObjectData->gameObjectSelection.selected)->has_ice_cream ? "Has Ice Cream":"");
				point = getCenterOfRect(((IceCreamPerson*)gameObjectData->gameObjectSelection.selected)->rect);
				centerCameraOnPoint(graphicsData,point.x,point.y);
				renderFillRadius(graphicsData, &point, PERSON_WIDTH/2, 255,255,255, 80);
			}
			break;
		case GOS_ROAMING_SPIDER:
			sprintf(gameObjectData->gameObjectSelection.nameString,"Spider");
			getSpiderInfoString(gameObjectData->gameObjectSelection.selected,gameObjectData->gameObjectSelection.infoString);
			point = getCenterOfRect(((RoamingSpider*)gameObjectData->gameObjectSelection.selected)->rect);
			centerCameraOnPoint(graphicsData,point.x,point.y);
			renderFillRadius(graphicsData, &point, PERSON_WIDTH/2, 255,255,255, 80);
			break;
		case GOS_DROPPED_ICE_CREAM:
			sprintf(gameObjectData->gameObjectSelection.nameString,"Ice Cream");
			sprintf(gameObjectData->gameObjectSelection.infoString,
				      "%d seconds until it melts",
							((DroppedIceCream*)gameObjectData->gameObjectSelection.selected)->droppedTimer);
			point = getCenterOfRect(((DroppedIceCream*)gameObjectData->gameObjectSelection.selected)->rect);
			renderFillRadius(graphicsData, &point, gameObjectData->droppedIceCream->rect.w/2, 255,255,255, 80);
			break;
		case GOS_NONE:
			return;
	}
}

static void getWorkerInfoString(ProgrammableWorker *programmableWorker, char *targetString){
	targetString[0] = '\0';
	switch(programmableWorker->status){
		case LEAVING:
			strcat(targetString,"Leaving the hive");
			break;
		case RETURNING:
			strcat(targetString,"Returning");
			break;
		case WANTING_TO_RETURN:
			strcat(targetString,"Does this even happen any more?");
			break;
		case NODE_FOUND:
			strcat(targetString,"I'm sure this doesn't exist\n");
			break;
		case IDLE:
			strcat(targetString,"Idle");
			break;
		case BLANK:
			strcat(targetString,"BLANK");
			break;
	}
	if(programmableWorker->cargo != 0){
		strcat(targetString," & has cargo");
	}
	if(programmableWorker->wet_and_cant_fly){
		strcat(targetString," & wet");
	}
	if(programmableWorker->stunned_after_sting){
		strcat(targetString," & stunned");
	}
}

static void getResourceNodeInfoString(ResourceNode *resourceNode, char *targetString){
		switch(resourceNode->type){
			case 0:
				sprintf(targetString,"Blue: %d",SUGAR_VALUE_OF_BLUE_FLOWER);
				break;
			case 1:
				sprintf(targetString,"Red: %d",SUGAR_VALUE_OF_RED_FLOWER);
				break;
			case 2:
				sprintf(targetString,"Yellow: %d",SUGAR_VALUE_OF_YELLOW_FLOWER);
				break;
		}
}

static void getSpiderInfoString(RoamingSpider *roamingSpider, char *targetString){
	/* This just tells the compiler not to throw unused parameter warnings when compiling */
	(void)roamingSpider;
	sprintf(targetString,"Insy winsy");
}

/* -------------------------------------------------------------------------- */

void selectGameObject(GameObjectData *gameObjectData, GraphicsData *graphicsData, SDL_Event *event){
	SDL_Point p;
	p.x = event->button.x - graphicsData->navigationOffset.x;
	p.y = event->button.y - graphicsData->navigationOffset.y;
	/* This function is called on every click to see whether we need to select a new game object */
	/* Order should be workers -> hive -> spider -> ice cream person -> ice cream -> resource node -> tree */

	gameObjectData->gameObjectSelection.visible = 0;

	if(!gameObjectData->gameObjectSelection.enabled){
		return;
	}

	gameObjectData->gameObjectSelection.selected = selectWorker(gameObjectData,p);
	if(gameObjectData->gameObjectSelection.selected != NULL){
		gameObjectData->gameObjectSelection.type = GOS_PROGRAMMABLE_WORKER;
		gameObjectData->gameObjectSelection.visible = 1;
		return;
	}

	gameObjectData->gameObjectSelection.selected = selectHive(gameObjectData,p);
	if(gameObjectData->gameObjectSelection.selected != NULL){
		gameObjectData->gameObjectSelection.type = GOS_HIVE;
		gameObjectData->gameObjectSelection.visible = 1;
		return;
	}

	gameObjectData->gameObjectSelection.selected = selectSpider(gameObjectData,p);
	if(gameObjectData->gameObjectSelection.selected != NULL){
		gameObjectData->gameObjectSelection.type = GOS_ROAMING_SPIDER;
		gameObjectData->gameObjectSelection.visible = 1;
		return;
	}

	gameObjectData->gameObjectSelection.selected = selectIceCreamPerson(gameObjectData,p);
	if(gameObjectData->gameObjectSelection.selected != NULL){
		gameObjectData->gameObjectSelection.type = GOS_ICE_CREAM_PERSON;
		gameObjectData->gameObjectSelection.visible = 1;
		return;
	}

	gameObjectData->gameObjectSelection.selected = selectDroppedIceCream(gameObjectData,p);
	if(gameObjectData->gameObjectSelection.selected != NULL){
		gameObjectData->gameObjectSelection.type = GOS_DROPPED_ICE_CREAM;
		gameObjectData->gameObjectSelection.visible = 1;
	}

	gameObjectData->gameObjectSelection.selected = selectResourceNode(gameObjectData,p);
	if(gameObjectData->gameObjectSelection.selected != NULL){
		gameObjectData->gameObjectSelection.type = GOS_RESOURCE_NODE;
		gameObjectData->gameObjectSelection.visible = 1;
		return;
	}

	gameObjectData->gameObjectSelection.selected = selectTree(gameObjectData,p);
	if(gameObjectData->gameObjectSelection.selected != NULL){
		gameObjectData->gameObjectSelection.type = GOS_TREE;
		gameObjectData->gameObjectSelection.visible = 1;
		return;
	}
}

static ProgrammableWorker *selectWorker(GameObjectData *gameObjectData, SDL_Point point){
	ProgrammableWorker *p = gameObjectData->first_programmable_worker;
	while(p != NULL){
		if(isPointInRect(point.x,point.y,p->rect)){
			return p;
		}
		p = p->next;
	}
	return NULL;
}

static Hive *selectHive(GameObjectData *gameObjectData, SDL_Point point){
	if(isPointInRect(point.x,point.y,gameObjectData->hive.rect)){
		return &gameObjectData->hive;
	}
	return NULL;
}

static RoamingSpider *selectSpider(GameObjectData *gameObjectData, SDL_Point point){
	if(gameObjectData->roamingSpider->currently_on_screen && isPointInRect(point.x,point.y,gameObjectData->roamingSpider->rect)){
		return gameObjectData->roamingSpider;
	}
	return NULL;
}

static IceCreamPerson *selectIceCreamPerson(GameObjectData *gameObjectData, SDL_Point point){
	if(gameObjectData->iceCreamPerson->currently_on_screen && isPointInRect(point.x,point.y,gameObjectData->iceCreamPerson->rect)){
		return gameObjectData->iceCreamPerson;
	}
	return NULL;
}

static DroppedIceCream *selectDroppedIceCream(GameObjectData *gameObjectData, SDL_Point point){
	if(gameObjectData->droppedIceCream->dropped && isPointInRect(point.x,point.y,gameObjectData->droppedIceCream->rect)){
		return gameObjectData->droppedIceCream;
	}
	return NULL;
}

static ResourceNode *selectResourceNode(GameObjectData *gameObjectData, SDL_Point point){
	int i = 0;
	int j = 0;
	SDL_Point point2;
	while(i < NUMBER_OF_FLOWER_PATCHES){
		point2 = getCenterOfRect(gameObjectData->resourceNodeSpawners[i].collisionRect);
		if(isPointInRangeOf(point,point2,gameObjectData->resourceNodeSpawners[i].spawnRadius)){
			j = 0;
			while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
				if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive &&
				   isPointInRect(point.x,point.y,gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect)){
					return &gameObjectData->resourceNodeSpawners[i].resourceNodes[j];
				}
				j++;
			}
		}
		i++;
	}
	return NULL;
}

static Tree *selectTree(GameObjectData *gameObjectData, SDL_Point point){
	int i = 0;
	while(i < NUMBER_OF_TREES){
		if(isPointInRect(point.x,point.y,gameObjectData->tree[i].rect)){
			return &gameObjectData->tree[i];
		}
		i++;
	}
	return NULL;
}

/* -------------------------------------------------------------------------- */


void centerViewOnHive(GraphicsData *graphicsData, GameObjectData *gameObjectData){
	SDL_Point p = getCenterOfRect(gameObjectData->hive.rect);
	int win_x, win_y;
	SDL_GetWindowSize(graphicsData->window,&win_x,&win_y);
	p.x -= win_x/2;
	p.y -= win_y/2;
	setNavigationOffset(graphicsData, p.x, p.y);
}

/* -------------------------------------------------------------------------- */

void nullifyLocalAIInformation(GameObjectData *gameObjectData){
	ProgrammableWorker *p = gameObjectData->first_programmable_worker;

	while(p != NULL){
		nullifyProgrammableWorkerBrain(&p->brain);
		p = p->next;
	}
}

static void nullifyProgrammableWorkerBrain(ProgrammableWorkerBrain *brain){
	brain->aiStartPoint = 0;
}

/* -------------------------------------------------------------------------- */

int countIdleWorkers(GameObjectData *gameObjectData){
	int count = 0;
	ProgrammableWorker *p = gameObjectData->first_programmable_worker;
	while(p != NULL){
		if(p->status == IDLE){
			count++;
		}
		p = p->next;
	}
	return count;
}
