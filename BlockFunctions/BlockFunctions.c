#include <stdio.h>
#include <stdlib.h>


typedef struct BlockFunctionArgs BlockFunctionArgs;
struct BlockFunctionArgs{
  char *characters;
  int numOfChars;
  int *integers;
  int numOfInts;
  float *floats;
  int numOfFloats;
};

typedef int(*blockFunction_WrappedFunction)(BlockFunctionArgs arguments);

typedef struct BlockFunction BlockFunction;
struct BlockFunction{
  blockFunction_WrappedFunction wrapped_function;
  BlockFunction *primary;
  BlockFunction *secondary;
  BlockFunctionArgs arguments;
};


int blockFunction_Print(BlockFunctionArgs arguments);

int blockFunction_IfFirstTwoIntsEqual(BlockFunctionArgs arguments);

int blockFunction_IfFloatWithinLimit(BlockFunctionArgs arguments);

int blockFunction_IfCountEqualToSecondInt(BlockFunctionArgs arguments);

int runBlockFunction(BlockFunction blockFunction);

int main(void){
  BlockFunction blockFunctions[50];
  char *characters;
  int *integer1;
  int *integer2;
  int *integer3;
  float *flo;

  blockFunctions[0].wrapped_function = &blockFunction_Print;
  blockFunctions[0].primary = NULL;
  blockFunctions[0].secondary = NULL;

  characters = "Hello";

  blockFunctions[0].arguments.characters = "Hello";
  blockFunctions[0].arguments.numOfChars = 5;

  runBlockFunction(blockFunctions[0]);
  printf("\n------------------\n");

  blockFunctions[1].wrapped_function = &blockFunction_IfFirstTwoIntsEqual;
  blockFunctions[1].primary = &blockFunctions[0];
  blockFunctions[1].secondary = NULL;

  integer1 = (int *)malloc(256);

  integer1[0] = 5;
  integer1[1] = 6;

  blockFunctions[1].arguments.integers = integer1;
  blockFunctions[1].arguments.numOfInts = 2;

  runBlockFunction(blockFunctions[1]);
  printf("\n------------------\n");

  blockFunctions[2].wrapped_function = &blockFunction_IfFloatWithinLimit;
  blockFunctions[2].primary = &blockFunctions[1];
  blockFunctions[2].secondary = NULL;

  flo = (float *)malloc(256);
  flo[0] = 5.0;
  integer2 = (int *)malloc(256);
  integer2[0] = 6;

  blockFunctions[2].arguments.floats = flo;
  blockFunctions[2].arguments.numOfFloats++;
  blockFunctions[2].arguments.integers = integer2;
  blockFunctions[2].arguments.numOfInts++;

  runBlockFunction(blockFunctions[2]);
  printf("\n------------------\n");

  blockFunctions[3].wrapped_function = &blockFunction_IfCountEqualToSecondInt;
  blockFunctions[3].primary = NULL;
  blockFunctions[3].secondary = &blockFunctions[0];

  integer3 = (int *)malloc(256);
  integer3[0] = 0;
  integer3[0] = 1;

  blockFunctions[3].arguments.integers = integer3;
  blockFunctions[3].arguments.numOfInts = 2;

  blockFunctions[0].primary = &blockFunctions[3];

  runBlockFunction(blockFunctions[3]);



  return(1);
}

int blockFunction_IfCountEqualToSecondInt(BlockFunctionArgs arguments){
  int i = 0;
  printf("blockFunction_IfCountEqualToSecondInt");
  if(arguments.integers[0] == arguments.integers[1]){
    /* Reset the count to 0 */
    arguments.integers[0] = 0;
    return(1);
  }
  arguments.integers[1]++;
  return(2);
}

int blockFunction_Print(BlockFunctionArgs arguments){
  int i = 0;
  printf("blockFunction_Print\n");
  while(i < arguments.numOfChars){
      printf("%c",arguments.characters[i]);
      i++;
  }
  printf("\n");
  return(1);
}

int blockFunction_IfFirstTwoIntsEqual(BlockFunctionArgs arguments){
    int i = 0;
    printf("blockFunction_IfFirstTwoIntsEqual\n");
    if(arguments.integers[0] == arguments.integers[1]){
        return(1);
    }
    return(2);
}

int blockFunction_IfFloatWithinLimit(BlockFunctionArgs arguments){
    int i = 0;
    printf("blockFunction_IfFloatWithinLimit\n");
    if(arguments.floats[0] <= arguments.integers[0]){
        return(1);
    }
    return(2);
}

int runBlockFunction(BlockFunction blockFunction){
  int i;
  printf("\nrunBlockFunction\n");
  i = blockFunction.wrapped_function(blockFunction.arguments);
  if(i == 1){
      if(blockFunction.primary != NULL){
        runBlockFunction(*blockFunction.primary);
      }
      else{
        printf("\nblock wanted to continue but the primary successor was NULL\n");
      }
  }
  else if(i == 2){
      if(blockFunction.secondary != NULL){
        runBlockFunction(*blockFunction.secondary);
      }
      else{
        printf("\nblock wanted to continue but the secondary successor was NULL\n");
      }
  }
  return(0);
}
