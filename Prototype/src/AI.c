#include "AI.h"

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

int blockFunction_Void(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	return(1);
}

int blockFunction_IfIdle(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	if(programmableWorker->status == IDLE){
		return 1;
	}
	return 2;
}

int blockFunction_IfReturning(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	if(programmableWorker->status == RETURNING){
		return 1;
	}
	return 2;
}

int blockFunction_IfHasCargo(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	if(programmableWorker->cargo > 0){
		return 1;
	}
	return 2;
}

int blockFunction_Print(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i = 0;
 /* printf("PRINTING OUT BLOCK FUNCTION DATA\n");*/
  while(i < arguments->numOfChars){
      printf("%c",arguments->characters[i]);
      i++;
  }
 /* printf("\nINTEGERS\n");*/
  i = 0;
  while(i < arguments->numOfInts){
    printf("%d ",arguments->integers[i]);
    i++;
  }
  /*printf("\nFLOATS\n");*/
  i=0;
  while(i<arguments->numOfFloats){
    printf("%f ",arguments->floats[i]);
    i++;
  }
 /* printf("------------------");*/
  return(1);
}

int blockFunction_IfCargoGreaterThan(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->cargo > arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfStatusEqual(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->status == arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfOutsideOfBounds(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){

  if(programmableWorker->rect.x >= (X_SIZE_OF_WORLD - programmableWorker->rect.w) || programmableWorker->rect.x <= 0 ||
     programmableWorker->rect.y >= (Y_SIZE_OF_WORLD - programmableWorker->rect.h) || programmableWorker->rect.y <= 0){
       return(1);
    }
    return(2);
}

int blockFunction_IfWithinDistanceOfHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(d2 <= (double)(arguments->floats[0])){
    return(1);
  }
  return(2);
}

int blockFunction_IfNearHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(d2 <= 50.0){
    return(1);
  }
  return(2);

}

int blockFunction_SetHeadingRandomly(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->heading = (double)(randPi() * 2);
  programmableWorker->status = LEAVING;
  return(1);
}


int blockFunction_ReturnToHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->heading = getAngleBetweenRects(&gameObjectData->hive.rect,&programmableWorker->rect);
  programmableWorker->status = RETURNING;
  return(1);
}

int blockFunction_IfNumOfFlowersInRadius(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_RememberCurrentLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->brain.remembered_point.x = programmableWorker->rect.x;
  programmableWorker->brain.remembered_point.y = programmableWorker->rect.y;
  programmableWorker->brain.is_point_remembered = 1;
  return 1;
}

int blockFunction_GoToStoredLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
    SDL_Point p = getCenterOfRect(programmableWorker->rect);
    if(!programmableWorker->brain.is_point_remembered){
      return 2;
    }
    programmableWorker->heading = atan2((double)(programmableWorker->brain.remembered_point.x - p.x),
                                        (double)(programmableWorker->brain.remembered_point.y - p.y));
    programmableWorker->status = LEAVING;
    return 1;
}

int blockFunction_IfWithinDistanceOfStoredLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_ForgetStoredLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->brain.is_point_remembered = 0;
  return 1;
}

int blockFunction_RandomShiftStoredLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int x_shift = rand() % arguments->integers[0];
  int y_shift = rand() % arguments->integers[0];

  x_shift -= arguments->integers[0];
  y_shift -= arguments->integers[0];
  programmableWorker->brain.remembered_point.x += x_shift;
  programmableWorker->brain.remembered_point.y += y_shift;
  return 1;
}

int blockFunction_IfNodeFound(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->brain.foundNode != NULL){
    return 1;
  }
  return 2;
}

int blockFunction_HeadToFoundNode(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->brain.foundNode != NULL){
    programmableWorker->heading = getAngleBetweenRects(&programmableWorker->brain.foundNode->rect,&programmableWorker->rect);
    programmableWorker->status = LEAVING;
  }
  return 1;
}

