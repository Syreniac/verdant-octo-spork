#include "generic.h"

/* By putting the game object structs in here, we can more easily include them
   in other modules without having to copy-paste code BUT we'll need to put
   a check in to make sure this file doesn't get include multiple times.*/

#define _GAME_OBJECTS_INCLUDED 1

/* I'm calling them resource nodes rather than flowers because it makes the
   game more easily adaptable to other themes if we want. Otherwise they are
   identical. */

typedef struct ResourceNode ResourceNode;
struct ResourceNode{
  /* this is a 1/0 boolean to see whether the flower is alive or dead. */
  int alive;
  /* How many resources (e.g. Honey) are in a node*/
  int resourceUnits;
  /* standard positioning data - x,y coords */
  float xPosition;
  float yPosition;
  /* This is how we know when to respawn it once it dies. The value stored here
     will be the SDL_GetTicks() at the time of the flowers death. */
  int deathTime;
};

/*   Rather than having an array with just resourceNodes in, if we have a
     hierarchical layout based around spawners, we can do some wizardry to make
     the collision detection system and respawn system more effecient */

typedef struct ResourceNodeSpawner ResourceNodeSpawner;
struct ResourceNodeSpawner{
  /* These two values will control how many nodes the spawner has and will
     create */
  int maximumNodeCount;
  int currentNodeCount;
  /* fairly standard positioning data, just x,y coords */
  float xPosition;
  float yPosition;
  /* make a note of the time when the spawner was last updated. */
  float ticksSinceSpawn;
  int spawnDelay;
  float spawnRadius;
  /* rather than specify a hard array, we'll calloc something up to go here
     later */
  ResourceNode *resourceNodes;
};

typedef struct ProgrammableWorker ProgrammableWorker;
struct ProgrammableWorker{
  float xPosition;
  float yPosition;

  float xDimensions;
  float yDimensions;

  float heading;
  float speed;

  int type;
  int status;
};

typedef struct GameObjectData GameObjectData;
struct GameObjectData{
  ResourceNodeSpawner resourceNodeSpawners[5];
  int resourceNodeSpawnerCount;
  ProgrammableWorker programmableWorkers[1];
  int programmableWorkerCount;
};

int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner);
ProgrammableWorker createProgrammableWorker(void);
void updateProgrammableWorker(ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, float ticks);
