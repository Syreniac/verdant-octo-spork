#include "AI.h"

/* All blockFunctions must:
  - Return either integers between 0 and 2.
  - Have accomodations for resetting any arguments adjusted in the course of the
    function.
  - Take the same arguments to ensure pointer compatibility */

int blockFunction_Print(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i = 0;
  printf("blockFunction_Print\n");
  while(i < arguments->numOfChars){
      printf("%c",arguments->characters[i]);
      i++;
  }
  printf("\n");
  return(1);
}

int blockFunction_IfWorkerCargoGreaterThan(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->cargo > arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfWorkerStatusEqual(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i = 0;
  while(i<arguments->numOfInts){
    i++;
  }
  if(programmableWorker->status == arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfWorkerOutsideOfBounds(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->xPosition >= (X_SIZE_OF_SCREEN - programmableWorker->xDimensions) || programmableWorker->xPosition < 0 ||
     programmableWorker->yPosition >= (Y_SIZE_OF_SCREEN - programmableWorker->yDimensions) || programmableWorker->yPosition < 0){
       return(1);
    }
    return(2);
}

int blockFunction_IfWorkerWithinDistanceOfHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(getDistance2BetweenPoints(programmableWorker->xPosition,
                               programmableWorker->yPosition,
                               gameObjectData->hive.xPosition,
                               gameObjectData->hive.yPosition) <= arguments->floats[0]){
    return(1);
  }
  return(2);
}

int blockFunction_SetWorkerHeadingRandomly(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->heading = randPi() * 2;
  programmableWorker->status = LEAVING;
  return(1);
}


int blockFunction_WorkerReturnToHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->heading = atan2(gameObjectData->hive.xPosition - programmableWorker->xPosition,
                                      gameObjectData->hive.yPosition - programmableWorker->yPosition);
  programmableWorker->status = RETURNING;
  return(1);
}

int runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  runBlockFunctionOverWorker(&(blockFunctionRoot->blockFunctions[0]), programmableWorker,gameObjectData);
}

int runBlockFunctionOverWorker(BlockFunction *blockFunction, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i;
  i = blockFunction->wrapped_function(&(blockFunction->arguments),
                                     programmableWorker,
                                     gameObjectData);
  if(i == 1){
      if(blockFunction->primary != NULL){
        runBlockFunctionOverWorker(blockFunction->primary,
                                   programmableWorker,
                                   gameObjectData);
      }
      else{
        return(0);
      }
  }
  else if(i == 2){
      if(blockFunction->secondary != NULL){
        runBlockFunctionOverWorker(blockFunction->secondary,
                                   programmableWorker,
                                   gameObjectData);
      }
      else{
        return(0);
      }
  }
  return(0);
}

BlockFunctionRoot generateGenericWorkerOrders(void){
  BlockFunctionRoot blockFunctionRoot;
  BlockFunction blockFunction1,
                blockFunction2,
                blockFunction3,
                blockFunction4,
                blockFunction5,
                blockFunction6;

  BlockFunctionArgs blockFunctionArgs1,
                    blockFunctionArgs2,
                    blockFunctionArgs3,
                    blockFunctionArgs4,
                    blockFunctionArgs5,
                    blockFunctionArgs6;

  /* This is the "if worker status = RETURNING" */
  blockFunction1.name = "blockFunction_IfWorkerStatusEqual";
  blockFunction1.wrapped_function = &blockFunction_IfWorkerStatusEqual;
  blockFunctionArgs1.integers = calloc(1, sizeof(int));
  blockFunctionArgs1.integers[0] = 2;
  blockFunctionArgs1.numOfInts = 1;
  blockFunction1.arguments = blockFunctionArgs1;

  /* This is the "within x distance of the hive part." */
  blockFunction2.name = "blockFunction_IfWorkerWithinDistanceOfHive";
  blockFunction2.wrapped_function = &blockFunction_IfWorkerWithinDistanceOfHive;
  blockFunctionArgs2.floats = calloc(1, sizeof(float));
  blockFunctionArgs2.floats[0] = 1.0;
  blockFunctionArgs2.numOfFloats = 1;
  blockFunction2.arguments = blockFunctionArgs2;

  /* This is the "then randomly move away from the hive" part */
  blockFunction3.name = "blockFunction_SetWorkerHeadingRandomly";
  blockFunction3.wrapped_function = &blockFunction_SetWorkerHeadingRandomly;
  blockFunction3.arguments = blockFunctionArgs3;
  /* Putting the primary link to NULL will break the AI recursion
     after this function */

  /* This is the "check to see if the bee picked anything up part" */
  blockFunction4.name = "blockFunction_IfWorkerCargoGreaterThan";
  blockFunction4.wrapped_function = &blockFunction_IfWorkerCargoGreaterThan;
  blockFunctionArgs4.integers = calloc(1,sizeof(int));
  blockFunctionArgs4.integers[0] = 0;
  blockFunctionArgs4.numOfInts = 1;
  blockFunction4.arguments = blockFunctionArgs4;

  /* This is "return to the hive" */
  blockFunction5.name = "blockFunction_WorkerReturnToHive";
  blockFunction5.wrapped_function = &blockFunction_WorkerReturnToHive;
  blockFunction5.arguments = blockFunctionArgs5;
  /* Set the primary to NULL to break the recursion */

  /* This is "check whether the bee is in the bounds or not" */
  blockFunction6.name = "blockFunction_IfWorkerOutsideOfBounds";
  blockFunction6.wrapped_function = &blockFunction_IfWorkerOutsideOfBounds;
  blockFunction6.arguments = blockFunctionArgs6;
  /* Just to be confusing, I'll save us a function by looping this back up to
     the function above, which is an example of how you can do a sort of OR
     statement, by having multiple IF FunctionBlocks pointing to a condition */

  blockFunctionRoot.blockFunctions = calloc(6,sizeof(BlockFunction));
  blockFunctionRoot.blockFunctions[0] = blockFunction1;
  blockFunctionRoot.blockFunctions[1] = blockFunction2;
  blockFunctionRoot.blockFunctions[2] = blockFunction3;
  blockFunctionRoot.blockFunctions[3] = blockFunction4;
  blockFunctionRoot.blockFunctions[4] = blockFunction5;
  blockFunctionRoot.blockFunctions[5] = blockFunction6;
  blockFunctionRoot.numOfBlockFunctions = 6;


  blockFunctionRoot.blockFunctions[0].primary = &blockFunctionRoot.blockFunctions[1];
  blockFunctionRoot.blockFunctions[0].secondary = &blockFunctionRoot.blockFunctions[3];

  blockFunctionRoot.blockFunctions[1].primary = &blockFunctionRoot.blockFunctions[2];
  blockFunctionRoot.blockFunctions[1].secondary = &blockFunctionRoot.blockFunctions[3];

  blockFunctionRoot.blockFunctions[2].primary = NULL;
  blockFunctionRoot.blockFunctions[2].secondary = NULL;

  blockFunctionRoot.blockFunctions[3].primary = &blockFunctionRoot.blockFunctions[4];
  blockFunctionRoot.blockFunctions[3].secondary = &blockFunctionRoot.blockFunctions[5];

  blockFunctionRoot.blockFunctions[4].primary = NULL;
  blockFunctionRoot.blockFunctions[4].secondary = NULL;

  blockFunctionRoot.blockFunctions[5].primary = &blockFunctionRoot.blockFunctions[4];
  blockFunctionRoot.blockFunctions[5].secondary = NULL;

  return(blockFunctionRoot);
}
