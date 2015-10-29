#ifndef _GENERIC_INCLUDED
#include "generic.h"
#endif
#ifndef _GAME_OBJECTS_INCLUDED
#include "game_objects.h"
#endif

#define _AI_INCLUDED 1

typedef struct BlockFunctionArgs BlockFunctionArgs;
typedef struct BlockFunction BlockFunction;
typedef struct BlockFunctionRoot BlockFunctionRoot;

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


struct BlockFunctionRoot{
  BlockFunction *blockFunctions;
  int numOfBlockFunctions;
};

struct BlockFunction{
  char *name;
  blockFunction_WrappedFunction wrapped_function;
  BlockFunction *primary;
  BlockFunction *secondary;
  BlockFunctionArgs arguments;
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

int runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot,
                                   ProgrammableWorker *programmbleWorker,
                                   GameObjectData *gameObjectData);

int runBlockFunctionOverWorker(BlockFunction *blockFunction,
                               ProgrammableWorker *programmableWorker,
                               GameObjectData *gameObjectData);

BlockFunctionRoot generateGenericWorkerOrders(void);
