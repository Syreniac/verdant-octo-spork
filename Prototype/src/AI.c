#include "AI.h"

/* All blockFunctions must:
  - Return either integers between 0 and 2.
  - Have accomodations for resetting any arguments adjusted in the course of the
    function.
  - Take the same arguments to ensure pointer compatibility */

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

  if(programmableWorker->rect.x > (X_SIZE_OF_WORLD - programmableWorker->rect.w) || programmableWorker->rect.x <= 0 ||
     programmableWorker->rect.y > (Y_SIZE_OF_WORLD - programmableWorker->rect.h) || programmableWorker->rect.y <= 0){
       printf("worker outside of bounds\n");
       return(1);
    }
    return(2);
}

int blockFunction_IfWorkerWithinDistanceOfHive(BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
	double d2 = getDistance2BetweenPoints((float)(programmableWorker->rect.x + programmableWorker->rect.w/2),
                                       (float)(programmableWorker->rect.y + programmableWorker->rect.h/2),
                                       (float)(gameObjectData->hive.rect.x + gameObjectData->hive.rect.w/2),
                                       (float)(gameObjectData->hive.rect.y + gameObjectData->hive.rect.h/2));
	printf("AI bounce %f v %f\n",arguments->floats[0],d2);
  if(d2 <= (double)(arguments->floats[0])){
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
  programmableWorker->heading = atan2((double)(gameObjectData->hive.rect.x + gameObjectData->hive.rect.w/2 - programmableWorker->rect.w/2 - programmableWorker->rect.x),
                                      (double)(gameObjectData->hive.rect.y + gameObjectData->hive.rect.h/2 - programmableWorker->rect.h/2 - programmableWorker->rect.y));
  programmableWorker->status = RETURNING;
  return(1);
}

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData){
  int i;
  BlockFunction *blockFunction = &(blockFunctionRoot->blockFunctions[0]);
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
  strcpy(blockFunction1.name,"blockFunction_IfWorkerStatusEqual");
  blockFunction1.wrapped_function = &blockFunction_IfWorkerStatusEqual;
  blockFunctionArgs1.integers = calloc(1, sizeof(int));
  blockFunctionArgs1.integers[0] = 2;
  blockFunctionArgs1.numOfInts = 1;
  blockFunction1.arguments = blockFunctionArgs1;

  /* This is the "within x distance of the hive part." */
  strcpy(blockFunction2.name,"blockFunction_IfWorkerWithinDistanceOfHive");
  blockFunction2.wrapped_function = &blockFunction_IfWorkerWithinDistanceOfHive;
  blockFunctionArgs2.floats = calloc(1, sizeof(float));
  blockFunctionArgs2.floats[0] = 1.0;
  blockFunctionArgs2.numOfFloats = 1;
  blockFunction2.arguments = blockFunctionArgs2;

  /* This is the "then randomly move away from the hive" part */
  strcpy(blockFunction3.name,"blockFunction_SetWorkerHeadingRandomly");
  blockFunction3.wrapped_function = &blockFunction_SetWorkerHeadingRandomly;
  blockFunction3.arguments = blockFunctionArgs3;
  /* Putting the primary link to NULL will break the AI recursion
     after this function */

  /* This is the "check to see if the bee picked anything up part" */
  strcpy(blockFunction4.name,"blockFunction_IfWorkerCargoGreaterThan");
  blockFunction4.wrapped_function = &blockFunction_IfWorkerCargoGreaterThan;
  blockFunctionArgs4.integers = calloc(1,sizeof(int));
  blockFunctionArgs4.integers[0] = 0;
  blockFunctionArgs4.numOfInts = 1;
  blockFunction4.arguments = blockFunctionArgs4;

  /* This is "return to the hive" */
  strcpy(blockFunction5.name,"blockFunction_WorkerReturnToHive");
  blockFunction5.wrapped_function = &blockFunction_WorkerReturnToHive;
  blockFunction5.arguments = blockFunctionArgs5;
  /* Set the primary to NULL to break the recursion */

  /* This is "check whether the bee is in the bounds or not" */
  strcpy(blockFunction6.name, "blockFunction_IfWorkerOutsideOfBounds");
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

blockFunction_WrappedFunction getBlockFunctionByName(char *blockFunctionName){
  if(strcmp(blockFunctionName,"BlockFunction_IfWorkerCargoGreaterThan") == 0){
    return &blockFunction_IfWorkerCargoGreaterThan;
  }
  if(strcmp(blockFunctionName,"BlockFunction_IfWorkerStatusEqual") == 0){
    return &blockFunction_IfWorkerStatusEqual;
  }
  if(strcmp(blockFunctionName,"BlockFunction_IfWorkerOutsideOfBounds") == 0){
    return &blockFunction_IfWorkerOutsideOfBounds;
  }
  if(strcmp(blockFunctionName,"BlockFunction_IfWorkerWithinDistanceOfHive") == 0){
    return &blockFunction_IfWorkerWithinDistanceOfHive;
  }
  if(strcmp(blockFunctionName,"BlockFunction_SetWorkerHeadingRandomly") == 0){
    return &blockFunction_SetWorkerHeadingRandomly;
  }
  if(strcmp(blockFunctionName,"BlockFunction_WorkerReturnToHive") == 0){
    return &blockFunction_WorkerReturnToHive;
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

void makeBlockFunctionRootFromFile(BlockFunctionRoot *blockFunctionRoot, FILE *file){
  int blockFunctionIndex = 0;
  int stringWhiteSpaceShift = 0;
  char read_line[255];
  char *read_line2;
  int *integers;
  int intReadOffset = 0;
  int readInt;
  int presumedNumOfInts;
  int numOfInts;
  float *floats;
  int floatReadOffset = 0;
  float readFloat;
  int presumedNumOfFloats;
  int numOfFloats;
  int maxDescLength;
  int primaryRef = -1;
  int secondaryRef = -1;
  int initialRun = 1;
  int functionBlockCount = getNumberOfTextStoredBlocks(file,&maxDescLength);
  char name[255];
  blockFunction_WrappedFunction wrapped_function;

  printf("number of function blocks in file = %d\n",functionBlockCount);

  fseek(file,0,SEEK_SET);

  blockFunctionRoot->blockFunctions = malloc(functionBlockCount * sizeof(BlockFunction));

  while(fgets(read_line,255,file) != NULL){
    /* Through magic, this removes the \n character from the read_line */
    /* It does work, I've tested it */
    /* Also, every thing coded after this comment was done whilst I was drunk */
    read_line2 = strtok(read_line,"\n");
    stringWhiteSpaceShift = 0;
    /* Work out whether we need to move onto the next block */
    if(read_line2[0] != '\t' && read_line2[0] != ' '){
      if(initialRun == 0){
        strcpy(blockFunctionRoot->blockFunctions[blockFunctionIndex].name, name);
        blockFunctionRoot->blockFunctions[blockFunctionIndex].wrapped_function = wrapped_function;
        if(primaryRef != -1){
          blockFunctionRoot->blockFunctions[blockFunctionIndex].primary = &blockFunctionRoot->blockFunctions[primaryRef-1];
        }
        else{
          blockFunctionRoot->blockFunctions[blockFunctionIndex].primary = NULL;
        }
        if(secondaryRef != -1){
          blockFunctionRoot->blockFunctions[blockFunctionIndex].secondary = &blockFunctionRoot->blockFunctions[secondaryRef-1];
        }
        else{
          blockFunctionRoot->blockFunctions[blockFunctionIndex].secondary = NULL;
        }
        blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.floats = &floats[0];
        blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.numOfFloats = numOfFloats;
        blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.integers = &integers[0];
        blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.numOfInts = numOfInts;
        blockFunctionIndex++;
        primaryRef = -1;
        secondaryRef = -1;
        integers = NULL;
        floats = NULL;
      }
      initialRun = 0;
      /* If we do, then we need to work with the new name to get the wrapped
         function */
      wrapped_function = getBlockFunctionByName(read_line2);
      strcpy(name,read_line2);


    }
    /* Else trim out whitespace */
    else{
      while(read_line[stringWhiteSpaceShift] == '\t' || read_line[stringWhiteSpaceShift] == ' '){
        stringWhiteSpaceShift++;
      }

      /* Now work out whether what kind of secondary line we are looking at (integers,
         floats, primary/secondary BlockFunctions) */
      /* strncmp is a variation on strcmp which only compares a specific number
         of characters from a string to another. Combining this with some pointer
         magic and we can read only the bits which are relevant */
      /* The pointer magic works by tricking the program into treating the string
         referred to by the pointer as starting at the address of the index given */
      /* DON'T ASK ME FOR A BETTER EXPLANATION, I'M DRUNK */
      if(strncmp(&(read_line[stringWhiteSpaceShift]),"primary =",9) == 0){
        sscanf(&(read_line[stringWhiteSpaceShift]),"primary = %d",&primaryRef);
      }
      else if(strncmp(&(read_line[stringWhiteSpaceShift]),"secondary =", 11) == 0){
        sscanf(&(read_line[stringWhiteSpaceShift]),"secondary = %d",&secondaryRef);
      }
      else if(strncmp(&(read_line[stringWhiteSpaceShift]),"integers =", 10) == 0){
        presumedNumOfInts = countCharsInString(&(read_line[stringWhiteSpaceShift]),',') + 1;
        integers = calloc((size_t)presumedNumOfInts,sizeof(int));
        intReadOffset = 0;
        numOfInts = 0;
        while(sscanf(&(read_line[stringWhiteSpaceShift+intReadOffset+11]),"%d",&readInt) > 0 && numOfInts < presumedNumOfInts){
          /* Add some offset until we find the next comma */
          integers[numOfInts] = readInt;
          numOfInts++;
          while(presumedNumOfInts > numOfInts && read_line[stringWhiteSpaceShift+intReadOffset+11] != ','){
            intReadOffset++;
          }
          intReadOffset++;
          /* increament the numofInts */
        }
      }
      else if(strncmp(&(read_line[stringWhiteSpaceShift]),"floats =", 8) == 0){
        presumedNumOfFloats = countCharsInString(&(read_line[stringWhiteSpaceShift]),',') + 1;
        floats = calloc((size_t)presumedNumOfFloats,sizeof(float));
        floatReadOffset = 0;
        numOfFloats = 0;
        while(sscanf(&(read_line[stringWhiteSpaceShift+floatReadOffset+9]),"%f",&readFloat) > 0 && numOfFloats < presumedNumOfFloats){
          /* Add some offset until we find the next comma */
          floats[numOfFloats] = readFloat;
          numOfFloats++;
          while(presumedNumOfFloats > numOfFloats && read_line[stringWhiteSpaceShift+floatReadOffset+9] != ','){
            floatReadOffset++;
          }
          floatReadOffset++;
        }
      }
    }
  }
    strcpy(blockFunctionRoot->blockFunctions[blockFunctionIndex].name, name);
    blockFunctionRoot->blockFunctions[blockFunctionIndex].wrapped_function = wrapped_function;
    if(primaryRef != -1){
      blockFunctionRoot->blockFunctions[blockFunctionIndex].primary = &blockFunctionRoot->blockFunctions[primaryRef-1];
    }
    else{
      blockFunctionRoot->blockFunctions[blockFunctionIndex].primary = NULL;
    }
    if(secondaryRef != -1){
      blockFunctionRoot->blockFunctions[blockFunctionIndex].secondary = &blockFunctionRoot->blockFunctions[secondaryRef-1];
    }
    else{
      blockFunctionRoot->blockFunctions[blockFunctionIndex].secondary = NULL;
    }
    blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.floats = floats;
    blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.numOfFloats = numOfFloats;
    blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.integers = integers;
    blockFunctionRoot->blockFunctions[blockFunctionIndex].arguments.numOfInts = numOfInts;
}

void runAI(AIData *aiData, GameObjectData *gameObjectData){
  /* AIData *aiData                 = the pointer to the AIData struct that
                                      holds all the AI data we need to run it
     GameObjectData *gameObjectData = the pointer to the GameObjectData struct
                                      that holds all the data we need to find
                                      GameObjects and get the context for the AI

     This function is where we run the AI over all the ProgrammableWorkers. */
  int i = 0;
  while(i < gameObjectData->programmableWorkerCount){
    runBlockFunctionRootOverWorker(&aiData->blockFunctionRoots[0],
                                   &gameObjectData->programmableWorkers[i],
                                   gameObjectData);
    i++;
  }
}
