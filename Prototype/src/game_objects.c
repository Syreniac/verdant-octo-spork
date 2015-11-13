#include "game_objects.h"

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
          *resourceNodeSpawnerPointer = &gameObjectData->resourceNodeSpawners[i];
          return(&gameObjectData->resourceNodeSpawners[i].resourceNodes[j]);
        }
        j++;
      }
    }
    i++;
  }
  return(NULL);
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

ProgrammableWorker createProgrammableWorker(void){
  /* This function creates a ProgrammableWorker struct and fills in the default
     values. Many of these are defined in generic.h */
  ProgrammableWorker programmableWorker;
  /* Because we're using trigonometry to move things around, if work purely with
     integers then Bad Things happen. Instead, we work first in doubles then
     copy the information to the integers in the rectangle. */
  programmableWorker.rawX = 100.0;
  programmableWorker.rawY = 100.0;
  programmableWorker.rect.x = 100;
  programmableWorker.rect.y = 100;
  programmableWorker.rect.w = X_SIZE_OF_WORKER;
  programmableWorker.rect.h = Y_SIZE_OF_WORKER;
  /* heading is measured in radians because maths in C all take radians */
  programmableWorker.heading = 0.0;
  /* This is basically pixels/milliseconds */
  programmableWorker.speed = 0.1;
  /* I don't think this is used yet... */
  programmableWorker.type = 1;
  programmableWorker.cargo = 0;
  /* This is an enum detailed in game_objects.h */
  programmableWorker.status = LEAVING;
  return(programmableWorker);
}

Hive createHive(void){
  /* This function creates a Hive struct and fills in the default
     values. Many of these are defined in generic.h */
  Hive hive;
  hive.rect.w = 64;
  hive.rect.h = 80;
  hive.rect.x = X_SIZE_OF_WORLD/2 - hive.rect.w/2;
  hive.rect.y = Y_SIZE_OF_WORLD/2 - hive.rect.h/2;
  return(hive);
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
  ResourceNodeSpawner *resourceNodeSpawner;
  ResourceNode *resourceNode;

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
  programmableWorker->rect.x = floor(programmableWorker->rawX);
  programmableWorker->rect.y = floor(programmableWorker->rawY);

  if(getDistance2BetweenPoints(programmableWorker->rect.x + programmableWorker->rect.w/2,
							   programmableWorker->rect.y + programmableWorker->rect.h/2,
							   gameObjectData->hive.rect.x + gameObjectData->hive.rect.w/2,
							   gameObjectData->hive.rect.y + gameObjectData->hive.rect.h/2) < 10.0 && programmableWorker->status == RETURNING){
								   printf("dropped off cargo\n");
    programmableWorker->cargo = 0;
  }
  if(programmableWorker->status == LEAVING){
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
  resourceNodeSpawner.ticksSinceSpawn = 0.0;
  resourceNodeSpawner.spawnDelay = DEFAULT_SPAWNDELAY;
  resourceNodeSpawner.spawnRadius = radius;
  
  resourceNodeSpawner.collisionRect.x = xPosition - radius/2;
  resourceNodeSpawner.collisionRect.y = yPosition - radius/2;
  resourceNodeSpawner.collisionRect.w = radius;
  resourceNodeSpawner.collisionRect.h = radius;

  /* calloc up an array for us to use here */
  resourceNodeSpawner.resourceNodes = calloc(maximumNodeCount, sizeof(ResourceNode));
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
  resourceNode.rect.x = parentSpawner->xPosition + generateRandomCoordOffset(parentSpawner->spawnRadius) - X_SIZE_OF_NODE/2;
  resourceNode.rect.y = parentSpawner->yPosition + generateRandomCoordOffset(parentSpawner->spawnRadius) - Y_SIZE_OF_NODE/2;
  resourceNode.rect.w = 20;
  resourceNode.rect.h = 20;
  return resourceNode;
}

void updateGameObjects(GameObjectData *gameObjectData, GraphicsData *graphicsData, float ticks){
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

  /* First, we need to draw the Hive in at the correct position. */
  blitGameObject(gameObjectData->hive.rect,
                 graphicsData,
                 graphicsData->hiveTexture);
        
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
                       graphicsData->nodeTexture);
      }
      j++;
    }
    i++;
  }
  /* Thirdly, we loop through all the ProgrammableWorkers and update them */
  /* AI thinking has been moved to a seperate function to prevent some circular
     inheritance issues. */
  /* Also, remember the important rule - AI should tell workers to do things rather
     than directly doing things itself! */
     
  i = 0;
  while(i < gameObjectData->programmableWorkerCount){
    if(!gameObjectData->pause_status){
      updateProgrammableWorker(&gameObjectData->programmableWorkers[i],gameObjectData,ticks);
    }
    blitGameObject(gameObjectData->programmableWorkers[i].rect,
                   graphicsData,
                   graphicsData->workerTexture);

    i++;
  }
}
