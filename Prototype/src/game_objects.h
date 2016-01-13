#include "announcements.h"

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
  /* standard positioning data - x,y coords */
  int displayInfo;
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

  int flapTimer;

  char *beeStatus;

  int insideHive;

  double senseRange;

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
  HiveCell *hiveCells;
  int hiveCellCount;
  int hiveCellSpawnDelay;
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

typedef enum {STARVATION, COLD} gameOverCause;

struct GameObjectData{
  Weather weather;
  Hive hive;
  Tree *tree;
  ResourceNodeSpawner *resourceNodeSpawners;
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
  gameOverCause gameOverCause;
  Uint32 gameOverEventNum;
  Uint32 objectDisplayEventNum;
  int gameRestart;
  AudioData audioData;
  /* Grand tidy up, made these not defined */
  int HONEY_REQUIRED_FOR_WINTER;
  int REQUIREMENT_YEAR_INCREASE_PERCENTAGE;
  int DELAY_BEFORE_SUMMER;
  int NUMBER_OF_FLOWER_PATCHES;
  int DEFAULT_MAXNODECOUNT;
  int X_SIZE_OF_WORLD;
  int Y_SIZE_OF_WORLD;
  int DEFAULT_SPAWNRADIUS;
  int INITIAL_NUMBER_OF_WORKERS;
  int NUMBER_OF_TREES;
  int SIZE_OF_TREE;
  int X_SIZE_OF_HIVE;
  int Y_SIZE_OF_HIVE;
  int WORKER_SENSE_RANGE;
  int X_SIZE_OF_WORKER;
  int Y_SIZE_OF_WORKER;
  int LENGTH_OF_STATUS_STRING;
  double WORKER_SPEED;
  int PERSON_HEIGHT;
  int PERSON_WIDTH;
  int MAX_DAYS_TO_WINTER;
  int SIZE_OF_TREESTUMP;
  int WINTER_THRESHOLD;
  int HIVE_SHELTER_RADIUS;
  int MS_BETWEEN_FLAPPING;
  int STUNNED_AFTER_STING_DURATION;
  int SUGAR_VALUE_OF_FLOWER;
  int SUGAR_VALUE_OF_ICECREAM;
  int ICECREAM_PICKUP_RADIUS;
  int CHANCE_OF_REGAINING_FLIGHT;
  int STRIDE_FREQUENCY;
  int STING_HIT_RADIUS;
  int DEFAULT_SPAWNDELAY;
  int X_SIZE_OF_NODE;
  int Y_SIZE_OF_NODE;
  int TICKSPERWEATHER;
  int CHANCE_OF_CLOUD;
  int CHANCE_OF_RAIN;
  int SIZE_OF_FLOWER;
  double WINTER_COUNTDOWN_SPEED;
  int AUTUMN_THRESHOLD;
  int DROPPED_ICECREAM_WIDTH;
  int DROPPED_ICECREAM_HEIGHT;
  int RANDOMISE_SPAWNRADIUS;
  int TREE_SHELTER_RADIUS;
  int MELT_TIME_THRESHOLD;
  int MAX_DROPPED_ICECREAM_WIDTH;
  int COLD_DEATH_THRESHOLD;
  int BEE_SHRINK_FACTOR_ON_GROUND;
  int CARRYING_FLOWER_INDEX_OFFSET;
  int CARRYING_ICECREAM_INDEX_OFFSET;
  int ICE_CREAM_PERSON_PROB;
};

void killAllBees(ProgrammableWorker **programmableWorker);
void killProgrammableWorker(GameObjectData *gameObjectData, ProgrammableWorker **programmableWorker);
ProgrammableWorker *createProgrammableWorker(GameObjectData *gameObjectData);
void updateProgrammableWorker(ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, AnnouncementsData *announcementsData, int ticks);
void updateIceCreamPerson(GameObjectData *gameObjectData, int ticks);

Hive initHive(GameObjectData *gameObjectData,ConfigurationData *configData);
Tree createTree(GameObjectData *gameObjectData, int forceX, int forceY);
Weather createWeatherLayer(void);

/* I think that it will be better to do static respawning objects through
   creating spawners rather than directly into the world.

   Doing it this way, we can use the created hierarchy to cull collision
   checking quickly and respawning resources will be easier */

int getFirstDeadResourceNode(ResourceNodeSpawner *resourceNodeSpawner);
ResourceNodeSpawner createResourceNodeSpawner(GameObjectData *gameObjectData, int maximumNodeCount, float xPosition, float yPosition, float radius);
void updateResourceNodeSpawner(GameObjectData *gameObjectData, ResourceNodeSpawner *spawner, int ticks);
void updateWeather(GameObjectData *gameObjectData, AudioData *audioData, Weather *weather, int ticks);

void initResourceNode(GameObjectData *gameObjectData, ResourceNode *resourceNode);
ResourceNode createResourceNode(GameObjectData *gameObjectData, ResourceNodeSpawner *parentSpawner);
IceCreamPerson *createIceCreamPerson(GameObjectData *gameObjectData);
DroppedIceCream *createDroppedIceCream(GameObjectData *gameObjectData);
RoamingSpider *createRoamingSpider(GameObjectData *gameObjectData);
void reInitialiseIceCreamPerson(GameObjectData *gameObjectData);
void reInitialiseRoamingSpider(GameObjectData *gameObjecData);
void updateGameObjects(GameObjectData *gameObjectData, AudioData *audioData, GraphicsData *graphicsData, AnnouncementsData *announcementsData, int ticks);
int countProgrammableWorkersInRange(GameObjectData *gameObjectData, SDL_Point center, double radius);
int isPointInRangeOf(SDL_Point point, SDL_Point center, double radius);

void initAudio(GameObjectData *gameObjectData, AudioData audioData);
void centerViewOnHive(GraphicsData *graphicsData, GameObjectData *gameObjectData);
void objectInfoDisplay(GameObjectData *gameObjectData, GraphicsData *graphicsData, AnnouncementsData *announcementsData,
SDL_MouseButtonEvent *mbEvent);

void nullifyLocalAIInformation(GameObjectData *gameObjectData);
int countResourceNodesInRadius(GameObjectData *gameObjectData, int x, int y, double radius);
