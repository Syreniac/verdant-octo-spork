#include "world_generation.h"

#define _AI_INCLUDED 1

typedef struct ProgrammableWorker ProgrammableWorker;
typedef struct GameObjectData GameObjectData;

typedef struct BlockFunctionArgs BlockFunctionArgs;
typedef struct BlockFunction BlockFunction;
typedef struct BlockFunctionRoot BlockFunctionRoot;
typedef struct AIData AIData;

typedef int(*blockFunction_WrappedFunction)(BlockFunctionArgs *arguments,
                                            ProgrammableWorker *programmableWorker,
                                            GameObjectData *gameObjectData);

struct BlockFunctionArgs{
  char *characters;
  int numOfChars;
  int *integers;
  int numOfInts;
  float *floats;
  int numOfFloats;
};

struct BlockFunction{
  char name[255];
  blockFunction_WrappedFunction wrapped_function;
  BlockFunction *primary;
  BlockFunction *secondary;
  BlockFunctionArgs arguments;
};


struct BlockFunctionRoot{
  BlockFunction *blockFunctions;
  int numOfBlockFunctions;
};

struct AIData{
  BlockFunctionRoot *blockFunctionRoots;
};


int blockFunction_Print(BlockFunctionArgs *arguments,
                                            ProgrammableWorker *programmableWorker,
                                            GameObjectData *gameObjectData);

int blockFunction_IfWorkerCargoGreaterThan(BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData);

int blockFunction_IfWorkerStatusEqual(BlockFunctionArgs *arguments,
                                      ProgrammableWorker *programmableWorker,
                                      GameObjectData *gameObjectData);

int blockFunction_IfWorkerOutsideOfBounds(BlockFunctionArgs *arguments,
                                          ProgrammableWorker *programmableWorker,
                                          GameObjectData *gameObjectData);

int blockFunction_IfWorkerWithinDistanceOfHive(BlockFunctionArgs *arguments,
                                               ProgrammableWorker *programmableWorker,
                                               GameObjectData *gameObjectData);

int blockFunction_SetWorkerHeadingRandomly(BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData);

int blockFunction_WorkerReturnToHive(BlockFunctionArgs *arguments,
                                     ProgrammableWorker *programmableWorker,
                                     GameObjectData *gameObjectData);

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot,
                                   ProgrammableWorker *programmbleWorker,
                                   GameObjectData *gameObjectData);

int runBlockFunctionOverWorker(BlockFunction *blockFunction,
                               ProgrammableWorker *programmableWorker,
                               GameObjectData *gameObjectData);

BlockFunctionRoot generateGenericWorkerOrders(void);

void makeBlockFunctionRootFromFile(BlockFunctionRoot *blockFunctionRoot, FILE *file);

int getNumberOfTextStoredBlocks(FILE *file, int *maxDescLength);

int countCharsInString(char *string, char countChar);

void runAI(AIData *aiData, GameObjectData *gameObjectData);
