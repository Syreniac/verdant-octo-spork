#ifndef _GENERIC_INCLUDED
#include "generic.h"
#endif
/* Because we're working with numerous files that need to include game_objects.h
   having this #ifndef check here prevents things breaking from trying to
   include the same file several times */
#ifndef _GAME_OBJECTS_INCLUDED
#include "game_objects.h"
#endif

/* This is where we put the procedural generation stuff. */

/* It's not really very complicated at the moment */

void generateResourceNodeSpawners(GameObjectData *gameObjectData);

void generateProgrammableWorkers(GameObjectData *gameObjectData);

void generateHive(GameObjectData *gameObjectData);
