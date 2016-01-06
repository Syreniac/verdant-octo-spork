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

int blockFunction_IfWorkerIdle(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	if(programmableWorker->status == IDLE){
		return 1;
	}
	return 2;
}

int blockFunction_IfWorkerReturning(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	if(programmableWorker->status == RETURNING){
		return 1;
	}
	return 2;
}

int blockFunction_IfWorkerHasCargo(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	if(programmableWorker->cargo > 0){
		return 1;
	}
	return 2;
}

int blockFunction_Print(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i = 0;
  printf("PRINTING OUT BLOCK FUNCTION DATA\n");
  while(i < arguments->numOfChars){
      printf("%c",arguments->characters[i]);
      i++;
  }
  printf("\nINTEGERS\n");
  i = 0;
  while(i < arguments->numOfInts){
    printf("%d ",arguments->integers[i]);
    i++;
  }
  printf("\nFLOATS\n");
  i=0;
  while(i<arguments->numOfFloats){
    printf("%f ",arguments->floats[i]);
    i++;
  }
  printf("------------------");
  return(1);
}

int blockFunction_IfWorkerCargoGreaterThan(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->cargo > arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfWorkerStatusEqual(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  if(programmableWorker->status == arguments->integers[0]){
    return(1);
  }
  return(2);
}

int blockFunction_IfWorkerOutsideOfBounds(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){

  if(programmableWorker->rect.x >= (X_SIZE_OF_WORLD - programmableWorker->rect.w) || programmableWorker->rect.x <= 0 ||
     programmableWorker->rect.y >= (Y_SIZE_OF_WORLD - programmableWorker->rect.h) || programmableWorker->rect.y <= 0){
       return(1);
    }
    return(2);
}

int blockFunction_IfWorkerWithinDistanceOfHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(d2 <= (double)(arguments->floats[0])){
    return(1);
  }
  return(2);
}

int blockFunction_IfWorkerNearHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	double d2 = getDistance2BetweenRects(programmableWorker->rect,gameObjectData->hive.rect);
  if(d2 <= 50.0){
    return(1);
  }
  return(2);

}

int blockFunction_SetWorkerHeadingRandomly(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->heading = (double)(randPi() * 2);
  programmableWorker->status = LEAVING;
  return(1);
}


int blockFunction_WorkerReturnToHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_GoToRememberedLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
    if(!programmableWorker->brain.is_point_remembered){
      return 2;
    }
    programmableWorker->heading = atan2((double)(programmableWorker->brain.remembered_point.x - programmableWorker->rect.x),
                                        (double)(programmableWorker->brain.remembered_point.y - programmableWorker->rect.y));
    programmableWorker->status = LEAVING;
    return 1;
}

int blockFunction_IfWorkerWithinDistanceOfRememberedLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_ForgetRememberedLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  programmableWorker->brain.is_point_remembered = 0;
  return 1;
}

int blockFunction_RandomShiftRememberedLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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

int blockFunction_HasRememberedLocation(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
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
  if(strcmp(blockFunctionName,"IfWorkerCargoGreaterThan") == 0){
    return &blockFunction_IfWorkerCargoGreaterThan;
  }
  if(strcmp(blockFunctionName,"IfWorkerStatusEqual") == 0){
    return &blockFunction_IfWorkerStatusEqual;
  }
  if(strcmp(blockFunctionName,"IfWorkerOutsideBounds") == 0){
    return &blockFunction_IfWorkerOutsideOfBounds;
  }
  if(strcmp(blockFunctionName,"IfWorkerWithinXOfHive") == 0){
    return &blockFunction_IfWorkerWithinDistanceOfHive;
  }
  if(strcmp(blockFunctionName,"SetWorkerHeadingRandomly") == 0){
    return &blockFunction_SetWorkerHeadingRandomly;
  }
  if(strcmp(blockFunctionName,"WorkerReturnToHive") == 0){
    return &blockFunction_WorkerReturnToHive;
  }
  if(strcmp(blockFunctionName,"IfNumOfFlowersInRadius") == 0){
    return &blockFunction_IfNumOfFlowersInRadius;
  }
  if(strcmp(blockFunctionName,"RememberCurrentLocation") == 0){
    return &blockFunction_RememberCurrentLocation;
  }
  if(strcmp(blockFunctionName,"ForgetRememberedLocation") == 0){
    return &blockFunction_ForgetRememberedLocation;
  }
  if(strcmp(blockFunctionName,"HasRememberedLocation") == 0){
    return &blockFunction_HasRememberedLocation;
  }
  if(strcmp(blockFunctionName,"GoToRememberedLocation") == 0){
    return &blockFunction_GoToRememberedLocation;
  }
  if(strcmp(blockFunctionName,"IfWorkerWithinDistanceOfRememberedLocation") == 0){
    return &blockFunction_IfWorkerWithinDistanceOfRememberedLocation;
  }
  if(strcmp(blockFunctionName, "RandomiseRememberedLocation") == 0){
    return &blockFunction_RandomShiftRememberedLocation;
  }
  if(strcmp(blockFunctionName, "IfNodeFound") == 0){
    return &blockFunction_IfNodeFound;
  }
  if(strcmp(blockFunctionName, "HeadToFoundNode") == 0){
    return &blockFunction_HeadToFoundNode;
  }
  if(strcmp(blockFunctionName, "IfWorkerNearHive") == 0){
	  return &blockFunction_IfWorkerNearHive;
  }
  if(strcmp(blockFunctionName, "START") == 0){
	  return &blockFunction_Void;
  }
  if(strcmp(blockFunctionName,"IfWorkerIdle") == 0){
	  return &blockFunction_IfWorkerIdle;
  }
  if(strcmp(blockFunctionName,"IfWorkerHasCargo") == 0){
	  return &blockFunction_IfWorkerHasCargo;
  }
  if(strcmp(blockFunctionName,"IfWorkerReturning") == 0){
	  return &blockFunction_IfWorkerReturning;
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
  BlockFunctionArgumentType arguments[2] = {BF_PRIMARY,BF_SECONDARY};
	file = fopenAndVerify("GenericWorkerAI.txt","r");
	aiData.blockFunctionRoots = calloc(1,sizeof(BlockFunctionRoot));
	aiData.blockFunctionRoots[0] = makeBlockFunctionRootFromFile(file);
  aiData.templates = NULL;
	fclose(file);
  makeAIBlockTemplate(&aiData,"Void",1,arguments);
  makeAIBlockTemplate(&aiData,"IfWorkerIdle",2,arguments);
  makeAIBlockTemplate(&aiData,"IfWorkerReturning",2,arguments);
  makeAIBlockTemplate(&aiData,"IfWorkerHasCargo",2,arguments);
  makeAIBlockTemplate(&aiData,"IfWorkerOutsideBounds",2,arguments);
  makeAIBlockTemplate(&aiData,"IfWorkerNearHive",2,arguments);
  makeAIBlockTemplate(&aiData,"SetWorkerHeadingRandomly",1,arguments);
  makeAIBlockTemplate(&aiData,"WorkerReturnToHive",1,arguments);
  makeAIBlockTemplate(&aiData,"RememberCurrentLocation",1,arguments);
  makeAIBlockTemplate(&aiData,"GoToRememberedLocation",2,arguments);
  makeAIBlockTemplate(&aiData,"ForgetRememberedLocation",1,arguments);
  makeAIBlockTemplate(&aiData,"IfNodeFound",2,arguments);
  makeAIBlockTemplate(&aiData,"HeadToFoundNode",1,arguments);
  makeAIBlockTemplate(&aiData,"HasRememberedLocation",2,arguments);

	return aiData;
}
