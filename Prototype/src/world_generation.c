#include "world_generation.h"

float generateRandomCoordOffset(float radius){
  /* float radius = the range to generate random numbers within

     This function just generates a random x or y offset for a coordinate within
     a given range */
  float return_value = (radius * ( (float) rand() - (float) RAND_MAX * 0.5 )/(float) RAND_MAX );
  return(return_value);
}

ResourceNodeSpawner createResourceNodeSpawner(int maximumNodeCount, float xPosition, float yPosition, float radius){
  /* int maximumNodeCount = the maximum number of ResourceNodes for this spawner
                            to create
     float xPosition = the x coordinates that the spawner will be created at.
     float yPosition = the y coordinates that the spawner will be created at.

     This function will create a new ResourceNodeSpawner for us to use.
     Because we use a calloc here, we'll need to be sure to clean it up!*/
  ResourceNodeSpawner resourceNodeSpawner;
  int i = 0;

  resourceNodeSpawner.maximumNodeCount = maximumNodeCount;
  resourceNodeSpawner.currentNodeCount = 0;

  resourceNodeSpawner.xPosition = xPosition;
  resourceNodeSpawner.yPosition = yPosition;

  resourceNodeSpawner.ticksSinceSpawn = 0.0;
  resourceNodeSpawner.spawnDelay = 3000;
  resourceNodeSpawner.spawnRadius = radius;

  resourceNodeSpawner.resourceNodes = calloc(maximumNodeCount, sizeof(ResourceNode));
  while(i<maximumNodeCount){
    resourceNodeSpawner.resourceNodes[i] = initResourceNode(&resourceNodeSpawner);
    i++;
  }
  return(resourceNodeSpawner);
}

ResourceNode initResourceNode(ResourceNodeSpawner *spawner){
  ResourceNode resourceNode;
  resourceNode.alive = 0;
  resourceNode.resourceUnits = 0;
  resourceNode.xPosition = 0;
  resourceNode.yPosition = 0;
  resourceNode.deathTime = -1;
  return(resourceNode);
}

void updateResourceNodeSpawner(ResourceNodeSpawner *spawner, float ticks){
  /* ResourceNodeSpawner *spawner = the spawner that we are updating
     float ticks = the number of ticks we are updating over.

     This function will just update a ResourceNodeSpawner as part of the main
     game loop. */
  int i = 0;
  ResourceNode resNode;
  if(spawner->currentNodeCount < spawner->maximumNodeCount){
    printf("ticksSinceSpawn: %f | spawnDelay: %f | ticks: %f\n", spawner->ticksSinceSpawn, spawner->spawnDelay, ticks);
    printf("currentNodeCount: %d | maxNodeCount %d\n", spawner->currentNodeCount, spawner->maximumNodeCount);
    /* If the number of nodes is less than the desired maximum, then we will
       want to do some spawning. Maybe. We should check the timer first */
    spawner->ticksSinceSpawn += ticks;
    if(spawner->ticksSinceSpawn > spawner->spawnDelay){
      printf("spawning new ResourceNode\n");
      i = getFirstDeadResourceNode(spawner);
      resNode = createResourceNode(spawner,100);
      spawner->resourceNodes[i] = resNode;
      spawner->ticksSinceSpawn = 0;
      printf("%f\n",spawner->ticksSinceSpawn);
      spawner->currentNodeCount++;
    }
  }
}

ResourceNode createResourceNode(ResourceNodeSpawner *parentSpawner, int resourceUnits){
  /* ResourceNodeSpawner *parentSpawner = the spawner which this resource node
                                          is attached to
     int resourceUnits = the amount of resources to put into this node*/
  ResourceNode resourceNode;
  resourceNode.alive = 1;
  resourceNode.resourceUnits = resourceUnits;
  resourceNode.xPosition = parentSpawner->xPosition + generateRandomCoordOffset(parentSpawner->spawnRadius);
  resourceNode.yPosition = parentSpawner->yPosition + generateRandomCoordOffset(parentSpawner->spawnRadius);
  resourceNode.deathTime = -1;
  return resourceNode;
}
