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
  BF_SEASON_TIME,
  BF_DURATION
};

typedef struct BlockFunctionArgs BlockFunctionArgs;
typedef struct BlockFunction BlockFunction;
typedef struct BlockFunctionRoot BlockFunctionRoot;
typedef struct AIData AIData;
typedef struct BlockFunctionTemplate BlockFunctionTemplate;
typedef struct BlockFunctionGlobals BlockFunctionGlobals;
typedef enum BlockFunctionArgumentType BlockFunctionArgumentType;
typedef struct BlockFunctionUniversals BlockFunctionUniversals;

typedef int(*blockFunction_WrappedFunction)(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                            ProgrammableWorker *programmableWorker,
                                            GameObjectData *gameObjectData);

struct BlockFunctionArgs{
  char *characters;
  int numOfChars;
  int *integers;
  int numOfInts;
  double *floats;
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

struct BlockFunctionUniversals{
  int idle_count;
};

struct BlockFunctionGlobals{
  int count;
  ProgrammableWorker *selectedWorker;
  int ticks;
  char flip;
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
  blockFunction_WrappedFunction function;
};

struct AIData{
  BlockFunctionRoot *blockFunctionRoots;
  BlockFunctionTemplate *templates;
};

int blockFunction_SetWorkerHeadingRandomly(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                           BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData);

int blockFunction_WorkerReturnToHive(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                     BlockFunctionArgs *arguments,
                                     ProgrammableWorker *programmableWorker,
                                     GameObjectData *gameObjectData);

int blockFunction_GoToRememberedLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                         BlockFunctionArgs *arguments,
                                         ProgrammableWorker *programmableWorker,
                                         GameObjectData *gameObjectData);

int blockFunction_HeadToFoundNode(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                  BlockFunctionArgs *arguments,
                                  ProgrammableWorker *programmableWorker,
                                  GameObjectData *gameObjectData);

int blockFunction_Void(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                       BlockFunctionArgs *arguments,
                       ProgrammableWorker *programmableWorker,
                       GameObjectData *gameObjectData);

int blockFunction_IfWorkerIdle(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                               BlockFunctionArgs *arguments,
                               ProgrammableWorker *programmableWorker,
                               GameObjectData *gameObjectData);

int blockFunction_IfWorkerReturning(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                    BlockFunctionArgs *arguments,
                                    ProgrammableWorker *programmableWorker,
                                    GameObjectData *gameObjectData);

int blockFunction_IfWorkerHasCargo(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                   BlockFunctionArgs *arguments,
                                   ProgrammableWorker *programmableWorker,
                                   GameObjectData *gameObjectData);

int blockFunction_IfWorkerStatusEqual(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                      BlockFunctionArgs *arguments,
                                      ProgrammableWorker *programmableWorker,
                                      GameObjectData *gameObjectData);

int blockFunction_IfWorkerOutsideOfBounds(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals,
                                          BlockFunctionArgs *arguments,
                                          ProgrammableWorker *programmableWorker,
                                          GameObjectData *gameObjectData);

int blockFunction_RememberCurrentLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                          ProgrammableWorker *programmableWorker,
                                          GameObjectData *gameObjectData);

int blockFunction_ForgetRememberedLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                           ProgrammableWorker *programmableWorker,
                                           GameObjectData *gameObjectData);

int blockFunction_RandomShiftRememberedLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                                ProgrammableWorker *programmableWorker,
                                                GameObjectData *gameObjectData);

int blockFunction_IfNodeFound(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                              ProgrammableWorker *programmableWorker,
                              GameObjectData *gameObjectData);

int blockFunction_HasRememberedLocation(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                                        ProgrammableWorker *programmableWorker,
                                        GameObjectData *gameObjectData);

int blockFunction_CoinFlip(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments,
                           ProgrammableWorker *programmableWorker,
                           GameObjectData *gameObjectData);

int blockFuntion_LoadCount(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData);

int blockFunction_SaveCount(BlockFunctionUniversals *universals, BlockFunctionGlobals *globals, BlockFunctionArgs *arguments, ProgrammableWorker *programmableWorker, GameObjectData *gameObjectData);

void runBlockFunctionRootOverWorker(BlockFunctionRoot *blockFunctionRoot,
                                    BlockFunctionUniversals *universals,
                                    ProgrammableWorker *programmableWorker,
                                    GameObjectData *gameObjectData,
                                    int ticks);

int runBlockFunctionOverWorker(BlockFunction *blockFunction,
                               BlockFunctionUniversals *universals,
                               BlockFunctionGlobals *globals,
                               ProgrammableWorker *programmableWorker,
                               GameObjectData *gameObjectData);

int getNumberOfTextStoredBlocks(FILE *file, int *maxDescLength);

int countCharsInString(char *string, char countChar);

void runAI(AIData *aiData, GameObjectData *gameObjectData, int ticks);

AIData initAIData(void);

BlockFunction createAIBlockFunctionFromTokens(AIData *aiData, BlockFunctionRoot *blockFunctionRoot, int numOfLinesToUse, char *tokensToUse[7]);

BlockFunctionRoot makeBlockFunctionRootFromString(AIData *aiData, char *str, int numOfBlocks);

void nullifyBlockFunctionRoot(BlockFunctionRoot *root, GameObjectData *gameObjectData);
BlockFunction *testBlockFunctionRootForLoops(AIData *aiData, BlockFunction *toTest, BlockFunction **testAgainst, int countToTestAgainst);
int testBlockFunctionRootForStart(BlockFunctionRoot *root);
