#include "world_generation.h"

void generateResourceNodeSpawners(GameObjectData *gameObjectData){

  /* Create two ResourceNodeSpawners at 'random' positions */
  gameObjectData->resourceNodeSpawners[0] = createResourceNodeSpawner(DEFAULT_MAXNODECOUNT,
                                                                      rand()%X_SIZE_OF_WORLD,
                                                                      rand()%Y_SIZE_OF_WORLD,
                                                                      DEFAULT_SPAWNRADIUS);

  gameObjectData->resourceNodeSpawners[1] = createResourceNodeSpawner(DEFAULT_MAXNODECOUNT,
                                                                      rand()%X_SIZE_OF_WORLD,
                                                                      rand()%Y_SIZE_OF_WORLD,
                                                                      DEFAULT_SPAWNRADIUS);


  gameObjectData->resourceNodeSpawners[2] = createResourceNodeSpawner(DEFAULT_MAXNODECOUNT*2,
                                                                      rand()%X_SIZE_OF_WORLD,
                                                                      rand()%Y_SIZE_OF_WORLD,
                                                                      DEFAULT_SPAWNRADIUS);

  /* We need to keep a record of how many resourceNodeSpawners we have active
     so that we can loop through them later */
  gameObjectData->resourceNodeSpawnerCount = 3;
}

void generateProgrammableWorkers(GameObjectData *gameObjectData){
  /* Create two ProgrammableWorkers */

  createProgrammableWorker(gameObjectData);
    createProgrammableWorker(gameObjectData);
      createProgrammableWorker(gameObjectData);
        createProgrammableWorker(gameObjectData);
          createProgrammableWorker(gameObjectData);
            createProgrammableWorker(gameObjectData);
              createProgrammableWorker(gameObjectData);
  /*gameObjectData->programmableWorkers[0] = createProgrammableWorker();*/
  /* Make sure the count is correct */
  gameObjectData->programmableWorkerCount = 1;
}

void generateHive(GameObjectData *gameObjectData){
  /* Create a Hive */
  gameObjectData->hive = createHive();
}

void generateWeatherLayer(GameObjectData *gameObjectData){
  /* Create a Hive */
  gameObjectData->weather = createWeatherLayer();
}
