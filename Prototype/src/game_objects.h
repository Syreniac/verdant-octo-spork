#include "announcements.h"

enum ProgrammableWorkerStatus {LEAVING = 1,
                               RETURNING = 2,
                               WANTING_TO_RETURN = 3,
                               NODE_FOUND = 4,
							   IDLE = 5,
                               BLANK = -1};
enum GameOverCause {STARVATION,
	COLD};
typedef enum GameOverCause GameOverCause;

typedef struct ProgrammableWorker ProgrammableWorker;
typedef struct ResourceNode ResourceNode;
typedef struct ResourceNodeSpawner ResourceNodeSpawner;
typedef struct ProgrammableWorkerBrain ProgrammableWorkerBrain;
typedef struct Hive Hive;
typedef struct Tree Tree;
typedef struct IceCreamPerson IceCreamPerson;
typedef struct RoamingSpider RoamingSpider;
typedef struct Weather Weather;
typedef struct GameObjectData GameObjectData;
typedef struct DroppedIceCream DroppedIceCream;
typedef struct HiveCell HiveCell;

/* I'm calling them resource nodes rather than flowers because it makes the
   game more easily adaptable to other themes if we want. Otherwise they are
   identical. */

struct ResourceNode{
  /* this is a 1/0 boolean to see whether the flower is alive or dead. */
  int alive;
  /* How many resources (e.g. Honey) are in a node*/
  int resourceUnits;
  /* standard positioning data - x,y coords */
  int displayInfo;
  SDL_Rect rect;
  int type;
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
  int aiStartPoint;
  int waitTime;
  int storedCount;
};

struct ProgrammableWorker{
  SDL_Rect rect;

  double rawX;
  double rawY;

  double heading;
  float speed;

  int currentGraphicIndex;

  int wet_and_cant_fly;
  int cold_and_about_to_die;
  int stunned_after_sting;
  int fighting_spider;

  int flapTimer;

  char *beeStatus;

  int insideHive;

  int cargo;

  int displayInfo;
  int xRenderPosWhenSelected;
  int yRenderPosWhenSelected;

  int type;
  enum ProgrammableWorkerStatus status;
  ProgrammableWorker *next;
  ProgrammableWorkerBrain brain;
};

struct HiveCell{
  int timer;
};

struct Hive{
  float xPosition;
  float yPosition;
  SDL_Rect rect;
  int displayInfo;
  int flowers_collected;
  int bees_taking_shelter;
  int scoreBeforeWinter;
  int delayBeforeSummer;
  int winterCountdown;
  float winterCountdownFloat;
  int years_survived;
  HiveCell hiveCells[NUMBER_OF_CELLS_IN_HIVE];
};

struct Tree{
  float xPosition;
  float yPosition;
  SDL_Rect rect;
  SDL_Rect stumpRect;
  int bees_taking_shelter;
  int displayInfo;
  int currentGraphicIndex;
};

struct DroppedIceCream{
	float xPosition;
	float yPosition;
	SDL_Rect rect;
	int dropped;
	int droppedTimer;
	int sizeOscillator;
	int displayInfo;
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
  int stung;
  int displayInfo;
};

struct RoamingSpider{
  float xPosition;
  float yPosition;
  SDL_Rect rect;

  double heading;
  double speed;

  int countDownToStride;
  int currentGraphicIndex;

  int going_home;

  int eating_bee;
  int currently_on_screen;
  int stung;
  int displayInfo;
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
  DroppedIceCream *droppedIceCream;
  RoamingSpider *roamingSpider;
  int resourceNodeSpawnerCount;
  ProgrammableWorker *first_programmable_worker;
  int programmableWorkerCount;
  int pause_status;
  char announcement[256];
  char gameOverString[256];
  int gameOver;
  int gameOverBoxVisible;
  GameOverCause gameOverCause;
  Uint32 gameOverEventNum;
  Uint32 objectDisplayEventNum;
  int gameRestart;
  AudioData audioData;
};

void killAllBees(ProgrammableWorker **programmableWorker);
void killProgrammableWorker(GameObjectData *gameObjectData, ProgrammableWorker **programmableWorker);
ProgrammableWorker *createProgrammableWorker(GameObjectData *gameObjectData);
void updateProgrammableWorker(ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks);
void updateIceCreamPerson(GameObjectData *gameObjectData, int ticks);

Hive createHive(void);
Tree createTree(Hive *hive, int forceX, int forceY);
Weather createWeatherLayer(void);

/* I think that it will be better to do static respawning objects through
   creating spawners rather than directly into the world.

   Doing it this way, we can use the created hierarchy to cull collision
   checking quickly and respawning resources will be easier */

int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner);
ResourceNode createResourceNode(GameObjectData *gameObjectData, ResourceNodeSpawner *parentSpawner, int resourceUnits);
ResourceNodeSpawner createResourceNodeSpawner(GameObjectData *gameObjectData, int maximumNodeCount, float xPosition, float yPosition, float radius);
void updateResourceNodeSpawner(GameObjectData *gameObjectData, ResourceNodeSpawner *spawner, int ticks);
void updateWeather(GameObjectData *gameObjectData, AudioData *audioData, Weather *weather, int ticks);

void initResourceNode(ResourceNode *resourceNode);
ResourceNode createResourceNode(GameObjectData *gameObjectData, ResourceNodeSpawner *parentSpawner, int resourceUnits);
IceCreamPerson *createIceCreamPerson(void);
DroppedIceCream *createDroppedIceCream(void);
RoamingSpider *createRoamingSpider(void);
void reInitialiseIceCreamPerson(IceCreamPerson *iceCreamPerson);
void reInitialiseRoamingSpider(RoamingSpider *roamingSpider);
void updateGameObjects(GameObjectData *gameObjectData, AudioData *audioData, GraphicsData *graphicsData, AnnouncementsData *announcementsData, int ticks);
void updateHiveCell(GameObjectData *gameObjectData, AnnouncementsData *announcementsData, HiveCell *hiveCell, int ticks);
void updateHive(GameObjectData *gameObjectData, AnnouncementsData *announcementsData,  int ticks);
int countProgrammableWorkersInRange(GameObjectData *gameObjectData, SDL_Point center, double radius);
int isPointInRangeOf(SDL_Point point, SDL_Point center, double radius);

void initAudio(GameObjectData *gameObjectData, AudioData audioData);
void centerViewOnHive(GraphicsData *graphicsData, GameObjectData *gameObjectData);
void objectInfoDisplay(GameObjectData *gameObjectData, GraphicsData *graphicsData, AnnouncementsData *announcementsData,
SDL_MouseButtonEvent *mbEvent);

void nullifyLocalAIInformation(GameObjectData *gameObjectData);
int countResourceNodesInRadius(GameObjectData *gameObjectData, int x, int y, double radius);
