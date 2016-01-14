#include "world_generation.h"


enum BlockFunctionArgumentType{
  BF_WORKER_STATUS,
  BF_DISTANCE,
  BF_CARGO_QUANTITY,
  BF_PRIMARY,
  BF_SECONDARY,
  BF_THEN,
  BF_COMPARISON,
  BF_COUNT_QUANTITY,
  BF_PERCENT,
  BF_SEASON_TIME
};

typedef struct BlockFunctionArgs BlockFunctionArgs;
typedef struct BlockFunction BlockFunction;
typedef struct BlockFunctionRoot BlockFunctionRoot;
typedef struct AIData AIData;
typedef struct BlockFunctionTemplate BlockFunctionTemplate;
typedef struct BlockFunctionGlobals BlockFunctionGlobals;
typedef enum BlockFunctionArgumentType BlockFunctionArgumentType;

typedef int(*blockFunction_WrappedFunction)(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                            ProgrammableWorker *programmableWorker,
                                            GameObjectData *gameObjectData,
                                            int ticks);

struct BlockFunctionArgs{
  char *characters;
  int numOfChars;
  int *integers;
  int numOfInts;
  float *floats;
  int numOfFloats;
  int blockFunctionIndex;
};

struct BlockFunction{
  char name[255];
  blockFunction_WrappedFunction wrapped_function;
  BlockFunction *primary;
  BlockFunction *secondary;
  BlockFunctionArgs arguments;
  /* A terminal block is one is allowed
     to loop backwards, because execution
     pauses and the AI stops */
  int terminal;
};

struct BlockFunctionGlobals{
  int count;
  ProgrammableWorker *selectedWorker;
};

struct BlockFunctionRoot{
  BlockFunction *blockFunctions;
  int numOfBlockFunctions;
  BlockFunctionGlobals globals;
};

struct BlockFunctionTemplate{
  BlockFunctionTemplate *next;
  char name[50];
  int numOfArguments;
  int terminal;
  int red;
  int green;
  int blue;
  BlockFunctionArgumentType *arguments;
};

struct AIData{
  BlockFunctionRoot *blockFunctionRoots;
  BlockFunctionTemplate *templates;
};

FILE *fopenAndVerify(char *file_name, char *permission);

int blockFunction_Void(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                       ProgrammableWorker *programmableWorker,
                       GameObjectData *gameObjectData,
                       int ticks);

int blockFunction_IfWorkerIdle(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                               ProgrammableWorker *programmableWorker,
                               GameObjectData *gameObjectData,
                               int ticks);

int blockFunction_IfWorkerReturning(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                    ProgrammableWorker *programmableWorker,
                                    GameObjectData *gameObjectData,
                                    int ticks);

int blockFunction_IfWorkerHasCargo(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                   ProgrammableWorker *programmableWorker,
                                   GameObjectData *gameObjectData,
                                   int ticks);

int blockFunction_IfWorkerCargoGreaterThan(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData,
                                           int ticks);

int blockFunction_IfWorkerStatusEqual(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                      ProgrammableWorker *programmableWorker,
                                      GameObjectData *gameObjectData,
                                      int ticks);

int blockFunction_IfWorkerOutsideOfBounds(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                          ProgrammableWorker *programmableWorker,
                                          GameObjectData *gameObjectData,
                                          int ticks);

int blockFunction_IfWorkerNearHive(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                   ProgrammableWorker *programmableWorker,
                                   GameObjectData *gameObjectData,
                                   int ticks);

int blockFunction_SetWorkerHeadingRandomly(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData,
                                           int ticks);

int blockFunction_WorkerReturnToHive(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                     ProgrammableWorker *programmableWorker,
                                     GameObjectData *gameObjectData,
                                     int ticks);

int blockFunction_IfNumOfFlowersInRadius(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                     ProgrammableWorker *programmableWorker,
                                     GameObjectData *gameObjectData,
                                     int ticks);

int blockFunction_RememberCurrentLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                          ProgrammableWorker *programmableWorker,
                                          GameObjectData *gameObjectData,
                                          int ticks);

int blockFunction_GoToRememberedLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                         ProgrammableWorker *programmableWorker,
                                         GameObjectData *gameObjectData,
                                         int ticks);

int blockFunction_ForgetRememberedLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData,
                                           int ticks);

int blockFunction_RandomShiftRememberedLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                                ProgrammableWorker *programmableWorker,
                                                GameObjectData *gameObjectData,
                                                int ticks);

int blockFunction_IfNodeFound(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                              ProgrammableWorker *programmableWorker,
                              GameObjectData *gameObjectData,
                              int ticks);

int blockFunction_HeadToFoundNode(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                  ProgrammableWorker *programmableWorker,
                                  GameObjectData *gameObjectData,
                                  int ticks);

int blockFunction_HasRememberedLocation(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                        ProgrammableWorker *programmableWorker,
                                        GameObjectData *gameObjectData,
                                        int ticks);

int blockFunction_CoinFlip(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                           ProgrammableWorker *programmableWorker,
                           GameObjectData *gameObjectData,
                           int ticks);

int blockFuntion_LoadCount(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks);

int blockFunction_SaveCount(BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData, int ticks);

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot,
                                   ProgrammableWorker *programmableWorker,
                                   GameObjectData *gameObjectData,
                                   int ticks);

int runBlockFunctionOverWorker(BlockFunction *blockFunction,
                               BlockFunctionGlobals *globals,
                               ProgrammableWorker *programmableWorker,
                               GameObjectData *gameObjectData,
                               int ticks);

int getNumberOfTextStoredBlocks(FILE *file, int *maxDescLength);

int countCharsInString(char *string, char countChar);

void runAI(AIData *aiData, GameObjectData *gameObjectData, int ticks);

AIData initAIData(void);

BlockFunction createAIBlockFunctionFromTokens(BlockFunctionRoot *blockFunctionRoot, int numOfLinesToUse, char *tokensToUse[7]);

BlockFunctionRoot makeBlockFunctionRootFromString(char *str, int numOfBlocks);

blockFunction_WrappedFunction getBlockFunctionByName(char *blockFunctionName);
void nullifyBlockFunctionRoot(BlockFunctionRoot *root, GameObjectData *gameObjectData);
BlockFunction *testBlockFunctionRootForLoops(BlockFunction *toTest, BlockFunction **testAgainst, int countToTestAgainst);
int testBlockFunctionRootForStart(BlockFunctionRoot *root);
