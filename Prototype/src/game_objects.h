#include "generic.h"

enum ProgrammableWorkerStatus {LEAVING = 1,
                               RETURNING = 2,
                               WANTING_TO_RETURN = 3,
                               BLANK = -1};

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
  SDL_Rect rect;
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
  SDL_Rect rect;

  double rawX;
  double rawY;

  double heading;
  float speed;

  int cargo;

  int type;
  enum ProgrammableWorkerStatus status;
};

typedef struct Hive Hive;
struct Hive{
  float xPosition;
  float yPosition;
  SDL_Rect rect;
};

typedef struct GameObjectData GameObjectData;
struct GameObjectData{
  Hive hive;
  ResourceNodeSpawner resourceNodeSpawners[5];
  int resourceNodeSpawnerCount;
  ProgrammableWorker programmableWorkers[2];
  int programmableWorkerCount;
  int pause_status;
};

float generateRandomCoordOffset(float radius);
float randPi(void);
float square(float f);
double getDistance2BetweenPoints(float p1X, float p1Y, float p2X, float p2Y);

ProgrammableWorker createProgrammableWorker(void);
void updateProgrammableWorker(ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, float ticks);

Hive createHive(void);

/* I think that it will be better to do static respawning objects through
   creating spawners rather than directly into the world.

   Doing it this way, we can use the created hierarchy to cull collision
   checking quickly and respawning resources will be easier */

int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner);
ResourceNodeSpawner createResourceNodeSpawner(int maximumNodeCount, float xPosition, float yPosition, float radius);
void updateResourceNodeSpawner(ResourceNodeSpawner *spawner, float ticks);

void initResourceNode(ResourceNode *resourceNode);
ResourceNode createResourceNode(ResourceNodeSpawner *parentSpawner, int resourceUnits);
void updateGameObjects(GameObjectData *gameObjectData, GraphicsData *graphicsData, float ticks);
