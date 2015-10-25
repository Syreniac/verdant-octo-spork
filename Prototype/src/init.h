/* This is the header file for init.c.
   Most of the stuff that would be here has been moved to the generic_includes.h
   file. */
#ifndef _GENERIC_INCLUDED
#include "generic.h"
#endif
#include "game.h"

SDL_Window *initialise(void);
int uninitialise(void);
