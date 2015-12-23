#include "world_generation.h"

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

FILE *fopenAndVerify(char *file_name, char *permission);

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

int blockFunction_IfNumOfFlowersInRadius(BlockFunctionArgs *arguments,
                                     ProgrammableWorker *programmableWorker,
                                     GameObjectData *gameObjectData);

int blockFunction_RememberCurrentLocation(BlockFunctionArgs *arguments,
                                          ProgrammableWorker *programmableWorker,
                                          GameObjectData *gameObjectData);

int blockFunction_GoToRememberedLocation(BlockFunctionArgs *arguments,
                                         ProgrammableWorker *programmableWorker,
                                         GameObjectData *gameObjectData);

int blockFunction_ForgetRememberedLocation(BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData);

int blockFunction_RandomShiftRememberedLocation(BlockFunctionArgs *arguments,
                                                ProgrammableWorker *programmableWorker,
                                                GameObjectData *gameObjectData);

int blockFunction_IfNodeFound(BlockFunctionArgs *arguments,
                              ProgrammableWorker *programmableWorker,
                              GameObjectData *gameObjectData);

int blockFunction_HeadToFoundNode(BlockFunctionArgs *arguments,
                                  ProgrammableWorker *programmableWorker,
                                  GameObjectData *gameObjectData);

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot,
                                   ProgrammableWorker *programmableWorker,
                                   GameObjectData *gameObjectData);

int runBlockFunctionOverWorker(BlockFunction *blockFunction,
                               ProgrammableWorker *programmableWorker,
                               GameObjectData *gameObjectData);
							   
int getNumberOfTextStoredBlocks(FILE *file, int *maxDescLength);

int countCharsInString(char *string, char countChar);

void runAI(AIData *aiData, GameObjectData *gameObjectData);

AIData initAIData(void);

BlockFunction createAIBlockFunctionFromTokens(BlockFunctionRoot *blockFunctionRoot, int numOfLinesToUse, char *tokensToUse[7]);

BlockFunctionRoot makeBlockFunctionRootFromString(char *str, int numOfBlocks);

blockFunction_WrappedFunction getBlockFunctionByName(char *blockFunctionName);
void nullifyBlockFunctionRoot(BlockFunctionRoot *root); 