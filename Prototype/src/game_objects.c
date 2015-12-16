#include "game_objects.h"

static ResourceNode *chooseNodeRandomly(ResourceNodeSpawner *resourceNodeSpawner);
static void shuffleAliveNodes(ResourceNodeSpawner *resourceNodeSpawner);

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

static void shuffleAliveNodes(ResourceNodeSpawner *resourceNodeSpawner){
  int start = 0;
  int end = resourceNodeSpawner->maximumNodeCount;

  while(start < end){
    if(!resourceNodeSpawner->resourceNodes[start].alive && resourceNodeSpawner->resourceNodes[end].alive){
      resourceNodeSpawner->resourceNodes[start] = resourceNodeSpawner->resourceNodes[end];
      resourceNodeSpawner->resourceNodes[end].alive = 0;
    }
    if(resourceNodeSpawner->resourceNodes[start].alive){
      start++;
    }
    if(!resourceNodeSpawner->resourceNodes[end].alive){
      end--;
    }
  }

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
  double d2;
  /* Loop through all the resourceNodeSpawners in the gameObjectData we got
     passed */
  while(i < gameObjectData->resourceNodeSpawnerCount){
    /* Get the distance2 between the resourceNodeSpawner and the point to check */
    /* This checks if we are close enough to a resourceNodeSpawner that we
       should check against the individual nodes*/
    if(testRectIntersection(programmableWorker->rect, gameObjectData->resourceNodeSpawners[i].collisionRect)){
      /* Reset j every time */
      j = 0;
      *resourceNodeSpawnerPointer = &gameObjectData->resourceNodeSpawners[i];
      /* Loop through the ResourceNodes on the resourceNodeSpawner and check
         collisions */
      while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
        /* Get the distance2 between the ResourceNode and the point to check */
        /* Check whether we are close enough to a ResourceNode to collide and
           that the node we're testing is actually alive.*/
        if(isPointInRect(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect.x + X_SIZE_OF_NODE/2,
		   gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect.y + Y_SIZE_OF_NODE/2,
		   programmableWorker->rect)
			&& gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){
          /* Both these values are essentially returned */
          return(&gameObjectData->resourceNodeSpawners[i].resourceNodes[j]);
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
  printf("found %d workers in range\n",i);
  return i;
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
  programmableWorker->rawX = 100.0;
  programmableWorker->rawY = 100.0;
  programmableWorker->rect.x = 100;
  programmableWorker->rect.y = 100;
  programmableWorker->rect.w = X_SIZE_OF_WORKER;
  programmableWorker->rect.h = Y_SIZE_OF_WORKER;
  
  programmableWorker->wet_and_cant_fly = 0;
  programmableWorker->currently_under_tree = 0;
  
  programmableWorker->currentGraphicIndex = BEE_FLAP_GRAPHIC_1;
  /* heading is measured in radians because maths in C all take radians */
  programmableWorker->heading = 0.0;
  /* This is basically pixels/milliseconds */
  programmableWorker->speed = WORKER_SPEED;
  /* I don't think this is used yet... */
  programmableWorker->type = 1;
  programmableWorker->cargo = 0;
  /* This is an enum detailed in game_objects.h */
  programmableWorker->status = LEAVING;
  programmableWorker->next = NULL;
  programmableWorker->brain.is_point_remembered = 0;
  programmableWorker->brain.followTarget = NULL;
  programmableWorker->brain.foundNode = NULL;
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
  iceCreamPerson->speed = 0;
  iceCreamPerson->stung_count = 0;
  iceCreamPerson->strings_until_ice_cream_drop = 0;
  return iceCreamPerson;
}

Weather createWeatherLayer(void){
  /* This function creates a Weather struct and fills in the default
     values. Many of these are defined in generic.h */
  Weather weather;
  printf("Created weather layer.");
  weather.present_weather = Sun;
  return(weather);
}


Hive createHive(void){
  /* This function creates a Hive struct and fills in the default
     values. Many of these are defined in generic.h */
  Hive hive;
  hive.rect.w = 64;
  hive.rect.h = 80;
  hive.rect.x = X_SIZE_OF_WORLD/2 - hive.rect.w/2;
  hive.rect.y = Y_SIZE_OF_WORLD/2 - hive.rect.h/2;
  printf("created hive at %d,%d",hive.rect.x,hive.rect.y);
  hive.flowers_collected = 0;
  return(hive);
}

Tree createTree(void){
  Tree tree;
  tree.rect.w = SIZE_OF_TREE;
  tree.rect.h = SIZE_OF_TREE;
  tree.rect.x = rand() % X_SIZE_OF_WORLD - SIZE_OF_TREE/2;
  tree.rect.y = rand() % Y_SIZE_OF_WORLD - SIZE_OF_TREE/2;
  tree.bees_taking_shelter = 0;
  return tree;
}

void updateProgrammableWorker(ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  /* ProgrammableWorker *programmableWorker = the ProgrammableWorker we're going
                                              to be updating
     GameObjectData *gameObjectData         = the pointer to the GameObjectData
                                              that we will be using when
                                              determining collision
     float ticks                            = The number of ticks to update for.*/
  double newX,newY;
  int i;
  ResourceNodeSpawner *resourceNodeSpawner = NULL;
  ResourceNode *resourceNode;
  
  if(!programmableWorker->wet_and_cant_fly){ /*programmable worker has not been caught in rain recently*/
  	printf("moo\n");
  	programmableWorker->currentGraphicIndex = (programmableWorker->currentGraphicIndex + 1) % 2;
  	
  	if(gameObjectData->weather.present_weather == Rain && !programmableWorker->currently_under_tree){
  		programmableWorker->wet_and_cant_fly = 1; /*true*/
  	}


  	/* Because we're using a heading/velocity movement system here, we have to use
     some trigonometry to work out the new positions */
  	newX = sin(programmableWorker->heading);
  	newY = cos(programmableWorker->heading);
  	newX *= programmableWorker->speed * ticks;
  	newY *= programmableWorker->speed * ticks;

 	 /* These are then the tentative final new positions for the ProgrammableWorker
     we're updating */
  	programmableWorker->rawX += newX;
  	programmableWorker->rawY += newY;
  	programmableWorker->rect.x = (int)floor(programmableWorker->rawX);
 	programmableWorker->rect.y = (int)floor(programmableWorker->rawY);

  	if(programmableWorker->brain.foundNode != NULL && !programmableWorker->brain.foundNode->alive){
    	programmableWorker->brain.foundNode = NULL;
  	}

  	printf("worker status: %d\n",programmableWorker->status);

  	if(getDistance2BetweenPoints(programmableWorker->rect.x + programmableWorker->rect.w/2,
							   	programmableWorker->rect.y + programmableWorker->rect.h/2,
							   	gameObjectData->hive.rect.x + gameObjectData->hive.rect.w/2,
							   	gameObjectData->hive.rect.y + gameObjectData->hive.rect.h/2) < 50.0 &&
							   	programmableWorker->status == RETURNING){
    	if(programmableWorker->cargo != 0){
    	  programmableWorker->cargo = 0;
    	  gameObjectData->hive.flowers_collected++;
    	  printf("We've now collected %d flowers!\n",gameObjectData->hive.flowers_collected);
    	}
  	}
  	else if(programmableWorker->status == LEAVING){
    	/* status being 1 means that the bee heading away from the center */
	
    	/* We want to get back the ResourceNode and ResourceNodeSpawner (if any)
    	   that we are colliding with */
    	resourceNode = checkResourceNodeCollision(&resourceNodeSpawner,gameObjectData,programmableWorker);
    	/* resourceNode will be NULL if there are no collisions, so test that */
    	if(resourceNode != NULL){
    	  /* Kill the ResourceNode */
    	  resourceNode->alive = 0;
    	  /* Make sure the ResourceNodeSpawner knows that it's lost a ResourceNode */
    	  resourceNodeSpawner->currentNodeCount--;
    	  programmableWorker->cargo++;
    	  programmableWorker->brain.foundNode = NULL;
    	}
    	else if(programmableWorker->brain.foundNode == NULL && resourceNodeSpawner != NULL){
    	  programmableWorker->brain.foundNode = chooseNodeRandomly(resourceNodeSpawner);
    	}
  	}
  }else{ /*programmable worker has been caught in rain and hasn't regained ability to fly yet*/
	int flapChance;
  	if(!(rand() % CHANCE_OF_REGAINING_FLIGHT)){
  		programmableWorker->wet_and_cant_fly = 0;
  	}
  	if((flapChance = rand() % 100)){
  		if(!(rand() % flapChance)){
  			programmableWorker->currentGraphicIndex = (programmableWorker->currentGraphicIndex + 1) % 2;
  		}
  	}
  }
}


void updateIceCreamPerson(GameObjectData *gameObjectData, int ticks){
  double newX,newY;
  int i;
  int distanceFromYBorder;
  int distanceFromXBorder;

  /*set iceCreamPerson to going home if sun has gone, or he has lost his ice cream*/
  if(!(gameObjectData->weather.present_weather == Sun &&
  gameObjectData->iceCreamPerson->has_ice_cream)){
  	gameObjectData->iceCreamPerson->going_home = 1;
  }
  /*set iceCreamPerson to running if it is raining*/
  if(gameObjectData->weather.present_weather == Rain){
    gameObjectData->iceCreamPerson->speed = 0.1;
  }else{
    gameObjectData->iceCreamPerson->speed = 0.05;
  }

  /*set iceCreamPerson->currently_on_screen to false if he has walked off screen*/
  if(gameObjectData->iceCreamPerson->xPosition > X_SIZE_OF_WORLD ||
  gameObjectData->iceCreamPerson->yPosition > Y_SIZE_OF_WORLD ||
  gameObjectData->iceCreamPerson->xPosition < 0 - PERSON_WIDTH ||
  gameObjectData->iceCreamPerson->yPosition < 0 - PERSON_HEIGHT){
    gameObjectData->iceCreamPerson->currently_on_screen = 0;
  }

  /*decrement countDownToStride*/
  gameObjectData->iceCreamPerson->countDownToStride--;

  /*if countDownToStride equals zero, reset count, and change stride image*/
  if(gameObjectData->iceCreamPerson->countDownToStride <= 0){
  	gameObjectData->iceCreamPerson->countDownToStride =
  	(double)STRIDE_FREQUENCY / gameObjectData->iceCreamPerson->speed;


  	switch(gameObjectData->iceCreamPerson->currentGraphicIndex){
  	  case 0:
  	  	gameObjectData->iceCreamPerson->currentGraphicIndex = WITH_ICE_CREAM_STRIDE2;
  	    break;
  	  case 1:
  	  	gameObjectData->iceCreamPerson->currentGraphicIndex = WITH_ICE_CREAM_STRIDE1;
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

  if(countProgrammableWorkersInRange(gameObjectData, getCenterOfRect(gameObjectData->iceCreamPerson->rect), 250.0) == 0 && !gameObjectData->iceCreamPerson->going_home){

  	if(gameObjectData->iceCreamPerson->xPosition >= X_SIZE_OF_WORLD - PERSON_WIDTH){
  	  /*world border has been reached and sun is still out, change direction*/
      gameObjectData->iceCreamPerson->heading = 3.142 + ((rand()%4)+ 0.069813);/*random facing away from border*/

  	}else if(gameObjectData->iceCreamPerson->xPosition <= 0){
  	  /*world border has been reached and sun is still out, change direction*/
      gameObjectData->iceCreamPerson->heading = ((rand()%4)+ 0.069813);/*random facing away from border*/

  	}else if(gameObjectData->iceCreamPerson->yPosition >= Y_SIZE_OF_WORLD - PERSON_HEIGHT){
  	  /*world border has been reached and sun is still out, change direction*/
      gameObjectData->iceCreamPerson->heading = 1.5708 + ((rand()%4)+ 0.069813);/*random facing away from border*/

  	}else if(gameObjectData->iceCreamPerson->yPosition <= 0){
  	  /*world border has been reached and sun is still out, change direction*/
      gameObjectData->iceCreamPerson->heading = 4.7124 + ((rand()%4)+ 0.069813);/*random facing away from border*/

    }else if(rand() % 1000 == 0){
      /*randomly change direction, just for the hell of it*/
   	  gameObjectData->iceCreamPerson->heading += ((double)(rand() % 30) / (double)10) - 1.5;
   	}
  }
  else{
    printf("forceing run away\n");
    distanceFromYBorder = gameObjectData->iceCreamPerson->yPosition - Y_SIZE_OF_WORLD/2;
    distanceFromXBorder = gameObjectData->iceCreamPerson->xPosition - X_SIZE_OF_WORLD/2;
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

ResourceNodeSpawner createResourceNodeSpawner(int maximumNodeCount, float xPosition, float yPosition, float radius){
  /* int maximumNodeCount = the maximum number of ResourceNodes for this spawner
                            to create
     float xPosition      = the x coordinates that the spawner will be created
                            at.
     float yPosition      = like above.
     float radius         = the radius around the ResourceNodeSpawner that we
                            want it to spawn ResourceNodes in

     This function will create a new ResourceNodeSpawner for us to use. Values
     here can be found in generic.h.
     Because we use a calloc here, we'll need to be sure to clean it up!*/
  ResourceNodeSpawner resourceNodeSpawner;
  int i = 0;

  resourceNodeSpawner.maximumNodeCount = maximumNodeCount;
  /* Set the currentNodeCount to 0 because no nodes have been spawned yet */
  resourceNodeSpawner.currentNodeCount = 0;

  resourceNodeSpawner.xPosition = xPosition;
  resourceNodeSpawner.yPosition = yPosition;

  /* Set the ticksSinceSpawn to 0 because we haven't spawned anything yet */
  resourceNodeSpawner.ticksSinceSpawn = 0;
  resourceNodeSpawner.spawnDelay = DEFAULT_SPAWNDELAY;
  resourceNodeSpawner.spawnRadius = radius;
  resourceNodeSpawner.collisionRect.x = (int)floor(xPosition - radius/2);
  resourceNodeSpawner.collisionRect.y = (int)floor(yPosition - radius/2);
  resourceNodeSpawner.collisionRect.w = (int)floor(radius);
  resourceNodeSpawner.collisionRect.h = (int)floor(radius);

  /* calloc up an array for us to use here */
  resourceNodeSpawner.resourceNodes = calloc((size_t)maximumNodeCount, sizeof(ResourceNode));
  while(i<maximumNodeCount){
    /* run through it and init the resourceNodes */
     initResourceNode(&resourceNodeSpawner.resourceNodes[i]);
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
  resourceNode->rect.x = 0;
  resourceNode->rect.y = 0;
  resourceNode->rect.w = X_SIZE_OF_NODE;
  resourceNode->rect.h = Y_SIZE_OF_NODE;
  resourceNode->deathTime = -1;
}

void updateResourceNodeSpawner(ResourceNodeSpawner *spawner, int ticks){
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
      resNode = createResourceNode(spawner,DEFAULT_RESOURCEUNITS);
      /* attach it to the spawner by inserting it into the resourceNodes array */
      spawner->resourceNodes[i] = resNode;
      /* Set the timer back to 0 */
      spawner->ticksSinceSpawn = 0;
      /* Update the currentNodeCount */
      spawner->currentNodeCount++;
    }
  }
}

void updateWeather(Weather *weather, int ticks){
  /* Advance weather every TICKSPERWEATHER ticks; this may be semi-random due to tick-skipping. */
    weather->tickCount += ticks;

    if(weather->tickCount > TICKSPERWEATHER){
    	weather->tickCount = 0;

    printf("weather changed\n");
      switch (weather->present_weather)
      {
        /* Closing the Window will exit the program */
        case Sun:
          weather->present_weather = (rand() % CHANCE_OF_CLOUD == 0) ? Cloud : Sun;
          break;
        case Cloud:
          weather->present_weather = (rand() % CHANCE_OF_RAIN == 0) ? Rain : Sun;
          break;
        case Rain:
          weather->present_weather = (rand() % CHANCE_OF_CLOUD == 0) ? Cloud : Sun;
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


void reInitialiseIceCreamPerson(IceCreamPerson *iceCreamPerson){
  iceCreamPerson->currently_on_screen = 1;

  iceCreamPerson->heading = (double)(randPi() * 2); /*set heading randomly*/

  /*random chance of appearing in top left or right, or bottom left or right corner of world*/
  iceCreamPerson->xPosition = (rand()%2) ? X_SIZE_OF_WORLD - PERSON_WIDTH : 0;
  iceCreamPerson->yPosition = (rand()%2) ? Y_SIZE_OF_WORLD - PERSON_HEIGHT : 0;
  iceCreamPerson->rect.x = iceCreamPerson->xPosition;
  iceCreamPerson->rect.y = iceCreamPerson->yPosition;

  iceCreamPerson->has_ice_cream = 1;
  iceCreamPerson->going_home = 0;
  iceCreamPerson->speed = 0.05; /*pixels per millisecond*/
  iceCreamPerson->stung_count = 0;

  iceCreamPerson->countDownToStride = (double)STRIDE_FREQUENCY / iceCreamPerson->speed;

  iceCreamPerson->currentGraphicIndex = 0;


  iceCreamPerson->strings_until_ice_cream_drop = (rand() % 5) + 1;
}



ResourceNode createResourceNode(ResourceNodeSpawner *parentSpawner, int resourceUnits){
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
  fitRectToWorld(&resourceNode.rect);
  resourceNode.rect.w = 20;
  resourceNode.rect.h = 20;
  return resourceNode;
}



void updateGameObjects(GameObjectData *gameObjectData, GraphicsData *graphicsData, int ticks){
  /* GameObjectData *gameObjectData = the pointer to the GameObjectData struct
                                      that holds all the information about our
                                      GameObjects.
     GraphicsData *graphicsData     = the pointer to the GraphicsData struct
                                      that holds the information we need to draw
                                      things on the screen.
     float ticks                    = the number of milliseconds that we need to
                                      run the updates for this FRAME_RATE

     This function makes all the GameObjects move around on the screen and show
     up graphically. It gets called every frame from the main gameLoop function
     in game.c.*/
  int i = 0, j = 0;
  ProgrammableWorker *programmableWorker;
  blitTiledBackground(graphicsData, graphicsData->grassTexture);

  /* First, we need to draw the Hive in at the correct position. */
  blitGameObject(gameObjectData->hive.rect,
                 graphicsData,
                 graphicsData->hiveTexture,
                 0,
                 NULL,
                 SDL_FLIP_NONE);




  /* Second, we loop through all the ResourceNodeSpawners */
  while(i < gameObjectData->resourceNodeSpawnerCount){
    j = 0;

    if(!gameObjectData->pause_status){
       /* Update each one to reflect respawning nodes and so on. */
       updateResourceNodeSpawner(&gameObjectData->resourceNodeSpawners[i],ticks);
    }
    /* Then we need to loop through the attached ResourceNodes and draw them */
   while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
      if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive){
        blitGameObject(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect,
                       graphicsData,
                       graphicsData->nodeTexture,
                       0,
                       NULL,
                       SDL_FLIP_NONE);
      }
      j++;
    }
    i++;
  }

	/*BEES ON GROUND NEED RENDERING BEFORE PERSON*/
  for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL ; programmableWorker = programmableWorker->next){
  	SDL_Rect smallerBeeRect;
  	smallerBeeRect.w = (int)((float)programmableWorker->rect.w/BEE_SHRINK_FACTOR_ON_GROUND);
  	smallerBeeRect.h = (int)((float)programmableWorker->rect.h/BEE_SHRINK_FACTOR_ON_GROUND);
  	smallerBeeRect.x = programmableWorker->rect.x;
  	smallerBeeRect.y = programmableWorker->rect.y;
  	
  	if(programmableWorker->wet_and_cant_fly){
    	if(!gameObjectData->pause_status){
    	  updateProgrammableWorker(programmableWorker,gameObjectData,ticks);
    	}
	
    	blitGameObject(smallerBeeRect,
                   	graphicsData,
                   	graphicsData->bee->graphic[programmableWorker->currentGraphicIndex],
                   	DEGREESINCIRCLE-(programmableWorker->heading * RADIANSTODEGREES),
                   	NULL,
                   	SDL_FLIP_NONE);

    	i++;
    }
  }
  	
   /*determine if iceCreamPerson is on screen and needs animating*/
  if(gameObjectData->iceCreamPerson->currently_on_screen){

   	if(!gameObjectData->pause_status){
       	updateIceCreamPerson(gameObjectData, ticks);
   	}

   	blitGameObject(gameObjectData->iceCreamPerson->rect,
                  	graphicsData,
                   	graphicsData->person->graphic[gameObjectData->iceCreamPerson->currentGraphicIndex],
                   	DEGREESINCIRCLE-(gameObjectData->iceCreamPerson->heading * RADIANSTODEGREES),
                   	NULL,
                   	SDL_FLIP_NONE);

  }else{ /*small probability of re-initialising iceCreamPerson and setting location to on-screen*/
   	if((gameObjectData->weather.present_weather == Sun) &&
 	(rand() % ICE_CREAM_PERSON_PROB == 0)){

   		reInitialiseIceCreamPerson(gameObjectData->iceCreamPerson);

   }
 }



  /* Thirdly, we loop through all the ProgrammableWorkers and update them */
  /* AI thinking has been moved to a seperate function to prevent some circular
     inheritance issues. */
  /* Also, remember the important rule - AI should tell workers to do things rather
     than directly doing things itself! */

  for(programmableWorker = gameObjectData->first_programmable_worker; programmableWorker != NULL ; programmableWorker = programmableWorker->next){
  	if(!programmableWorker->wet_and_cant_fly){
    	if(!gameObjectData->pause_status){
    	  updateProgrammableWorker(programmableWorker,gameObjectData,ticks);
    	}

    	blitGameObject(programmableWorker->rect,
                   	graphicsData,
                   	graphicsData->bee->graphic[programmableWorker->currentGraphicIndex],
                   	DEGREESINCIRCLE-(programmableWorker->heading * RADIANSTODEGREES),
                   	NULL,
                   	SDL_FLIP_NONE);

    	i++;
    }
  }
  

  
  

  /* render tree tops last, so that they appear above everything else*/
  for(i = 0; i < NUMBER_OF_TREES; i++){

     blitParallaxTreeTops(gameObjectData->tree[i].rect,
                    graphicsData,
                    graphicsData->treeTexture);
  }

  /*finally render a layer or rain splatter if its raining*/
  if(gameObjectData->weather.present_weather == Rain){
    blitRainRandomly(graphicsData);
  }

  updateWeather(&gameObjectData->weather, ticks);

  paintWeatherLayer(graphicsData, gameObjectData->weather.present_weather);

}
