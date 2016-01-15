#include "AI.h"

static void resetBlockFunctionGlobals(BlockFunctionGlobals *globals);
static int doIntCompWithCharOperator(int a, int b, char op);
static int doDoubleCompWithCharOperator(double a, double b, char op);

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

FILE *fopenAndVerify(char *file_name, char *permission){
  FILE *file;

  printf("loading non-image file such as AI: %s\n",file_name);
  file = fopen(file_name, permission);
  if(file == NULL){
    printf("File loading has failed: %s. Errno reports: %d.\n", file_name, errno);
    printf("See errno.h to look up key of error values.\n");
    assert(file != NULL);
  }

  return file;
}

int blockFunction_HuntSpider(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  if(gameObjectData->roamingSpider->currently_on_screen && !gameObjectData->roamingSpider->deadSpider){
    programmableWorker->status = LEAVING;
    programmableWorker->heading = getAngleBetweenRects(&gameObjectData->roamingSpider->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IfDistanceToSpider(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->roamingSpider->rect);
  if(doDoubleCompWithCharOperator(d2, (double) pow(arguments->integers[0]*WORKER_SENSE_RANGE,2),arguments->characters[0])){
    return(1);
  }
  return(2);

}

int blockFunction_HeadToNearestWorker(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  SDL_Point point = getCenterOfRect(programmableWorker->rect);
  ProgrammableWorker *p = getNearestWorker(gameObjectData,point.x,point.y,programmableWorker);
  if(p!=NULL){
    programmableWorker->heading = getAngleBetweenRects(&p->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IfCountToWinter(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)programmableWorker;
  if(doIntCompWithCharOperator(gameObjectData->environment.winterCountdown,arguments->integers[0],arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_GoToIceCream(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  if(gameObjectData->droppedIceCream->dropped){
    programmableWorker->status = LEAVING;
    programmableWorker->heading = getAngleBetweenRects(&gameObjectData->droppedIceCream->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IfIceCreamExists(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  if(gameObjectData->droppedIceCream->dropped){
    return 1;
  }
  return 2;
}

int blockFunction_IfIceCreamManExists(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  if(gameObjectData->iceCreamPerson->currently_on_screen){
    return 1;
  }
  return 2;
}

int blockFunction_GoToIceCreamMan(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  if(gameObjectData->iceCreamPerson->currently_on_screen){
    programmableWorker->status = LEAVING;
    programmableWorker->heading = getAngleBetweenRects(&gameObjectData->iceCreamPerson->rect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_GoToTree(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  SDL_Point p = getCenterOfRect(programmableWorker->rect);
  Tree *tree = getNearestTree(gameObjectData,p.x,p.y);
  if(tree != NULL){
    programmableWorker->heading = getAngleBetweenRects(&tree->stumpRect,&programmableWorker->rect);
  }
  return 1;
}

int blockFunction_IsRaining(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  if(gameObjectData->environment.weather.present_weather == Rain){
    return 1;
  }
  return 2;
}

int blockFunction_PercentChance(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)programmableWorker;
  (void)gameObjectData;
  if(rand() % 100 < arguments->integers[0]){
    return 1;
  }
  return 2;
}

int blockFunction_IfCargo(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)gameObjectData;
  if(doIntCompWithCharOperator(programmableWorker->cargo,arguments->integers[0],arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_CountNearFlowers(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  SDL_Point p = getCenterOfRect(programmableWorker->rect);
  globals->count = countResourceNodesInRadius(gameObjectData,p.x,p.y,WORKER_SENSE_RANGE);
  return 1;
}

int blockFunction_CopyPointFromSelected(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)arguments;
  (void)gameObjectData;
  if(globals->selectedWorker != NULL){
    programmableWorker->brain.remembered_point = globals->selectedWorker->brain.remembered_point;
  }
  return 1;
}

int blockFunction_CompareCountToSaved(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)arguments;
  (void)gameObjectData;
  if(doIntCompWithCharOperator(globals->count, programmableWorker->brain.storedCount, arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_PickNearbyWorker(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_LoadCountFromOther(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)arguments;
  (void)programmableWorker;
  (void)gameObjectData;
  if(globals->selectedWorker != NULL){
    globals->count = globals->selectedWorker->brain.storedCount;
  }
  return 1;
}

int blockFunction_SaveCountFromOther(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)arguments;
  (void)gameObjectData;
  if(globals->selectedWorker != NULL){
    programmableWorker->brain.storedCount = globals->selectedWorker->brain.storedCount;
  }
  return 1;
}

int blockFunction_SaveCount(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->brain.storedCount = globals->count;
  return 1;
}

int blockFunction_LoadCount(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)arguments;
  (void)gameObjectData;
  globals->count = programmableWorker->brain.storedCount;
  return 1;
}

int blockFunction_Sleep(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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
    programmableWorker->brain.waitTime = 1000 * arguments->integers[0];
    programmableWorker->brain.aiStartPoint = arguments->blockFunctionIndex;
    return 0;
}

int blockFunction_CountNearWorkers(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_IfCountZero(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)arguments;
  (void)programmableWorker;
  (void)gameObjectData;
  if(globals->count == 0){
    return 1;
  }
  return 2;
}

int blockFunction_IfCount(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)programmableWorker;
  (void)gameObjectData;
  if(doIntCompWithCharOperator(globals->count, arguments->integers[0], arguments->characters[0])){
    return 1;
  }
  return 2;
}

int blockFunction_IfNearOtherWorker(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_Void(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)programmableWorker;
  (void)gameObjectData;
	return(1);
}

int blockFunction_IfIdle(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
	if(programmableWorker->status == IDLE){
		return 1;
	}
	return 2;
}

int blockFunction_IfReturning(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
	if(programmableWorker->status == RETURNING){
		return 1;
	}
	return 2;
}

int blockFunction_IfOutsideOfBounds(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->rect.x >= (X_SIZE_OF_WORLD - programmableWorker->rect.w) || programmableWorker->rect.x <= 0 ||
     programmableWorker->rect.y >= (Y_SIZE_OF_WORLD - programmableWorker->rect.h) || programmableWorker->rect.y <= 0){
       return(1);
    }
    return(2);
}

int blockFunction_IfDistanceToHive(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)gameObjectData;
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(doDoubleCompWithCharOperator(d2, (double) pow(arguments->integers[0]*WORKER_SENSE_RANGE,2),arguments->characters[0])){
    return(1);
  }
  return(2);

}

int blockFunction_SetHeadingRandomly(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->heading = (double)(randPi() * 2);
  programmableWorker->status = LEAVING;
  return(1);
}


int blockFunction_ReturnToHive(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  programmableWorker->heading = getAngleBetweenRects(&gameObjectData->hive.rect,&programmableWorker->rect);
  programmableWorker->status = RETURNING;
  return(1);
}

int blockFunction_RememberCurrentLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->brain.remembered_point.x = programmableWorker->rect.x;
  programmableWorker->brain.remembered_point.y = programmableWorker->rect.y;
  programmableWorker->brain.is_point_remembered = 1;
  return 1;
}

int blockFunction_GoToStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
    SDL_Point p = getCenterOfRect(programmableWorker->rect);
    if(!programmableWorker->brain.is_point_remembered){
      return 1;
    }
    programmableWorker->heading = atan2((double)(programmableWorker->brain.remembered_point.x - p.x),
                                        (double)(programmableWorker->brain.remembered_point.y - p.y));
    programmableWorker->status = LEAVING;
    return 1;
}

int blockFunction_ForgetStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  programmableWorker->brain.is_point_remembered = 0;
  return 1;
}

int blockFunction_IfNodeFound(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->brain.foundNode != NULL){
    return 1;
  }
  return 2;
}

int blockFunction_HeadToFoundNode(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->brain.foundNode != NULL){
    programmableWorker->heading = getAngleBetweenRects(&programmableWorker->brain.foundNode->rect,&programmableWorker->rect);
    programmableWorker->status = LEAVING;
  }
  return 1;
}

int blockFunction_HasStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  (void)globals;
  (void)arguments;
  (void)gameObjectData;
  if(programmableWorker->brain.is_point_remembered){
    return 1;
  }
  return 2;
}

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  int i;
  resetBlockFunctionGlobals(&blockFunctionRoot->globals);
  blockFunctionRoot->globals.ticks = ticks;
  BlockFunction *blockFunction = &(blockFunctionRoot->blockFunctions[programmableWorker->brain.aiStartPoint]);
  if(blockFunctionRoot->numOfBlockFunctions == 0){
	  return;
  }
  while(1){
    i = runBlockFunctionOverWorker(blockFunction, &blockFunctionRoot->globals, programmableWorker,gameObjectData);
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

int runBlockFunctionOverWorker(BlockFunction *blockFunction, BlockFunctionGlobals *globals, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i;
  i = blockFunction->wrapped_function(globals,
                                     &(blockFunction->arguments),
                                     programmableWorker,
                                     gameObjectData);
  return(i);
}

blockFunction_WrappedFunction getBlockFunctionByName(char *blockFunctionName){
  if(strcmp(blockFunctionName,"IfOutsideBounds") == 0){
    return &blockFunction_IfOutsideOfBounds;
  }
  if(strcmp(blockFunctionName,"SetHeadingRandomly") == 0){
    return &blockFunction_SetHeadingRandomly;
  }
  if(strcmp(blockFunctionName,"ReturnToHive") == 0){
    return &blockFunction_ReturnToHive;
  }
  if(strcmp(blockFunctionName,"RememberCurrentLocation") == 0){
    return &blockFunction_RememberCurrentLocation;
  }
  if(strcmp(blockFunctionName,"ForgetStoredLocation") == 0){
    return &blockFunction_ForgetStoredLocation;
  }
  if(strcmp(blockFunctionName,"HasStoredLocation") == 0){
    return &blockFunction_HasStoredLocation;
  }
  if(strcmp(blockFunctionName,"GoToStoredLocation") == 0){
    return &blockFunction_GoToStoredLocation;
  }
  if(strcmp(blockFunctionName, "IfNodeFound") == 0){
    return &blockFunction_IfNodeFound;
  }
  if(strcmp(blockFunctionName, "HeadToFoundNode") == 0){
    return &blockFunction_HeadToFoundNode;
  }
  if(strcmp(blockFunctionName, "IfDistanceToHive") == 0){
	  return &blockFunction_IfDistanceToHive;
  }
  if(strcmp(blockFunctionName, "START") == 0){
	  return &blockFunction_Void;
  }
  if(strcmp(blockFunctionName,"IfIdle") == 0){
	  return &blockFunction_IfIdle;
  }
  if(strcmp(blockFunctionName,"IfReturning") == 0){
	  return &blockFunction_IfReturning;
  }
  if(strcmp(blockFunctionName,"IfNearOtherWorker") == 0){
    return &blockFunction_IfNearOtherWorker;
  }
  if(strcmp(blockFunctionName,"Sleep") == 0){
    return &blockFunction_Sleep;
  }
  if(strcmp(blockFunctionName,"CountNearWorkers") == 0){
    return &blockFunction_CountNearWorkers;
  }
  if(strcmp(blockFunctionName,"IfCountZero") == 0){
    return &blockFunction_IfCountZero;
  }
  if(strcmp(blockFunctionName,"LoadCount") == 0){
    return &blockFunction_LoadCount;
  }
  if(strcmp(blockFunctionName,"SaveCount") == 0){
    return &blockFunction_SaveCount;
  }
  if(strcmp(blockFunctionName,"LoadCountFromOther") == 0){
    return &blockFunction_LoadCountFromOther;
  }
  if(strcmp(blockFunctionName,"SaveCountFromOther") == 0){
    return &blockFunction_SaveCountFromOther;
  }
  if(strcmp(blockFunctionName,"PickNearbyWorker") == 0){
    return &blockFunction_PickNearbyWorker;
  }
  if(strcmp(blockFunctionName,"CompareCountToSaved") == 0){
    return &blockFunction_CompareCountToSaved;
  }
  if(strcmp(blockFunctionName,"CopyPointFromSelected") == 0){
    return &blockFunction_CopyPointFromSelected;
  }
  if(strcmp(blockFunctionName,"CountNearFlowers") == 0){
    return &blockFunction_CountNearFlowers;
  }
  if(strcmp(blockFunctionName,"IfCargo") == 0){
    return &blockFunction_IfCargo;
  }
  if(strcmp(blockFunctionName,"IfCount") == 0){
    return &blockFunction_IfCount;
  }
  if(strcmp(blockFunctionName,"PercentChance") == 0){
    return &blockFunction_PercentChance;
  }
  if(strcmp(blockFunctionName,"GoToTree") == 0){
    return &blockFunction_GoToTree;
  }
  if(strcmp(blockFunctionName,"IfRaining") == 0){
    return &blockFunction_IsRaining;
  }
  if(strcmp(blockFunctionName,"GoToIceCreamMan") == 0){
    return &blockFunction_GoToIceCreamMan;
  }
  if(strcmp(blockFunctionName,"IfIceCreamManExists") == 0){
    return &blockFunction_IfIceCreamManExists;
  }
  if(strcmp(blockFunctionName,"IfIceCreamExists") == 0){
    return &blockFunction_IfIceCreamExists;
  }
  if(strcmp(blockFunctionName,"GoToIceCream") == 0){
    return &blockFunction_GoToIceCream;
  }
  if(strcmp(blockFunctionName,"IfCountToWinter") == 0){
    return &blockFunction_IfCountToWinter;
  }
  if(strcmp(blockFunctionName,"HeadToNearestWorker") == 0){
    return &blockFunction_HeadToNearestWorker;
  }
  if(strcmp(blockFunctionName,"HuntSpider") == 0){
    return &blockFunction_HuntSpider;
  }
  if(strcmp(blockFunctionName,"IfDistanceToSpider") == 0){
    return &blockFunction_IfDistanceToSpider;
  }
  printf("ERROR: Unrecognised function name: \"%s\".\n Substituting a VOID function.\n",blockFunctionName);
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

BlockFunctionRoot makeBlockFunctionRootFromFile(FILE *file){
	char *string = fileToString(file);
	int i = 0;
	BlockFunctionRoot blockFunctionRoot = makeBlockFunctionRootFromString(string,
	                                                                      getNumberOfTextStoredBlocks(file,&i));
	free(string);
	return blockFunctionRoot;
}

BlockFunctionRoot makeBlockFunctionRootFromString(char *str, int numOfBlocks){
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
			blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1] = createAIBlockFunctionFromTokens(&blockFunctionRoot,
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
		blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1] = createAIBlockFunctionFromTokens(&blockFunctionRoot,
																													                                                      i,tokensToUse);
    blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1].arguments.blockFunctionIndex = blockFunctionRoot.numOfBlockFunctions-1;
	}
	return blockFunctionRoot;
}

BlockFunction createAIBlockFunctionFromTokens(BlockFunctionRoot *blockFunctionRoot, int numOfLinesToUse, char *tokensToUse[7]){
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
			blockFunction.wrapped_function = getBlockFunctionByName(tokensToUse[i]);
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
  printf("-----------\n");
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
  if(&aiData->blockFunctionRoots[0].numOfBlockFunctions == 0){
    return;
  }
  for(programmableWorker = gameObjectData -> first_programmable_worker; programmableWorker != NULL; programmableWorker = programmableWorker->next){
    runBlockFunctionRootOverWorker(&aiData->blockFunctionRoots[0],
                                   programmableWorker,
                                   gameObjectData,
                                   ticks);
  }
}

BlockFunction *testBlockFunctionRootForLoops(BlockFunction *toTest, BlockFunction **testAgainst, int countToTestAgainst){
  int i = 0;
  BlockFunction *returnValue = NULL;
  BlockFunction **testAgainstCopy = calloc(countToTestAgainst+1,sizeof(BlockFunction*));
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
    returnValue = testBlockFunctionRootForLoops(toTest->primary,testAgainstCopy,countToTestAgainst+1);
  }
  if(returnValue == NULL && toTest->secondary != NULL){
    returnValue = testBlockFunctionRootForLoops(toTest->secondary,testAgainstCopy,countToTestAgainst+1);
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

static void makeAIBlockTemplate(AIData *aiData, char name[50], int r, int g, int b, int numOfArguments, ...){
  va_list vargs;
  va_start(vargs,numOfArguments);
  BlockFunctionTemplate *template = malloc(sizeof(BlockFunctionTemplate));
  BlockFunctionTemplate *previous = aiData->templates;
  int i = 0;
  template->red = r;
  template->green = g;
  template->blue = b;
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
  aiData.blockFunctionRoots = calloc(1,sizeof(BlockFunctionRoot));
  if(file != NULL){
  	aiData.blockFunctionRoots[0] = makeBlockFunctionRootFromFile(file);
  	fclose(file);
  	erroneousBlockFunction = testBlockFunctionRootForLoops(&aiData.blockFunctionRoots[0].blockFunctions[0],NULL,0);
  	if(erroneousBlockFunction != NULL){
  		while(erroneousBlockFunction != &aiData.blockFunctionRoots[0].blockFunctions[i]){
  		  i++;
  		}
  		nullifyBlockFunctionRoot(&aiData.blockFunctionRoots[0],NULL);
  	}
  }
  #define ACTION_BLOCK_COLOR 0xFF,0x80,0x00
  #define WAIT_BLOCK_COLOR 0x04,0x5F,0xB4
  #define COUNT_BLOCK_COLOR 0x5F,0xB4,0x04
  #define IF_BLOCK_COLOR 248,221,35
  #define POINT_BLOCK_COLOR 0xcc,0x99,0xff
  aiData.templates = NULL;

	makeAIBlockTemplate(&aiData,"SetHeadingRandomly",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"ReturnToHive",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToStoredLocation",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"HeadToFoundNode",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToTree",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToIceCreamMan",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"GoToIceCream",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"HeadToNearestWorker",ACTION_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"HuntSpider",ACTION_BLOCK_COLOR,1,BF_THEN);

	makeAIBlockTemplate(&aiData,"Sleep",WAIT_BLOCK_COLOR,2,BF_THEN,BF_DURATION);

	makeAIBlockTemplate(&aiData,"HasStoredLocation",IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"PickNearbyWorker",IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfNodeFound",IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfIdle",IF_BLOCK_COLOR,2,BF_PRIMARY,BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfReturning",IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfOutsideBounds",IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfRaining",IF_BLOCK_COLOR,2,BF_PRIMARY,BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"PercentChance",IF_BLOCK_COLOR,3,BF_PRIMARY, BF_SECONDARY, BF_PERCENT);
	makeAIBlockTemplate(&aiData,"CompareCountToSaved",IF_BLOCK_COLOR,3,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON);
	makeAIBlockTemplate(&aiData,"IfCargo",IF_BLOCK_COLOR,4,BF_PRIMARY,BF_SECONDARY,BF_COMPARISON,BF_CARGO_QUANTITY);
	makeAIBlockTemplate(&aiData,"IfCount",IF_BLOCK_COLOR,4,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON, BF_COUNT_QUANTITY);
	makeAIBlockTemplate(&aiData,"IfDistanceToHive",IF_BLOCK_COLOR,4,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON, BF_DISTANCE);
	makeAIBlockTemplate(&aiData,"IfDistanceToSpider",IF_BLOCK_COLOR,4,BF_PRIMARY, BF_SECONDARY, BF_COMPARISON, BF_DISTANCE);
	makeAIBlockTemplate(&aiData,"IfIceCreamManExists",IF_BLOCK_COLOR,2,BF_PRIMARY, BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfIceCreamExists",IF_BLOCK_COLOR,2,BF_PRIMARY,BF_SECONDARY);
	makeAIBlockTemplate(&aiData,"IfCountToWinter",IF_BLOCK_COLOR,4,BF_PRIMARY,BF_SECONDARY,BF_COMPARISON,BF_SEASON_TIME);

	makeAIBlockTemplate(&aiData,"CountNearFlowers",COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"CountNearWorkers",COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"LoadCount",COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"SaveCount",COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"LoadCountFromOther",COUNT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"SaveCountFromOther",COUNT_BLOCK_COLOR,1,BF_THEN);

	makeAIBlockTemplate(&aiData,"RememberCurrentLocation",POINT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"ForgetStoredLocation",POINT_BLOCK_COLOR,1,BF_THEN);
	makeAIBlockTemplate(&aiData,"CopyPointFromSelected",POINT_BLOCK_COLOR,1,BF_THEN);

	return aiData;
}

static void resetBlockFunctionGlobals(BlockFunctionGlobals *globals){
  globals->count = 0;
  globals->selectedWorker = NULL;
}
