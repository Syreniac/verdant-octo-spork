#include "world_generation.h"

void generateResourceNodeSpawners(GameObjectData *gameObjectData){

  /* Create two ResourceNodeSpawners at 'random' positions */
  int i = 0;
  while(i < gameObjectData->NUMBER_OF_FLOWER_PATCHES){
    gameObjectData->resourceNodeSpawners[i] = createResourceNodeSpawner(gameObjectData,
                                                                        gameObjectData->DEFAULT_MAXNODECOUNT,
                                                                        rand()%gameObjectData->X_SIZE_OF_WORLD,
                                                                        rand()%gameObjectData->Y_SIZE_OF_WORLD,
                                                                        gameObjectData->DEFAULT_SPAWNRADIUS);
    i++;
  }

  /* We need to keep a record of how many resourceNodeSpawners we have active
     so that we can loop through them later */
  gameObjectData->resourceNodeSpawnerCount = gameObjectData->NUMBER_OF_FLOWER_PATCHES;
}

void generateProgrammableWorkers(GameObjectData *gameObjectData){
  /* Create two ProgrammableWorkers */
  int i = 0;

  while(i < gameObjectData->INITIAL_NUMBER_OF_WORKERS){
	   createProgrammableWorker(gameObjectData);
	    i++;
  }
}


void generateIceCreamPerson(GameObjectData *gameObjectData){
/*create iceCreamPerson at initial location outside of world boundaries*/
  gameObjectData->iceCreamPerson = createIceCreamPerson(gameObjectData);
}

void generateDroppedIceCream(GameObjectData *gameObjectData){
/*create iceCreamPerson at initial location outside of world boundaries*/
  gameObjectData->droppedIceCream = createDroppedIceCream(gameObjectData);
}

void generateRoamingSpider(GameObjectData *gameObjectData){
/*create roaming Spider at initial location outside of world boundaries*/
  gameObjectData->roamingSpider = createRoamingSpider(gameObjectData);
}

void generateTrees(GameObjectData *gameObjectData){
  /* Create a Tree */
  int i;


  for(i = 2; i < gameObjectData->NUMBER_OF_TREES; i++){
     gameObjectData->tree[i] = createTree(gameObjectData,
                                          rand() % gameObjectData->X_SIZE_OF_WORLD - gameObjectData->SIZE_OF_TREE/2,
                                          rand() % gameObjectData->Y_SIZE_OF_WORLD - gameObjectData->SIZE_OF_TREE/2);
  }


  gameObjectData->tree[0] = createTree(gameObjectData,
                                       (gameObjectData->X_SIZE_OF_WORLD/2 - gameObjectData->X_SIZE_OF_HIVE/2),
  									                   (gameObjectData->Y_SIZE_OF_WORLD/2 - gameObjectData->Y_SIZE_OF_HIVE/2));

  gameObjectData->tree[1] = createTree(gameObjectData,
                                       (gameObjectData->X_SIZE_OF_WORLD/2 - gameObjectData->X_SIZE_OF_HIVE/2),
  									                   (gameObjectData->Y_SIZE_OF_WORLD/2 - gameObjectData->Y_SIZE_OF_HIVE/2));


}



void generateWeatherLayer(GameObjectData *gameObjectData){
  /* Create a Hive */
  gameObjectData->weather = createWeatherLayer();
}
