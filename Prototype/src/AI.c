#include "AI.h"

static void resetBlockFunctionGlobals(BlockFunctionGlobals *globals);
static int doIntCompWithCharOperator(int a, int b, char op);
static int doDoubleCompWithCharOperator(double a, double b, char op);
static void resetBlockFunctionUniversals(BlockFunctionUniversals *universals);
static blockFunction_WrappedFunction getBlockFunctionByName(AIData *aiData, char *blockFunctionName);

static void adjustProgrammableWorkerStatus(BlockFunctionUniversals *universals, ProgrammableWorker *programmableWorker, enum ProgrammableWorkerStatus new_status);

static int doIntCompWithCharOperator(int a, int b, char op){
  switch(op){
    case '<':
      return a < b;
    case '>':
      return a > b;
    case '=':
      return a == b;
  }
  return 0;
}

static int doDoubleCompWithCharOperator(double a, double b, char op){
  switch(op){
    case '<':
      return a < b;
    case '>':
      return a > b;
    case '=':
      return a == b;
  }
  assert(op != op);
  return 0;
}

/* All blockFunctions must:
  - Return either integers between 0 and 2.
  - Have accomodations for resetting any arguments adjusted in the course of the
    function.
  - Take the same arguments to ensure pointer compatibility */

int blockFunction_IfNumOfIdleWorkers(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)programmableWorker;
  (void)gameObjectData;
  if(doIntCompWithCharOperator(universals->idle_count,arguments->integers[0],arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_HuntSpider(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  if(gameObjectData->roamingSpider->currently_on_screen && !gameObjectData->roamingSpider->deadSpider){
    adjustProgrammableWorkerStatus(universals,programmableWorker,LEAVING);
    programmableWorker->heading = getAngleBetweenRects(&gameObjectData->roamingSpider->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IfDistanceToSpider(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->roamingSpider->rect);
  if(doDoubleCompWithCharOperator(d2, (double) pow(arguments->integers[0]*WORKER_SENSE_RANGE_BLOCKS,2),arguments->characters[0])){
    return(1);
  }
  return(2);

}

int blockFunction_HeadToNearestWorker(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  SDL_Point point = getCenterOfRect(programmableWorker->rect);
  if(programmableWorker->brain.nearestWorkerCacheTime <= 0){
    programmableWorker->brain.nearestWorker = getNearestWorker(gameObjectData,point.x,point.y,programmableWorker);
    programmableWorker->brain.nearestWorkerCacheTime += NEARESTWORKER_CACHETIME + globals->flip * 20;
  }
  if(programmableWorker->brain.nearestWorker!=NULL){
    programmableWorker->heading = getAngleBetweenRects(&programmableWorker->brain.nearestWorker->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IfdaysToWinter(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)programmableWorker;
  if(doIntCompWithCharOperator(gameObjectData->environment.winterCountdown,arguments->integers[0],arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_GoToIceCream(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  if(gameObjectData->droppedIceCream->dropped){
    adjustProgrammableWorkerStatus(universals,programmableWorker,LEAVING);
    programmableWorker->heading = getAngleBetweenRects(&gameObjectData->droppedIceCream->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IfIceCreamExists(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  if(gameObjectData->droppedIceCream->dropped){
    return 1;
  }
  return 2;
}

int blockFunction_IfDistanceToGarry(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->iceCreamPerson->rect);
  if(doDoubleCompWithCharOperator(d2, (double) pow(arguments->integers[0]*WORKER_SENSE_RANGE_BLOCKS,2),arguments->characters[0])){
    return(1);
  }
  return(2);

}

int blockFunction_GoToIceCreamMan(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  if(gameObjectData->iceCreamPerson->currently_on_screen){
    adjustProgrammableWorkerStatus(universals,programmableWorker,LEAVING);
    programmableWorker->heading = getAngleBetweenRects(&gameObjectData->iceCreamPerson->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_GoToTree(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  SDL_Point p = getCenterOfRect(programmableWorker->rect);
  Tree *tree = getNearestTree(gameObjectData,p.x,p.y);
  if(tree != NULL){
    programmableWorker->heading = getAngleBetweenRects(&tree->stumpRect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IfRaining(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  if(gameObjectData->environment.weather.present_weather == Rain){
    return 1;
  }
  return 2;
}

int blockFunction_PercentChance(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)programmableWorker;
  (void)gameObjectData;
  if(rand() % 100 < arguments->integers[0]){
    return 1;
  }
  return 2;
}

int blockFunction_IfCargo(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)gameObjectData;
  if(doIntCompWithCharOperator(programmableWorker->cargo,arguments->integers[0],arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_CountNearFlowers(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  SDL_Point p = getCenterOfRect(programmableWorker->rect);
  globals->count = countResourceNodesInRadius(gameObjectData,p.x,p.y,WORKER_SENSE_RANGE);
  return 1;
}

int blockFunction_CopyPointFromSelected(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  (void)gameObjectData;
  if(globals->selectedWorker != NULL && globals->selectedWorker->brain.is_point_remembered){
    programmableWorker->brain.is_point_remembered = globals->selectedWorker->brain.is_point_remembered;
    programmableWorker->brain.remembered_point = globals->selectedWorker->brain.remembered_point;
  }
  return 1;
}

int blockFunction_CompareCountToSaved(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  (void)gameObjectData;
  if(doIntCompWithCharOperator(globals->count, programmableWorker->brain.storedCount, arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_PickNearbyWorker(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  (void)gameObjectData;
  ProgrammableWorker *p = gameObjectData->first_programmable_worker;
  while(p != NULL){
    if(p != programmableWorker && getDistance2BetweenRects(programmableWorker->rect,p->rect) < WORKER_SENSE_RANGE * WORKER_SENSE_RANGE){
      globals->selectedWorker = p;
      return 1;
    }
    p = p->next;
  }
  return 2;
}

int blockFunction_LoadCountFromOther(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  (void)programmableWorker;
  (void)gameObjectData;
  if(globals->selectedWorker != NULL){
    globals->count = globals->selectedWorker->brain.storedCount;
  }
  return 1;
}

int blockFunction_SaveCountFromOther(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  (void)gameObjectData;
  if(globals->selectedWorker != NULL){
    programmableWorker->brain.storedCount = globals->selectedWorker->brain.storedCount;
  }
  return 1;
}

int blockFunction_SaveCount(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->brain.storedCount = globals->count;
  return 1;
}

int blockFunction_LoadCount(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  (void)gameObjectData;
  globals->count = programmableWorker->brain.storedCount;
  return 1;
}

int blockFunction_Delay(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)gameObjectData;
    if(programmableWorker->brain.waitTime != -1){
      programmableWorker->brain.waitTime -= globals->ticks;
      if(programmableWorker->brain.waitTime <= 0){
        programmableWorker->brain.waitTime = -1;
        programmableWorker->brain.aiStartPoint = 0;
        return 1;
      }
      return 0;
    }
    programmableWorker->brain.waitTime = 1000 * arguments->integers[0] + globals->flip;
    programmableWorker->brain.aiStartPoint = arguments->blockFunctionIndex;
    return 0;
}

int blockFunction_CountNearWorkers(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  ProgrammableWorker *p = gameObjectData->first_programmable_worker;
  globals->count = 0;
  while(p != NULL){
    if(p != programmableWorker && getDistance2BetweenRects(programmableWorker->rect,p->rect) < WORKER_SENSE_RANGE * WORKER_SENSE_RANGE){
      globals->count++;
    }
    p = p->next;
  }
  return 1;
}

int blockFunction_IfCountZero(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)arguments;
  (void)programmableWorker;
  (void)gameObjectData;
  if(globals->count == 0){
    return 1;
  }
  return 2;
}

int blockFunction_IfCount(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)programmableWorker;
  (void)gameObjectData;
  if(doIntCompWithCharOperator(globals->count, arguments->integers[0], arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_IfNearOtherWorker(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  ProgrammableWorker *p = gameObjectData->first_programmable_worker;
  while(p != NULL){
    if(p!=programmableWorker && getDistance2BetweenRects(programmableWorker->rect,p->rect) < WORKER_SENSE_RANGE * WORKER_SENSE_RANGE){
      return 1;
    }
    p = p->next;
  }
  return 2;
}

int blockFunction_Void(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  (void)gameObjectData;
	return(1);
}

int blockFunction_IfIdle(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
	if(programmableWorker->status == IDLE){
		return 1;
	}
	return 2;
}

int blockFunction_IfReturning(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
	if(programmableWorker->status == RETURNING){
		return 1;
	}
	return 2;
}

int blockFunction_IfOutsideBounds(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->rect.x >= (X_SIZE_OF_WORLD - programmableWorker->rect.w) || programmableWorker->rect.x <= 0 ||
     programmableWorker->rect.y >= (Y_SIZE_OF_WORLD - programmableWorker->rect.h) || programmableWorker->rect.y <= 0){
       return(1);
    }
    return(2);
}

int blockFunction_IfDistanceToHive(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)gameObjectData;
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(doDoubleCompWithCharOperator(d2, (double) pow(arguments->integers[0]*WORKER_SENSE_RANGE,2),arguments->characters[0])){
    return(1);
  }
  return(2);

}

int blockFunction_SetHeadingRandomly(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->heading = (double)(randPi() * 2);
    adjustProgrammableWorkerStatus(universals,programmableWorker,LEAVING);
  return(1);
}


int blockFunction_ReturnToHive(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  programmableWorker->heading = getAngleBetweenRects(&gameObjectData->hive.rect,&programmableWorker->rect);
    adjustProgrammableWorkerStatus(universals,programmableWorker,RETURNING);
  return(1);
}

int blockFunction_StoreCurrentLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->brain.remembered_point.x = programmableWorker->rect.x;
  programmableWorker->brain.remembered_point.y = programmableWorker->rect.y;
  programmableWorker->brain.is_point_remembered = 1;
  return 1;
}

int blockFunction_GoToStoredLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
    SDL_Point p = getCenterOfRect(programmableWorker->rect);
    if(!programmableWorker->brain.is_point_remembered){
      return 1;
    }
    programmableWorker->heading = atan2((double)(programmableWorker->brain.remembered_point.x - p.x),
                                        (double)(programmableWorker->brain.remembered_point.y - p.y));
    adjustProgrammableWorkerStatus(universals,programmableWorker,LEAVING);
    return 1;
}

int blockFunction_ForgetStoredLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->brain.is_point_remembered = 0;
  return 1;
}

int blockFunction_IfNodeFound(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->brain.foundNode != NULL){
    return 1;
  }
  return 2;
}

int blockFunction_HeadToFoundNode(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->brain.foundNode != NULL){
    programmableWorker->heading = getAngleBetweenRects(&programmableWorker->brain.foundNode->rect,&programmableWorker->rect);
    adjustProgrammableWorkerStatus(universals,programmableWorker,LEAVING);
  }
  return 1;
}

int blockFunction_HasStoredLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)universals;
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->brain.is_point_remembered){
    return 1;
  }
  return 2;
}

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot, BlockFunctionUniversals *universals, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  static int flip = 0;
  int i;
  flip = !flip;
  resetBlockFunctionGlobals(&blockFunctionRoot->globals);
  if(programmableWorker->brain.nearestWorkerCacheTime > 0){
    programmableWorker->brain.nearestWorkerCacheTime -= ticks;
  }
  blockFunctionRoot->globals.ticks = ticks;
  blockFunctionRoot->globals.flip = flip;
  BlockFunction *blockFunction = &(blockFunctionRoot->blockFunctions[programmableWorker->brain.aiStartPoint]);
  if(blockFunctionRoot->numOfBlockFunctions == 0){
	  return;
  }
  while(1){
    i = runBlockFunctionOverWorker(blockFunction, universals, &blockFunctionRoot->globals, programmableWorker,gameObjectData);
    if(i == 1 && blockFunction->primary != NULL){
      blockFunction = blockFunction->primary;
    }
    else if(i == 2 && blockFunction->secondary != NULL){
      blockFunction = blockFunction->secondary;
    }
    else{
      return;
    }
  }
}

int runBlockFunctionOverWorker(BlockFunction *blockFunction, BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i;
  i = blockFunction->wrapped_function(universals,
                                      globals,
                                      &(blockFunction->arguments),
                                      programmableWorker,
                                      gameObjectData);
  return(i);
}

static blockFunction_WrappedFunction getBlockFunctionByName(AIData *aiData, char *blockFunctionName){
  BlockFunctionTemplate *t = aiData->templates;
  while(t != NULL){
    if(strcmp(t->name, blockFunctionName) == 0){
      return t->function;
    }
    t = t->next;
  }
  fprintf(stderr,"ERROR: Unrecognised function name: \"%s\".\n Substituting a VOID function.\n",blockFunctionName);
  return &blockFunction_Void;
}

int countCharsInString(char *string, char countChar){
  int count = 0;
  int i = 0;
  while(string[i] != '\0'){
    if(string[i] == countChar){
      count++;
    }
    i++;
  }
  return(count);
}

int getNumberOfTextStoredBlocks(FILE *file, int *maxDescLength){
  char read_char;
  int lineStarted = 1;
  int functionBlockCount = 0;
  int testMaxDescLength = 0;

  fseek(file,0,SEEK_SET);
  while((read_char = fgetc(file)) != EOF){
    /* Because the current format of BlockFunction files says that all
       lines within a single block function must be indented by tabs or spaces,
       we can count the number of BlockFunctions stored in a text file by
       counting the number of lines which don't start with either tabs or spaces */
    if(read_char == '\n'){
      lineStarted = 1;
    }
    else if(lineStarted == 1){
      if(read_char != '\t' && read_char != ' '){
        if(testMaxDescLength > *maxDescLength){
          *maxDescLength = testMaxDescLength - 1;
        }
        testMaxDescLength = 0;
        functionBlockCount++;
        lineStarted = 0;
      }
      else{
        lineStarted = 0;
      }
    }
    else{
      lineStarted = 0;
      testMaxDescLength++;
    }
  }
  /* Move the stream reading back to the start */
  fseek(file,0,SEEK_SET);
  return(functionBlockCount);
}

BlockFunctionRoot makeBlockFunctionRootFromFile(AIData *aiData, FILE *file){
	char *string = fileToString(file);
	int i = 0;
	BlockFunctionRoot blockFunctionRoot = makeBlockFunctionRootFromString(aiData,
                                                                        string,
	                                                                      getNumberOfTextStoredBlocks(file,&i));
	free(string);
	return blockFunctionRoot;
}

BlockFunctionRoot makeBlockFunctionRootFromString(AIData *aiData, char *str, int numOfBlocks){
	char *tok;
	char *tokensToUse[7] = {NULL};
	int i = -1;
	BlockFunctionRoot blockFunctionRoot;
	blockFunctionRoot.blockFunctions = calloc(numOfBlocks,sizeof(BlockFunction));
	blockFunctionRoot.numOfBlockFunctions = 0;

	tok = strtok(str,"\n");
	while(tok != NULL){
		i++;
		if(tok[0] != '\t' && i > 0){
			blockFunctionRoot.numOfBlockFunctions++;
			blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1] = createAIBlockFunctionFromTokens(aiData,
                                                                                                                  &blockFunctionRoot,
																														                                                      i,tokensToUse);
      blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1].arguments.blockFunctionIndex = blockFunctionRoot.numOfBlockFunctions-1;
			i = 0;
		}
		tokensToUse[i] = tok;
		tok = strtok(NULL,"\n");
	}
  /* Because the method we use for determining when to take our token collection
     to be made into a proper BlockFunction is when we hit a new BlockFunction name
     in the string, we will end up with a bit at the end left over, which is what
     we take here (below this comment) and make into a proper BlockFunction */
	if(i > 0 || blockFunctionRoot.numOfBlockFunctions == numOfBlocks - 1){
		if(blockFunctionRoot.numOfBlockFunctions == numOfBlocks - 1){
			i++;
		}
		blockFunctionRoot.numOfBlockFunctions++;
		blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1] = createAIBlockFunctionFromTokens(aiData,
                                                                                                                &blockFunctionRoot,
																													                                                      i,tokensToUse);
    blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1].arguments.blockFunctionIndex = blockFunctionRoot.numOfBlockFunctions-1;
	}
	return blockFunctionRoot;
}

BlockFunction createAIBlockFunctionFromTokens(AIData *aiData, BlockFunctionRoot *blockFunctionRoot, int numOfLinesToUse, char *tokensToUse[7]){
	int i = 0;
	BlockFunction blockFunction;
	int read_int;
	double read_float;
	char read_char;
	unsigned int read_position = 0;
	int read_add;
	blockFunction.primary = NULL;
	blockFunction.secondary = NULL;
	blockFunction.arguments.numOfInts = 0;
	blockFunction.arguments.integers = NULL;
	blockFunction.arguments.characters = NULL;
	blockFunction.arguments.numOfChars = 0;
	blockFunction.arguments.floats = NULL;
	blockFunction.arguments.numOfFloats = 0;

	while(i < numOfLinesToUse){
		if(tokensToUse[i][0] != '\t'){
			strcpy(blockFunction.name,tokensToUse[i]);
			blockFunction.wrapped_function = getBlockFunctionByName(aiData, tokensToUse[i]);
		}
		else if(strncmp(tokensToUse[i],"\tprimary = ",10) == 0){
			blockFunction.primary = &blockFunctionRoot->blockFunctions[atoi(&tokensToUse[i][11]) - 1];
		}
		else if(strncmp(tokensToUse[i],"\tsecondary = ",12) == 0){
			blockFunction.secondary = &blockFunctionRoot->blockFunctions[atoi(&tokensToUse[i][13]) - 1];
		}
		else if(strncmp(tokensToUse[i],"\tintegers = ",11) == 0){
			read_position = 12;
			blockFunction.arguments.numOfInts = 0;
			blockFunction.arguments.integers = NULL;
			while((sscanf(&tokensToUse[i][read_position],"%d%n",&read_int,&read_add)) != 0 && read_add != 0 && read_position < strlen(tokensToUse[i])){
  			read_position += read_add+1;
				blockFunction.arguments.numOfInts++;
				blockFunction.arguments.integers = realloc(blockFunction.arguments.integers,
				                                           blockFunction.arguments.numOfInts * sizeof(int));
				blockFunction.arguments.integers[blockFunction.arguments.numOfInts-1] = read_int;
			}
		}
		else if(strncmp(tokensToUse[i],"\tfloats = ",9) == 0){
			read_position = 10;
			blockFunction.arguments.numOfFloats = 0;
			blockFunction.arguments.floats = NULL;
			while((sscanf(&tokensToUse[i][read_position],"%lf%n",&read_float,&read_add)) != 0 && read_add != 0){
				read_position += read_add+1;
				blockFunction.arguments.numOfFloats++;
				blockFunction.arguments.floats = realloc(blockFunction.arguments.floats,
				                                         blockFunction.arguments.numOfFloats * sizeof(double));
				blockFunction.arguments.floats[blockFunction.arguments.numOfFloats-1] = read_float;
			}
		}
		else if(strncmp(tokensToUse[i],"\tchars = ",8) == 0){
			read_position = 9;
			blockFunction.arguments.numOfChars = 0;
			blockFunction.arguments.characters = NULL;
			while(tokensToUse[i][read_position] != '\n' && tokensToUse[i][read_position] != '\0' && tokensToUse[i][read_position] != EOF){
          read_char = tokensToUse[i][read_position];
          read_position += 1;
          blockFunction.arguments.numOfChars++;
          blockFunction.arguments.characters = realloc(blockFunction.arguments.characters,
                                                   blockFunction.arguments.numOfChars * sizeof(char));
          blockFunction.arguments.characters[blockFunction.arguments.numOfChars-1] = read_char;
			}
		}
		i++;
	}
	return blockFunction;
}

void nullifyBlockFunctionRoot(BlockFunctionRoot *root, GameObjectData *gameObjectData){
	int i = 0;
  if(gameObjectData != NULL){
    nullifyLocalAIInformation(gameObjectData);
  }
	while(i < root->numOfBlockFunctions){
		if(root->blockFunctions[i].arguments.numOfInts != 0){
			free(root->blockFunctions[i].arguments.integers);
			root->blockFunctions[i].arguments.integers = NULL;
			root->blockFunctions[i].arguments.numOfInts = 0;
		}
		if(root->blockFunctions[i].arguments.numOfFloats != 0){
			free(root->blockFunctions[i].arguments.floats);
			root->blockFunctions[i].arguments.floats = NULL;
			root->blockFunctions[i].arguments.numOfFloats = 0;
		}
		if(root->blockFunctions[i].arguments.numOfChars != 0){
			free(root->blockFunctions[i].arguments.characters);
			root->blockFunctions[i].arguments.characters = NULL;
			root->blockFunctions[i].arguments.numOfChars = 0;
		}
		i++;
	}
  if(root->blockFunctions != NULL){
  	free(root->blockFunctions);
  }
	root->blockFunctions = NULL;
	root->numOfBlockFunctions = 0;
}

void runAI(AIData *aiData, GameObjectData *gameObjectData, int ticks){
  /* AIData *aiData                 = the pointer to the AIData struct that
                                      holds all the AI data we need to run it
     GameObjectData *gameObjectData = the pointer to the GameObjectData struct
                                      that holds all the data we need to find
                                      GameObjects and get the context for the AI

     This function is where we run the AI over all the ProgrammableWorkers. */
  ProgrammableWorker *programmableWorker;
  BlockFunctionUniversals universals;
  if(&aiData->blockFunctionRoots[0].numOfBlockFunctions == 0 || gameObjectData->pause_status == 1){
    return;
  }
  universals.idle_count = countIdleWorkers(gameObjectData);
  for(programmableWorker = gameObjectData -> first_programmable_worker; programmableWorker != NULL; programmableWorker = programmableWorker->next){
    runBlockFunctionRootOverWorker(&aiData->blockFunctionRoots[0],
                                   &universals,
                                   programmableWorker,
                                   gameObjectData,
                                   ticks);
  }
}

BlockFunctionTemplate *getBlockFunctionTemplateByName(AIData *aiData, char *name){
  BlockFunctionTemplate *movingPointer = aiData->templates;
  while(movingPointer!=NULL){
    if(strcmp(movingPointer->name,name) == 0){
      return movingPointer;
    }
    movingPointer = movingPointer->next;
  }
  return NULL;
}

BlockFunction *testBlockFunctionRootForLoops(AIData *aiData, BlockFunction *toTest, BlockFunction **testAgainst, int countToTestAgainst){
  int i = 0;
  BlockFunction *returnValue = NULL;
  BlockFunction **testAgainstCopy;
  BlockFunctionTemplate *template = getBlockFunctionTemplateByName(aiData,toTest->name);

  if(template!=NULL && template->terminal){
    /* Terminal blocks halt the execution of an AI script and therefore don't
       need to concern themselves with loops - they might not work perfectly with
       them, but they won't freeze the game */
    return NULL;
  }

  testAgainstCopy = calloc(countToTestAgainst+1,sizeof(BlockFunction*));
  /* Make a copy of the testAgainst array that we can work with */
  if(testAgainst!=NULL && testAgainstCopy !=NULL){
    memcpy(testAgainstCopy,testAgainst,sizeof(BlockFunction*) * countToTestAgainst);
  }
  testAgainstCopy[countToTestAgainst] = toTest;
  /* If the block function we're testing against is in this list, we've found something that loops on itself */
  while(i < countToTestAgainst){
    if(testAgainst[i] == toTest){
      free(testAgainstCopy);
      return toTest;
    }
    i++;
  }
  /* Add the block function we're testing against to the test array and then test the next block functions */
  if(toTest->primary != NULL){
    returnValue = testBlockFunctionRootForLoops(aiData,toTest->primary,testAgainstCopy,countToTestAgainst+1);
  }
  if(returnValue == NULL && toTest->secondary != NULL){
    returnValue = testBlockFunctionRootForLoops(aiData,toTest->secondary,testAgainstCopy,countToTestAgainst+1);
  }
  /* Clean up */
  if(testAgainstCopy!=NULL){
    free(testAgainstCopy);
  }
  /* return the recursion */
  return returnValue;
}

int testBlockFunctionRootForStart(BlockFunctionRoot *root){
	if(root->blockFunctions[0].primary != NULL || root->blockFunctions[0].secondary != NULL){
		return 1;
	}
	return 0;
}

static void makeAIBlockTemplate(AIData *aiData, char name[50], blockFunction_WrappedFunction function, int terminal, int r, int g, int b, int numOfArguments, ...){
  va_list vargs;
  va_start(vargs,numOfArguments);
  BlockFunctionTemplate *template = malloc(sizeof(BlockFunctionTemplate));
  BlockFunctionTemplate *previous = aiData->templates;
  int i = 0;
  template->red = r;
  template->green = g;
  template->blue = b;
  template->terminal = terminal;
  template->function = function;
  strcpy(template->name,name);
  if(numOfArguments != 0){
    template->arguments = malloc(sizeof(BlockFunctionArgumentType) * numOfArguments);
    template->numOfArguments = numOfArguments;
    while(i < numOfArguments){
      template->arguments[i] = va_arg(vargs,BlockFunctionArgumentType);
      i++;
    }
  }
  else{
    template->arguments = NULL;
    template->numOfArguments = 0;
  }
  template->next = NULL;
  if(previous == NULL){
    aiData->templates = template;
  }
  else{
    while(previous->next != NULL){
      previous = previous->next;
    }
    previous->next = template;
  }
  va_end(vargs);
}

AIData initAIData(void){
	AIData aiData;
	FILE *file;
	int i = 0;
	BlockFunction *erroneousBlockFunction = NULL;
	file = fopen("GenericWorkerAI.txt","r");
  #define ACTION_BLOCK_COLOR 0xFF,0x80,0x00
  #define WAIT_BLOCK_COLOR 0x04,0x5F,0xB4
  #define COUNT_BLOCK_COLOR 0x5F,0xB4,0x04
  #define IF_BLOCK_COLOR 248,221,35
  #define POINT_BLOCK_COLOR 0xcc,0x99,0xff
  aiData.templates = NULL;

	makeAIBlockTemplate(&aiData,"SetHeadingRandomly", // pointer to where we want to store it / In game name
                              &blockFunction_SetHeadingRandomly, // the function we're going to use
                              0,                                // Whether it is terminal - i.e. causes the AI loop to stop (0 unless you're sure what you're doing)
                              ACTION_BLOCK_COLOR,               // The colour to use for it
                              1,BF_THEN);                       // The number of arguments needed and their types (see BlockFunctionArgumentType enum in AI.h)
                                                                // If your block is an if/else type, you'll need BF_PRIMARY and BF_SECONDARY
	makeAIBlockTemplate(&aiData,"ReturnToHive",&blockFunction_ReturnToHive,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToStoredLocation",&blockFunction_GoToStoredLocation,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"HeadToFoundNode",&blockFunction_HeadToFoundNode,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToTree",&blockFunction_GoToTree,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToIceCreamMan",&blockFunction_GoToIceCreamMan,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToIceCream",&blockFunction_GoToIceCream,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"HeadToNearestWorker",&blockFunction_HeadToNearestWorker,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"HuntSpider",&blockFunction_HuntSpider,
                      0,ACTION_BLOCK_COLOR,1,BF_THEN);

	makeAIBlockTemplate(&aiData,"Delay",&blockFunction_Delay,
                      1,WAIT_BLOCK_COLOR,2,BF_THEN,BF_DURATION);

	makeAIBlockTemplate(&aiData,"HasStoredLocation",&blockFunction_HasStoredLocation,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"PickNearbyWorker",&blockFunction_PickNearbyWorker,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfNodeFound",&blockFunction_IfNodeFound,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfIdle",&blockFunction_IfIdle,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY,BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfReturning",&blockFunction_IfReturning,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfOutsideBounds",&blockFunction_IfOutsideBounds,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfRaining",&blockFunction_IfRaining,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY,BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"PercentChance",&blockFunction_PercentChance,
                      0,IF_BLOCK_COLOR,3,BF_PRIMARY, BF_SECONDARY, BF_PERCENT);
	makeAIBlockTemplate(&aiData,"CompareCountToSaved",&blockFunction_CompareCountToSaved,
                      0,IF_BLOCK_COLOR,3,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON);
	makeAIBlockTemplate(&aiData,"IfCargo",&blockFunction_IfCargo,
                      0,IF_BLOCK_COLOR,4,BF_PRIMARY,BF_SECONDARY,BF_COMPARISON,BF_CARGO_QUANTITY);
	makeAIBlockTemplate(&aiData,"IfCount",&blockFunction_IfCount,
                      0,IF_BLOCK_COLOR,4,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON, BF_COUNT_QUANTITY);
	makeAIBlockTemplate(&aiData,"IfDistanceToHive",&blockFunction_IfDistanceToHive,
                      0,IF_BLOCK_COLOR,4,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON, BF_DISTANCE);
	makeAIBlockTemplate(&aiData,"IfDistanceToSpider",&blockFunction_IfDistanceToSpider,
                      0,IF_BLOCK_COLOR,4,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON, BF_DISTANCE);
	makeAIBlockTemplate(&aiData,"IfDistanceToGarry",&blockFunction_IfDistanceToGarry,
                      0,IF_BLOCK_COLOR,4,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfIceCreamExists",&blockFunction_IfIceCreamExists,
                      0,IF_BLOCK_COLOR,2,BF_PRIMARY,BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfDaysToWinter",&blockFunction_IfdaysToWinter,
                      0,IF_BLOCK_COLOR,4,BF_PRIMARY,BF_SECONDARY,BF_COMPARISON,BF_SEASON_TIME);
  makeAIBlockTemplate(&aiData,"IfNumOfIdleWorkers",&blockFunction_IfNumOfIdleWorkers,
                      0,IF_BLOCK_COLOR,4,BF_PRIMARY,BF_SECONDARY,BF_COMPARISON,BF_CARGO_QUANTITY);

	makeAIBlockTemplate(&aiData,"CountNearFlowers",&blockFunction_CountNearFlowers,
                      0,COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"CountNearWorkers",&blockFunction_CountNearWorkers,
                      0,COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"LoadCount",&blockFunction_LoadCount,
                      0,COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"SaveCount",&blockFunction_SaveCount,
                      0,COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"LoadCountFromOther",&blockFunction_LoadCountFromOther,
                      0,COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"SaveCountFromOther",&blockFunction_SaveCountFromOther,
                      0,COUNT_BLOCK_COLOR,1,BF_THEN);

	makeAIBlockTemplate(&aiData,"StoreCurrentLocation",&blockFunction_StoreCurrentLocation,
                      0,POINT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"ForgetStoredLocation",&blockFunction_ForgetStoredLocation,
                      0,POINT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"CopyPointFromSelected",&blockFunction_CopyPointFromSelected,
                      0,POINT_BLOCK_COLOR,1,BF_THEN);

  aiData.blockFunctionRoots = calloc(1,sizeof(BlockFunctionRoot));
  if(file != NULL){
  	aiData.blockFunctionRoots[0] = makeBlockFunctionRootFromFile(&aiData,file);
  	fclose(file);
  	erroneousBlockFunction = testBlockFunctionRootForLoops(&aiData,&aiData.blockFunctionRoots[0].blockFunctions[0],NULL,0);
  	if(erroneousBlockFunction != NULL){
  		while(erroneousBlockFunction != &aiData.blockFunctionRoots[0].blockFunctions[i]){
  		  i++;
  		}
  		nullifyBlockFunctionRoot(&aiData.blockFunctionRoots[0],NULL);
  	}
  }

	return aiData;
}

static void resetBlockFunctionGlobals(BlockFunctionGlobals *globals){
  globals->count = 0;
  globals->selectedWorker = NULL;
}

static void adjustProgrammableWorkerStatus(BlockFunctionUniversals *universals, ProgrammableWorker *programmableWorker, enum ProgrammableWorkerStatus new_status){
  switch(programmableWorker->status){
    case IDLE:
      universals->idle_count--;
      break;
    default:
      break;
  }
  programmableWorker->status = new_status;
  switch(programmableWorker->status){
    case IDLE:
      universals->idle_count++;
      break;
    default:
      break;
  }
}
