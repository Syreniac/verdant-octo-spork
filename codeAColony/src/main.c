#include "main.h"

#define xCellNum 68
#define yCellNum 48
#define cellSize 10

extern void init(char *);
extern void fillCellMapData(SDL_Surface *screen, int cellMap[xCellNum][yCellNum]);
extern void cleanup(void);
extern void getInput(void);

int main(int argc, char *argv[])
{
	int go;
	
	/* Start up SDL, window title "CodeAColony" */
	init("CodeAColony");
	
	/* Call the cleanup function when the program exits */
	atexit(cleanup);

	
	go = 1;
	/* Loop indefinitely for messages */
	
	while (go == 1)
	{

		getInput();

		
		/* Sleep briefly to stop sucking up all the CPU time */	
		SDL_Delay(16);
	}

	/* Exit the program */
	
	exit(0);
}
