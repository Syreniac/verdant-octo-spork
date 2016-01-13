#include "AI.h"

static void resetBlockFunctionGlobals(BlockFunctionGlobals *globals);

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

int blockFunction_CountNearFlowers(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  SDL_Point p = getCenterOfRect(programmableWorker->rect);
  globals->count = countResourceNodesInRadius(gameObjectData,p.x,p.y,WORKER_SENSE_RANGE);
  return 1;
}

int blockFunction_CopyPointFromSelected(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(globals->selectedWorker != NULL){
    programmableWorker->brain.remembered_point = globals->selectedWorker->brain.remembered_point;
  }
  return 1;
}

int blockFunction_IfGreaterThanSaved(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(globals->count > programmableWorker->brain.storedCount){
    return 1;
  }
  return 2;
}

int blockFunction_PickNearbyWorker(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
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

int blockFunction_LoadCountFromOther(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(globals->selectedWorker != NULL){
    globals->count = globals->selectedWorker->brain.storedCount;
  }
  return 1;
}

int blockFunction_SaveCountFromOther(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(globals->selectedWorker != NULL){
    programmableWorker->brain.storedCount = globals->selectedWorker->brain.storedCount;
  }
  return 1;
}

int blockFunction_SaveCount(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  programmableWorker->brain.storedCount = globals->count;
  return 1;
}

int blockFunction_LoadCount(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  globals->count = programmableWorker->brain.storedCount;
  return 1;
}

int blockFunction_Sleep(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
    if(programmableWorker->brain.waitTime != -1){
      programmableWorker->brain.waitTime -= ticks;
      if(programmableWorker->brain.waitTime <= 0){
        programmableWorker->brain.waitTime = -1;
        programmableWorker->brain.aiStartPoint = 0;
        return 1;
      }
      return 0;
    }
    programmableWorker->brain.waitTime = 1000;
    programmableWorker->brain.aiStartPoint = arguments->blockFunctionIndex;
    return 0;
}

int blockFunction_CountNearWorkers(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
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

int blockFunction_IfCountZero(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(globals->count == 0){
    return 1;
  }
  return 2;
}

int blockFunction_IfNearOtherWorker(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  ProgrammableWorker *p = gameObjectData->first_programmable_worker;
  while(p != NULL){
    if(p!=programmableWorker && getDistance2BetweenRects(programmableWorker->rect,p->rect) < WORKER_SENSE_RANGE * WORKER_SENSE_RANGE){
      return 1;
    }
    p = p->next;
  }
  return 2;
}

int blockFunction_OneInAHundred(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  int r = rand() % 100;
  if(r == 0){
    return 1;
  }
  return 2;
}

int blockFunction_CoinFlip(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  int r = (rand() % 2) + 1;
  return r;
}

int blockFunction_Void(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
	return(1);
}

int blockFunction_IfIdle(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
	if(programmableWorker->status == IDLE){
		return 1;
	}
	return 2;
}

int blockFunction_IfReturning(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
	if(programmableWorker->status == RETURNING){
		return 1;
	}
	return 2;
}

int blockFunction_IfHasCargo(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
	if(programmableWorker->cargo > 0){
		return 1;
	}
	return 2;
}

int blockFunction_IfCargoGreaterThan(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(programmableWorker->cargo > arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfStatusEqual(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(programmableWorker->status == arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfOutsideOfBounds(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){

  if(programmableWorker->rect.x >= (X_SIZE_OF_WORLD - programmableWorker->rect.w) || programmableWorker->rect.x <= 0 ||
     programmableWorker->rect.y >= (Y_SIZE_OF_WORLD - programmableWorker->rect.h) || programmableWorker->rect.y <= 0){
       return(1);
    }
    return(2);
}

int blockFunction_IfWithinDistanceOfHive(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(d2 <= (double)(arguments->floats[0])){
    return(1);
  }
  return(2);
}

int blockFunction_IfNearHive(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(d2 <= 50.0){
    return(1);
  }
  return(2);

}

int blockFunction_SetHeadingRandomly(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  programmableWorker->heading = (double)(randPi() * 2);
  programmableWorker->status = LEAVING;
  return(1);
}


int blockFunction_ReturnToHive(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  programmableWorker->heading = getAngleBetweenRects(&gameObjectData->hive.rect,&programmableWorker->rect);
  programmableWorker->status = RETURNING;
  return(1);
}

int blockFunction_IfNumOfFlowersInRadius(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  int count = 0;
  int i = 0;
  int j = 0;
  while(i < gameObjectData->resourceNodeSpawnerCount){
    j = 0;
    /* Then we need to loop through the attached ResourceNodes and draw them */
    while(j < gameObjectData->resourceNodeSpawners[i].maximumNodeCount){
      if(gameObjectData->resourceNodeSpawners[i].resourceNodes[j].alive &&
         (arguments->floats[0] >= getDistance2BetweenRects(programmableWorker->rect,gameObjectData->resourceNodeSpawners[i].resourceNodes[j].rect))){
        count++;
      }
      j++;
    }
    i++;
  }
  if(count >= arguments->integers[0]){
    return 1;
  }
  return 2;
}

int blockFunction_RememberCurrentLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  programmableWorker->brain.remembered_point.x = programmableWorker->rect.x;
  programmableWorker->brain.remembered_point.y = programmableWorker->rect.y;
  programmableWorker->brain.is_point_remembered = 1;
  return 1;
}

int blockFunction_GoToStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
    SDL_Point p = getCenterOfRect(programmableWorker->rect);
    if(!programmableWorker->brain.is_point_remembered){
      return 2;
    }
    programmableWorker->heading = atan2((double)(programmableWorker->brain.remembered_point.x - p.x),
                                        (double)(programmableWorker->brain.remembered_point.y - p.y));
    programmableWorker->status = LEAVING;
    return 1;
}

int blockFunction_IfWithinDistanceOfStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  double d2;
  if(!programmableWorker->brain.is_point_remembered){
    return 2;
  }
  d2 = getDistance2BetweenPoints(programmableWorker->rect.x,programmableWorker->rect.y,
                                 programmableWorker->brain.remembered_point.x,programmableWorker->brain.remembered_point.y);
  if(d2 <= arguments->floats[0]){
    return 1;
  }
  return 2;
}

int blockFunction_ForgetStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  programmableWorker->brain.is_point_remembered = 0;
  return 1;
}

int blockFunction_RandomShiftStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  int x_shift = rand() % arguments->integers[0];
  int y_shift = rand() % arguments->integers[0];

  x_shift -= arguments->integers[0];
  y_shift -= arguments->integers[0];
  programmableWorker->brain.remembered_point.x += x_shift;
  programmableWorker->brain.remembered_point.y += y_shift;
  return 1;
}

int blockFunction_IfNodeFound(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(programmableWorker->brain.foundNode != NULL){
    return 1;
  }
  return 2;
}

int blockFunction_HeadToFoundNode(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(programmableWorker->brain.foundNode != NULL){
    programmableWorker->heading = getAngleBetweenRects(&programmableWorker->brain.foundNode->rect,&programmableWorker->rect);
    programmableWorker->status = LEAVING;
  }
  return 1;
}

int blockFunction_HasStoredLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  if(programmableWorker->brain.is_point_remembered){
    return 1;
  }
  return 2;
}

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  int i;
  resetBlockFunctionGlobals(&blockFunctionRoot->globals);
  BlockFunction *blockFunction = &(blockFunctionRoot->blockFunctions[programmableWorker->brain.aiStartPoint]);
  if(blockFunctionRoot->numOfBlockFunctions == 0){
	  return;
  }
  while(1){
    i = runBlockFunctionOverWorker(blockFunction, &blockFunctionRoot->globals, programmableWorker,gameObjectData, ticks);
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

int runBlockFunctionOverWorker(BlockFunction *blockFunction, BlockFunctionGlobals *globals, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks){
  int i;
  i = blockFunction->wrapped_function(globals,
                                     &(blockFunction->arguments),
                                     programmableWorker,
                                     gameObjectData,
                                     ticks);
  return(i);
}

blockFunction_WrappedFunction getBlockFunctionByName(char *blockFunctionName){
  if(strcmp(blockFunctionName,"IfCargoGreaterThan") == 0){
    return &blockFunction_IfCargoGreaterThan;
  }
  if(strcmp(blockFunctionName,"IfStatusEqual") == 0){
    return &blockFunction_IfStatusEqual;
  }
  if(strcmp(blockFunctionName,"IfOutsideBounds") == 0){
    return &blockFunction_IfOutsideOfBounds;
  }
  if(strcmp(blockFunctionName,"IfWithinXOfHive") == 0){
    return &blockFunction_IfWithinDistanceOfHive;
  }
  if(strcmp(blockFunctionName,"SetHeadingRandomly") == 0){
    return &blockFunction_SetHeadingRandomly;
  }
  if(strcmp(blockFunctionName,"ReturnToHive") == 0){
    return &blockFunction_ReturnToHive;
  }
  if(strcmp(blockFunctionName,"IfNumOfFlowersInRadius") == 0){
    return &blockFunction_IfNumOfFlowersInRadius;
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
  if(strcmp(blockFunctionName,"IfWithinDistanceOfStoredLocation") == 0){
    return &blockFunction_IfWithinDistanceOfStoredLocation;
  }
  if(strcmp(blockFunctionName, "RandomiseStoredLocation") == 0){
    return &blockFunction_RandomShiftStoredLocation;
  }
  if(strcmp(blockFunctionName, "IfNodeFound") == 0){
    return &blockFunction_IfNodeFound;
  }
  if(strcmp(blockFunctionName, "HeadToFoundNode") == 0){
    return &blockFunction_HeadToFoundNode;
  }
  if(strcmp(blockFunctionName, "IfNearHive") == 0){
	  return &blockFunction_IfNearHive;
  }
  if(strcmp(blockFunctionName, "START") == 0){
	  return &blockFunction_Void;
  }
  if(strcmp(blockFunctionName,"IfIdle") == 0){
	  return &blockFunction_IfIdle;
  }
  if(strcmp(blockFunctionName,"IfHasCargo") == 0){
	  return &blockFunction_IfHasCargo;
  }
  if(strcmp(blockFunctionName,"IfReturning") == 0){
	  return &blockFunction_IfReturning;
  }
  if(strcmp(blockFunctionName,"IfNearOtherWorker") == 0){
    return &blockFunction_IfNearOtherWorker;
  }
  if(strcmp(blockFunctionName,"OneInAHundred") == 0){
    return &blockFunction_OneInAHundred;
  }
  if(strcmp(blockFunctionName,"CoinFlip") == 0){
    return &blockFunction_CoinFlip;
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
  if(strcmp(blockFunctionName,"IfGreaterThanSaved") == 0){
    return &blockFunction_IfGreaterThanSaved;
  }
  if(strcmp(blockFunctionName,"CopyPointFromSelected") == 0){
    return &blockFunction_CopyPointFromSelected;
  }
  if(strcmp(blockFunctionName,"CountNearFlowers") == 0){
    return &blockFunction_CountNearFlowers;
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
	float read_float;
	char read_char;
	int read_position;
	int read_add;
	int read_count;
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
      printf("making blockFunction: %s\n",blockFunction.name);
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
			while((sscanf(&tokensToUse[i][read_position],"%d%n",&read_int,&read_add)) != 0 && read_add != 0){
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
			while((sscanf(&tokensToUse[i][read_position],"%f%n",&read_float,&read_add)) != 0 && read_add != 0){
				read_position += read_add+1;
				blockFunction.arguments.numOfFloats++;
				blockFunction.arguments.floats = realloc(blockFunction.arguments.floats,
				                                         blockFunction.arguments.numOfFloats * sizeof(float));
				blockFunction.arguments.floats[blockFunction.arguments.numOfFloats-1] = read_float;
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
		if(root->blockFunctions[i].arguments.numOfInts != 0){
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

static void makeAIBlockTemplate(AIData *aiData, char name[50], int numOfArguments, BlockFunctionArgumentType *arguments){
  BlockFunctionTemplate *template = malloc(sizeof(BlockFunctionTemplate));
  BlockFunctionTemplate *previous = aiData->templates;
  int i = 0;
  strcpy(template->name,name);
  if(numOfArguments != 0){
    template->arguments = malloc(sizeof(BlockFunctionArgumentType) * numOfArguments);
    template->numOfArguments = numOfArguments;
    while(i < numOfArguments){
      template->arguments[i] = arguments[i];
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
}

AIData initAIData(void){
	AIData aiData;
	FILE *file;
	int i = 0;
	BlockFunction *erroneousBlockFunction = NULL;
    BlockFunctionArgumentType arguments[2] = {BF_PRIMARY,BF_SECONDARY};
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
  aiData.templates = NULL;
	makeAIBlockTemplate(&aiData,"Void",1,arguments);
	makeAIBlockTemplate(&aiData,"IfIdle",2,arguments);
	makeAIBlockTemplate(&aiData,"IfReturning",2,arguments);
	makeAIBlockTemplate(&aiData,"IfHasCargo",2,arguments);
	makeAIBlockTemplate(&aiData,"IfOutsideBounds",2,arguments);
	makeAIBlockTemplate(&aiData,"IfNearHive",2,arguments);
	makeAIBlockTemplate(&aiData,"SetHeadingRandomly",1,arguments);
	makeAIBlockTemplate(&aiData,"ReturnToHive",1,arguments);
	makeAIBlockTemplate(&aiData,"RememberCurrentLocation",1,arguments);
	makeAIBlockTemplate(&aiData,"GoToStoredLocation",2,arguments);
	makeAIBlockTemplate(&aiData,"ForgetStoredLocation",1,arguments);
	makeAIBlockTemplate(&aiData,"IfNodeFound",2,arguments);
	makeAIBlockTemplate(&aiData,"HeadToFoundNode",1,arguments);
	makeAIBlockTemplate(&aiData,"HasStoredLocation",2,arguments);
  makeAIBlockTemplate(&aiData,"IfNearOtherWorker",2,arguments);
  makeAIBlockTemplate(&aiData,"CoinFlip",2,arguments);
  makeAIBlockTemplate(&aiData,"OneInAHundred",2,arguments);
  makeAIBlockTemplate(&aiData,"Sleep",1,arguments);
  makeAIBlockTemplate(&aiData,"CountNearWorkers",1,arguments);
  makeAIBlockTemplate(&aiData,"IfCountZero",2,arguments);
  makeAIBlockTemplate(&aiData,"LoadCount",1,arguments);
  makeAIBlockTemplate(&aiData,"SaveCount",1,arguments);
  makeAIBlockTemplate(&aiData,"LoadCountFromOther",1,arguments);
  makeAIBlockTemplate(&aiData,"SaveCountFromOther",1,arguments);
  makeAIBlockTemplate(&aiData,"PickNearbyWorker",2,arguments);
  makeAIBlockTemplate(&aiData,"IfGreaterThanSaved",2,arguments);
  makeAIBlockTemplate(&aiData,"CopyPointFromSelected",1,arguments);
  makeAIBlockTemplate(&aiData,"CountNearFlowers",1,arguments);

	return aiData;
}

static void resetBlockFunctionGlobals(BlockFunctionGlobals *globals){
  globals->count = 0;
  globals->selectedWorker = NULL;
}
