#include "announcements.h"

enum ProgrammableWorkerStatus {LEAVING = 1,
                               RETURNING = 2,
                               WANTING_TO_RETURN = 3,
                               NODE_FOUND = 4,
							   IDLE = 5,
                               BLANK = -1};
enum GameOverCause {STARVATION,
	COLD};

enum GameObjectSelectionType{GOS_TREE,
                             GOS_HIVE,
                             GOS_RESOURCE_NODE,
                             GOS_PROGRAMMABLE_WORKER,
                             GOS_ICE_CREAM_PERSON,
                             GOS_ROAMING_SPIDER,
                             GOS_DROPPED_ICE_CREAM,
                             GOS_NONE};

enum Season{SPRING,SUMMER,AUTUMN,WINTER};

typedef enum GameObjectSelectionType GameObjectSelectionType;
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
typedef struct GameObjectSelection GameObjectSelection;
typedef struct Environment Environment;

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
  double xPosition;
  double yPosition;
  /* make a note of the time when the spawner was last updated. */
  int ticksSinceSpawn;
  int spawnDelay;
  double spawnRadius;
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
  int nearestWorkerCacheTime;
  ProgrammableWorker *nearestWorker;
};

struct ProgrammableWorker{
  SDL_Rect rect;

  double rawX;
  double rawY;

  double heading;
  double speed;

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

  int weatherproofTimer;

  int type;
  enum ProgrammableWorkerStatus status;
  ProgrammableWorker *next;
  ProgrammableWorkerBrain brain;
};

struct HiveCell{
  int timer;
};

struct Hive{
  double xPosition;
  double yPosition;
  SDL_Rect rect;
  int flowers_collected;
  int bees_taking_shelter;
  HiveCell hiveCells[NUMBER_OF_CELLS_IN_HIVE];
};

struct Tree{
  double xPosition;
  double yPosition;
  SDL_Rect rect;
  SDL_Rect stumpRect;
  int bees_taking_shelter;
  int displayInfo;
  int currentGraphicIndex;
};

struct DroppedIceCream{
	double xPosition;
	double yPosition;
	SDL_Rect rect;
	int dropped;
	int droppedTimer;
	int sizeOscillator;
	int displayInfo;
};

struct IceCreamPerson{
  double xPosition;
  double yPosition;
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
  double xPosition;
  double yPosition;
  SDL_Rect rect;

  double heading;
  double speed;

  double countDownToStride;
  int currentGraphicIndex;

  int going_home;

  int ticksSinceEating;
  int ticksSinceStung;
  int ticksSinceDead;

  int deadSpider;
  int eating_bee;
  int eating_bee_complete;
  int currently_on_screen;
  int stung;
  int displayInfo;
};

struct Weather{
  enum WeatherStatus present_weather;
  int tickCount;
};

struct GameObjectSelection{
  void *selected;
  GameObjectSelectionType type;
  int enabled;
  int visible;
  char nameString[255];
  char infoString[255];
};

struct Environment{
  int delayBeforeSummer;
  int winterCountdown;
  double winterCountdownFloat;
  enum Season season;
  int years_survived;
  int treeGraphic;
  Weather weather;
};

struct GameObjectData{
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

  int years_survived;
  Environment environment;
  GameObjectSelection gameObjectSelection;
};

GameObjectData createGameObjectData(void);
void cleanGameObjectData(GameObjectData *gameObjectData);
ProgrammableWorker *createProgrammableWorker(GameObjectData *gameObjectData);
Hive createHive(void);
Tree createTree(GameObjectData *gameObjectData, int forceX, int forceY);
ResourceNodeSpawner createResourceNodeSpawner(GameObjectData *gameObjectData, int maximumNodeCount, double xPosition, double yPosition, double radius);
IceCreamPerson *createIceCreamPerson(void);
DroppedIceCream *createDroppedIceCream(void);
RoamingSpider *createRoamingSpider(void);

/* Function for the GameLoop to update the world */
void updateGameObjects(GameObjectData *gameObjectData, AudioData *audioData, AnnouncementsData *announcementsData, int ticks);
void renderGameObjects(GameObjectData *gameObjectData, GraphicsData *graphicsData);

/* Functions to get keyboard and mouse controls passed in*/
void selectGameObject(GameObjectData *gameObjectData, GraphicsData *graphicsData, SDL_Event *event);
void centerViewOnHive(GraphicsData *graphicsData, GameObjectData *gameObjectData);

/* Useful things for the AI to have access to */
int countProgrammableWorkersInRange(GameObjectData *gameObjectData, SDL_Point center, double radius);
void nullifyLocalAIInformation(GameObjectData *gameObjectData);
int countResourceNodesInRadius(GameObjectData *gameObjectData, int x, int y, double radius);
Tree *getNearestTree(GameObjectData *gameObjectData, int x, int y);
ProgrammableWorker *getNearestWorker(GameObjectData *gameObjectData, int x, int y, ProgrammableWorker *ignore);
int countIdleWorkers(GameObjectData *gameObjectData);