int blockFunction_HasStoredLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->brain.is_point_remembered){
    return 1;
  }
  return 2;
}

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i;
  BlockFunction *blockFunction = &(blockFunctionRoot->blockFunctions[0]);
  if(blockFunctionRoot->numOfBlockFunctions == 0){
	  return;
  }
  while(1){
    i = runBlockFunctionOverWorker(blockFunction, programmableWorker,gameObjectData);
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

int runBlockFunctionOverWorker(BlockFunction *blockFunction, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i;
  i = blockFunction->wrapped_function(&(blockFunction->arguments),
                                     programmableWorker,
                                     gameObjectData);
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
  printf("ERROR: Unrecognised function name: \"%s\".\n Substituting a print function.\n",blockFunctionName);
  return &blockFunction_Print;
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
			i = 0;
		}
		tokensToUse[i] = tok;
		tok = strtok(NULL,"\n");
	}
	if(i > 0 || blockFunctionRoot.numOfBlockFunctions == numOfBlocks - 1){
		if(blockFunctionRoot.numOfBlockFunctions == numOfBlocks - 1){
			i++;
		}
		blockFunctionRoot.numOfBlockFunctions++;
		blockFunctionRoot.blockFunctions[blockFunctionRoot.numOfBlockFunctions-1] = createAIBlockFunctionFromTokens(&blockFunctionRoot,
																													i,tokensToUse);
	}
	/*i = 0;
	while(i < blockFunctionRoot.numOfBlockFunctions){
		printf("%s\n",blockFunctionRoot.blockFunctions[i].name);
		printf("%p\n",&blockFunctionRoot.blockFunctions[i]);
		i++;
	}*/
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

void nullifyBlockFunctionRoot(BlockFunctionRoot *root){
	int i = 0;
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
	free(root->blockFunctions);
	root->blockFunctions = NULL;
	root->numOfBlockFunctions = 0;
}

void runAI(AIData *aiData, GameObjectData *gameObjectData){
  /* AIData *aiData                 = the pointer to the AIData struct that
                                      holds all the AI data we need to run it
     GameObjectData *gameObjectData = the pointer to the GameObjectData struct
                                      that holds all the data we need to find
                                      GameObjects and get the context for the AI

     This function is where we run the AI over all the ProgrammableWorkers. */
  ProgrammableWorker *programmableWorker;
  for(programmableWorker = gameObjectData -> first_programmable_worker; programmableWorker != NULL; programmableWorker = programmableWorker->next){
    runBlockFunctionRootOverWorker(&aiData->blockFunctionRoots[0],
                                   programmableWorker,
                                   gameObjectData);
  }
}

BlockFunction *testBlockFunctionRootForLoops(BlockFunction *toTest, BlockFunction **testAgainst, int countToTestAgainst){
  int i = 0;
  BlockFunction *returnValue = NULL;
  BlockFunction **testAgainstCopy = calloc(countToTestAgainst+2,sizeof(BlockFunction*));
  /* Make a copy of the testAgainst array that we can work with */
  if(testAgainst!=NULL){
    memcpy(testAgainstCopy,testAgainst,sizeof(BlockFunction*) * countToTestAgainst);
  }
  printf("testing block function %s\n",toTest->name);
  /* If the block function we're testing against is in this list, we've found something that loops on itself */
  while(i < countToTestAgainst){
    if(testAgainst[i] == toTest){
      printf("uh, we found a copy\n");
      return toTest;
    }
    i++;
  }
  /* Add the block function we're testing against to the test array and then test the next block functions */
  testAgainstCopy[countToTestAgainst] = toTest;
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
  printf("prepared block template\n");
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
	file = fopenAndVerify("GenericWorkerAI.txt","r");
	aiData.blockFunctionRoots = calloc(1,sizeof(BlockFunctionRoot));
	aiData.blockFunctionRoots[0] = makeBlockFunctionRootFromFile(file);
    aiData.templates = NULL;
	fclose(file);
	erroneousBlockFunction = testBlockFunctionRootForLoops(&aiData.blockFunctionRoots[0].blockFunctions[0],NULL,0);
	if(erroneousBlockFunction != NULL){
		while(erroneousBlockFunction != &aiData.blockFunctionRoots[0].blockFunctions[i]){
		  i++;
		}
		printf("dumping loaded AI File: Loop detected in function %d: \'%s\'",i,erroneousBlockFunction->name);
		nullifyBlockFunctionRoot(&aiData.blockFunctionRoots[0]);
	}
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

	return aiData;
}
