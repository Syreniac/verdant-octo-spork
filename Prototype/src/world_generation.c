#include "world_generation.h"

void generateResourceNodeSpawners(GameObjectData *gameObjectData){

  /* Create two ResourceNodeSpawners at 'random' positions */
  int i = 0;
  while(i < NUMBER_OF_FLOWER_PATCHES){
    gameObjectData->resourceNodeSpawners[i] = createResourceNodeSpawner(DEFAULT_MAXNODECOUNT,
                                                                        rand()%X_SIZE_OF_WORLD,
                                                                        rand()%Y_SIZE_OF_WORLD,
                                                                        DEFAULT_SPAWNRADIUS);
    i++;
  }

  /* We need to keep a record of how many resourceNodeSpawners we have active
     so that we can loop through them later */
  gameObjectData->resourceNodeSpawnerCount = 0;
}

void generateProgrammableWorkers(GameObjectData *gameObjectData){
  /* Create two ProgrammableWorkers */

  createProgrammableWorker(gameObjectData);
  createProgrammableWorker(gameObjectData);
  createProgrammableWorker(gameObjectData);
  createProgrammableWorker(gameObjectData);
  createProgrammableWorker(gameObjectData);
  createProgrammableWorker(gameObjectData);
  createProgrammableWorker(gameObjectData);/**/
}


void generateIceCreamPerson(GameObjectData *gameObjectData){
/*create iceCreamPerson at initial location outside of world boundaries*/
  gameObjectData->iceCreamPerson = createIceCreamPerson();
}

void generateHive(GameObjectData *gameObjectData){
  /* Create a Hive */
  gameObjectData->hive = createHive();
}

void generateTrees(GameObjectData *gameObjectData){
  /* Create a Tree */
  int i;
  for(i = 0; i < NUMBER_OF_TREES; i++){

     gameObjectData->tree[i] = createTree();

  }
}



void generateWeatherLayer(GameObjectData *gameObjectData){
  /* Create a Hive */
  gameObjectData->weather = createWeatherLayer();
}
