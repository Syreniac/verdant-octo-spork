#include "audio.h"

enum ProgrammableWorkerStatus {LEAVING = 1,
                               RETURNING = 2,
                               WANTING_TO_RETURN = 3,
                               NODE_FOUND = 4,
							   IDLE = 5,
                               BLANK = -1};

typedef struct ProgrammableWorker ProgrammableWorker;
typedef struct ResourceNode ResourceNode;
typedef struct ResourceNodeSpawner ResourceNodeSpawner;
typedef struct ProgrammableWorkerBrain ProgrammableWorkerBrain;
typedef struct Hive Hive;
typedef struct Tree Tree;
typedef struct IceCreamPerson IceCreamPerson;
typedef struct Weather Weather;
typedef struct GameObjectData GameObjectData;

/* I'm calling them resource nodes rather than flowers because it makes the
   game more easily adaptable to other themes if we want. Otherwise they are
   identical. */

struct ResourceNode{
  /* this is a 1/0 boolean to see whether the flower is alive or dead. */
  int alive;
  /* How many resources (e.g. Honey) are in a node*/
  int resourceUnits;
  /* standard positioning data - x,y coords */
  SDL_Rect rect;
};

/*   Rather than having an array with just resourceNodes in, if we have a
     hierarchical layout based around spawners, we can do some wizardry to make
     the collision detection system and respawn system more effecient */

struct ResourceNodeSpawner{
  /* These two values will control how many nodes the spawner has and will
     create */
  int maximumNodeCount;
  int currentNodeCount;
  /* fairly standard positioning data, just x,y coords */
  float xPosition;
  float yPosition;
  /* make a note of the time when the spawner was last updated. */
  int ticksSinceSpawn;
  int spawnDelay;
  float spawnRadius;
  /* rather than specify a hard array, we'll calloc something up to go here
     later */
  ResourceNode *resourceNodes;
  SDL_Rect collisionRect;
};

struct ProgrammableWorkerBrain{
  SDL_Point remembered_point;
  ResourceNode *foundNode;
  int is_point_remembered;
  ProgrammableWorker *followTarget;
};

struct ProgrammableWorker{
  SDL_Rect rect;

  double rawX;
  double rawY;

  double heading;
  float speed;
  
  int currentGraphicIndex;
  
  int wet_and_cant_fly;


  int cargo;

  int type;
  enum ProgrammableWorkerStatus status;
  ProgrammableWorker *next;
  ProgrammableWorkerBrain brain;
};

struct Hive{
  float xPosition;
  float yPosition;
  SDL_Rect rect;
  int flowers_collected;
};

struct Tree{
  float xPosition;
  float yPosition;
  SDL_Rect rect;
  int bees_taking_shelter;
};

struct IceCreamPerson{
  float xPosition;
  float yPosition;
  SDL_Rect rect;

  double heading;
  double speed;

  int countDownToStride;
  int currentGraphicIndex;

  int going_home;

  int has_ice_cream;
  int currently_on_screen;
  int stung_count;
  int strings_until_ice_cream_drop;
};

struct Weather{
  enum WeatherStatus present_weather;
  int tickCount;
};

struct GameObjectData{
  Weather weather;
  Hive hive;
  Tree tree[NUMBER_OF_TREES];
  ResourceNodeSpawner resourceNodeSpawners[NUMBER_OF_FLOWER_PATCHES];
  IceCreamPerson *iceCreamPerson;
  int resourceNodeSpawnerCount;
  ProgrammableWorker *first_programmable_worker;
  int programmableWorkerCount;
  int pause_status;
  
  AudioData audioData;
};

ProgrammableWorker *createProgrammableWorker(GameObjectData *gameObjectData);
void updateProgrammableWorker(ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks);
void updateIceCreamPerson(GameObjectData *gameObjectData, int ticks);

Hive createHive(void);
Tree createTree(void);
Weather createWeatherLayer(void);

/* I think that it will be better to do static respawning objects through
   creating spawners rather than directly into the world.

   Doing it this way, we can use the created hierarchy to cull collision
   checking quickly and respawning resources will be easier */

int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner);
ResourceNodeSpawner createResourceNodeSpawner(int maximumNodeCount, float xPosition, float yPosition, float radius);
void updateResourceNodeSpawner(ResourceNodeSpawner *spawner, int ticks);
void updateWeather(GameObjectData *gameObjectData, Weather *weather, int ticks);

void initResourceNode(ResourceNode *resourceNode);
ResourceNode createResourceNode(ResourceNodeSpawner *parentSpawner, int resourceUnits);
IceCreamPerson *createIceCreamPerson(void);
void reInitialiseIceCreamPerson(IceCreamPerson *iceCreamPerson);
void updateGameObjects(GameObjectData *gameObjectData, GraphicsData *graphicsData, int ticks);
int countProgrammableWorkersInRange(GameObjectData *gameObjectData, SDL_Point center, double radius);

void initAudio(GameObjectData *gameObjectData, AudioData audioData);
