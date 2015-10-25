#include "game_objects.h"

double getDistance2BetweenPoints(float p1X, float p1Y, float p2X, float p2Y){
  return ((p1X - p2X) * (p1X - p2X)) + ((p1Y - p2Y) * (p1Y - p2Y));
}

float randPi(void){
  return 3.14159 * (float) rand() / (float) RAND_MAX;
}

float square(float f){
  return f*f;
}

ResourceNode *checkResourceNodeCollision(ResourceNodeSpawner **resourceNodeSpawnerPointer, GameObjectData *gameObjectData, float positionX, float positionY){
  int i = 0, j = 0;
  double d2;
  ResourceNodeSpawner resourceNodeSpawner;
  ResourceNode resourceNode;
  while(i < gameObjectData->resourceNodeSpawnerCount){
    j = 0;
    resourceNodeSpawner = gameObjectData->resourceNodeSpawners[i];
    d2=getDistance2BetweenPoints(positionX,positionY,resourceNodeSpawner.xPosition,resourceNodeSpawner.yPosition);
    if(d2 < square(resourceNodeSpawner.spawnRadius+40)){
      /* This means that we were close enough to a resourceNodeSpawner that we
         should check against the individual nodes*/
      while(j < resourceNodeSpawner.maximumNodeCount){
        resourceNode = resourceNodeSpawner.resourceNodes[j];
        d2 = getDistance2BetweenPoints(positionX,positionY,resourceNode.xPosition,resourceNode.yPosition);
        if(d2 < square(20) && resourceNode.alive){
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
     ResourceNodeSpawner */
  int i = 0;
  while(i<resourceNodeSpawner->maximumNodeCount){
    if(resourceNodeSpawner->resourceNodes[i].alive == 0){
      return(i);
    }
    i++;
  }
  /* We'll return -1 if we don't find one, but this shouldn't happen */
  return(-1);
}

ProgrammableWorker createProgrammableWorker(void){
  ProgrammableWorker programmableWorker;
  programmableWorker.xPosition = 200.0;
  programmableWorker.yPosition = 200.0;
  programmableWorker.xDimensions = 40.0;
  programmableWorker.yDimensions = 40.0;
  programmableWorker.heading = 0.523599;
  programmableWorker.speed = 0.5;
  programmableWorker.type = 1;
  programmableWorker.status = 1;
  return(programmableWorker);
}

void updateProgrammableWorker(ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, float ticks){
  double newX,newY,diffX,diffY;
  ResourceNodeSpawner *resourceNodeSpawner;
  ResourceNode *resourceNode;

  newX = sin(programmableWorker->heading) * programmableWorker->speed * ticks;
  newY = cos(programmableWorker->heading) * programmableWorker->speed * ticks;

  newX = programmableWorker->xPosition + newX;
  newY = programmableWorker->yPosition + newY;

  if(getDistance2BetweenPoints(newX,newY,320,240) < 0.1 && programmableWorker->status == 2){
    programmableWorker->heading = randPi() * 2;
    programmableWorker->status = 1;
  }
  if(programmableWorker->status == 1){
    /* THIS CODE IS LITERALLY HORRIBLE */
    /* DO NOT EVEN THINK ABOUT TRYING TO UNDERSTAND EXACTLY WHAT IS HAPPENING HERE */
    resourceNode = checkResourceNodeCollision(&resourceNodeSpawner,gameObjectData,newX,newY);
    if(resourceNode != NULL){
      resourceNode->alive = 0;
      resourceNodeSpawner->currentNodeCount--;
      printf(">>>>it ate a flower!<<<< %d\n",resourceNodeSpawner->currentNodeCount);
      programmableWorker->status = 3;
    }
  }
  if(newX >= (640 - programmableWorker->xDimensions) || newX < 0 || newY >= (480 - programmableWorker->yDimensions) || newY < 0 || programmableWorker->status == 3){
    programmableWorker->heading = atan2(320 - programmableWorker->xPosition,
                                        240 - programmableWorker->yPosition);
    newX = sin(programmableWorker->heading) * programmableWorker->speed * ticks;
    newY = cos(programmableWorker->heading) * programmableWorker->speed * ticks;

    newX += programmableWorker->xPosition + newX;
    newY += programmableWorker->yPosition + newY;
    programmableWorker->status = 2;
  }

  programmableWorker->xPosition = newX;
  programmableWorker->yPosition = newY;
}
