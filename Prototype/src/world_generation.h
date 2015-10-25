#include "generic.h"
/* Because we're working with numerous files that need to include game_objects.h
   having this #ifndef check here prevents things breaking from trying to
   include the same file several times */
#include "game_objects.h"

/* This is where we put the procedural generation stuff. */

/* I think that it will be better to do static respawning objects through
   creating spawners rather than directly into the world.

   Doing it this way, we can use the created hierarchy to cull collision
   checking quickly and respawning resources will be easier */

ResourceNodeSpawner createResourceNodeSpawner(int maximumNodeCount, float xPosition, float yPosition, float radius);
ResourceNode createResourceNode(ResourceNodeSpawner *parentSpawner, int resourceUnits);
float generateRandomCoordOffset(float radius);
void updateResourceNodeSpawner(ResourceNodeSpawner *spawner, float ticks);
ResourceNode initResourceNode(ResourceNodeSpawner *spawner);
