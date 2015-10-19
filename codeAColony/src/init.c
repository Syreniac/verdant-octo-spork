#include "init.h"
#include <time.h>
#include <math.h>
#include <string.h>


/*determin the number of cells the game environment is,
and the pixel size of each cell*/
#define xCellNum 100
#define yCellNum 60
#define cellSize 10

/*Entity struct could be a tree, or flower, with
multiple possible states or attributes*/
typedef struct{
    int x, y, dimension, pollinated, nectar, colonised, honey;
} Entity;


void fillCellMapData(SDL_Surface *surface, Entity* cellMap[xCellNum][yCellNum]);
int probOutOf(int val, int prob);
Entity* createEntity(char name[], int x, int y);

void init(char *title)
{
        /* declare surface for each image to be used*/
        SDL_Surface *grass, *flower, *blueFlower, *tree, *banner, *temp;
        
        /*declare an array of null pointers. There is one array element for
        every cell in the game environment. These array elements can be used later
        to point to entities requiring only the entity x y co-ordinates*/
        Entity *cellMap[xCellNum][yCellNum] = {{0},{0}};
        
        /*rectangle structs that will be used later to specific x y w and h to
        SDL functions requiring co-ordinate and dimension information*/
        SDL_Rect srcRect, dstRect;
        
        char* file_name;
        int i, j;
        Uint32 colorkey;

        
	/* Initialise SDL library (must be called before any other SDL_function),
	SDL_INIT_VIDEO flag initialise only the video SDL subsystem*/	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());	
		exit(1);
	}
	
	/* Open a screen, the size of the entire game environment. This needs to be
	amended so that it only displays a fraction of a larger game environment and can
	be used to navigate up, down and across */
	screen = SDL_SetVideoMode(xCellNum*cellSize, yCellNum*cellSize, 0, SDL_HWPALETTE);
	
	if (screen == NULL)
	{
		printf("Couldn't set screen mode to 640 x 480: %s\n", SDL_GetError());

		exit(1);
	}
	
	/*Randomly generate a game environment and fill the cellMap array with pointers
	to entities (trees/flowers). The index location of these pointers in the cellMap
	array corresponds with the x y co-ordinates of the entity in the game environment */
	fillCellMapData(screen,cellMap);
	
	
        /* load grass.bmp file into temp surface, and copy temp surface to grass surface
        using SDL_DisplayFormat to optimise for fast blitting to the screen*/
        file_name = "src/images/grass.bmp";
        temp = SDL_LoadBMP(file_name);
        grass = SDL_DisplayFormat(temp);
        SDL_FreeSurface(temp);
        if(grass == NULL){
                printf("Error: Couldn't load %s: %s\n", file_name, SDL_GetError());
        }
        
        /* do the same for banner.bmp */
        file_name = "src/images/banner.bmp";
        temp = SDL_LoadBMP(file_name);
        banner = SDL_DisplayFormat(temp);
        SDL_FreeSurface(temp);
        if(banner == NULL){
                printf("Couldn't load %s: %s\n", file_name, SDL_GetError());
        }        
        
        /*do the same for flower.bmp*/
        file_name = "src/images/flower.bmp";
        temp = SDL_LoadBMP(file_name);
        flower = SDL_DisplayFormat(temp);
        SDL_FreeSurface(temp);
        if(flower == NULL){
                printf("Couldn't load %s: %s\n", file_name, SDL_GetError());
        }       
        

        
        /*do the same for blueFlower.bmp*/
        file_name = "src/images/blueFlower.bmp";
        temp = SDL_LoadBMP(file_name);
        blueFlower = SDL_DisplayFormat(temp);
        SDL_FreeSurface(temp);
        if(blueFlower == NULL){
                printf("Couldn't load %s: %s\n", file_name, SDL_GetError());
        }
        
        /*do the same for tree.bmp */
        file_name = "src/images/tree.bmp";
        temp = SDL_LoadBMP(file_name);
        tree = SDL_DisplayFormat(temp);
        SDL_FreeSurface(temp);
        if(tree == NULL){
                printf("Couldn't load %s: %s\n", file_name, SDL_GetError());
        }
           
           
        /* set colorKey values for flower and blueflower so that their black and
        white backgrounds (repsectively) become transparent*/
        colorkey = SDL_MapRGB(flower->format, 0,0,0);
        SDL_SetColorKey(flower, SDL_SRCCOLORKEY, colorkey);
        colorkey = SDL_MapRGB(blueFlower->format, 255,255,255);
        SDL_SetColorKey(blueFlower, SDL_SRCCOLORKEY, colorkey);
        colorkey = SDL_MapRGB(tree->format, 255,255,255);
        SDL_SetColorKey(tree, SDL_SRCCOLORKEY, colorkey);



        /*Blit background grass onto the screen surface*/
        for(i = 0; i < screen->w; i+=grass->w){
                for(j = 0; j < screen->h; j+=grass->h){
                        /*dstRect specifies x y co-ordinates for each grass tile*/
                        SDL_Rect dstRect;
                        dstRect.x = i;
                        dstRect.y = j;
                        /*NULL specifies that entire grass tile is copied to screen*/
                        if(SDL_BlitSurface(grass, NULL, screen, &dstRect) < 0){
                                printf("Blitsurface error: %s\n", SDL_GetError());
                        }
                        
                }
        }
        


        /*Blit entities (trees/flowers) onto the screen at the correct location
        by querying the surface pointer at each index of the 2D cellMap array*/
        for(i = 0; i < yCellNum; i++){
            for(j = 0; j < xCellNum; j++){
                
                /*if pointer is not NULL and surface pointed to contains int
                dimension == 20 then there is a flower at this location*/
                if(cellMap[j][i] != NULL && cellMap[j][i]->dimension == 20){

                    dstRect.x = j * cellSize;
                    dstRect.y = i * cellSize;
                    if(SDL_BlitSurface(flower, NULL, screen, &dstRect) < 0){
                        printf("Blitsurface error: %s\n", SDL_GetError());
                    }
                    
                /*if pointer is not NULL and surface pointed to contains int
                dimension == 250 then there is a tree at this location. ONLY
                blit tree to screen if x y co-ordinates of tree match current
                cellMap index (this prevents tree from being rendered again and 
                again at every cellMap index that it spills onto)*/
                }else if(cellMap[j][i] != NULL &&\
                cellMap[j][i]->dimension == 250 &&\
                cellMap[j][i]->x == j && cellMap[j][i]->y == i){
                
                    dstRect.x = j * cellSize;
                    dstRect.y = i * cellSize;
                    if(SDL_BlitSurface(tree, NULL, screen, &dstRect) < 0){
                        printf("Blitsurface error: %s\n", SDL_GetError());
                    }
                    
                /*if pointer is not NULL and surface pointed to contains int
                dimension = 10 then there is a blueFlower at this location.*/
                }else if(cellMap[j][i] != NULL &&\
                cellMap[j][i]->dimension == 10){

                    dstRect.x = j * cellSize;
                    dstRect.y = i * cellSize;
                    if(SDL_BlitSurface(blueFlower,NULL, screen, &dstRect) < 0){
                        printf("Blitsurface error: %s\n", SDL_GetError());
                    }
                }
            }
        }
        
        /*no blit banner on top of everything else at top of screen surface.
        srcRect values specify what section of the banner is copied to screen*/
        srcRect.x = 0;
        srcRect.y = 30;
        srcRect.w = xCellNum*cellSize;
        srcRect.h = 80;      
        if(SDL_BlitSurface(banner, &srcRect, screen, NULL) < 0){
            printf("Blitsurface error: %s\n", SDL_GetError());
        }
        
                             
        /*Update entire screen to show all of the blits*/
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        
        /* Free the allocated image surfaces */
        SDL_FreeSurface(grass);
	    SDL_FreeSurface(banner);
	    SDL_FreeSurface(flower);
	    SDL_FreeSurface(blueFlower);
	    SDL_FreeSurface(tree);

	
	/* Set the screen title */
	SDL_WM_SetCaption(title, NULL);
}

void fillCellMapData(SDL_Surface *screen, Entity *cellMap[xCellNum][yCellNum])
{

    int x,y;
    srand(time(NULL));
    
    for(y = 0; y < yCellNum; y++){
        for(x = 0; x < xCellNum; x++){
            /*there is a 1 out of 1000 chance, if current cellMap index is
            less that 25 cells away from the bottom and right edge of the cellMap
            to prevent overflowing the cellMap array (tree takes up 25by25 cells)*/
            if(probOutOf(1,1000) && x < (xCellNum-25) && y < (xCellNum-25)){
                int i,j;
                
                /*create tree entity and return pointer to cellMap at current index*/
                cellMap[x][y] = createEntity("tree",x,y);
                
                for(i = 0; i < 25; i++){
                    for(j = 0; j < 25; j++){
                        /*copy the address of the tree entity to every cell that tree
                        overlaps onto*/
                        cellMap[x+j][y+i] = cellMap[x][y];
                        /*unless about to fall outside the index boundaries of cellMap */
                        if(x+j >= xCellNum || y+i >= yCellNum){
                            /*in which case break out of for loops*/
                            j = 25, i = 25;
                        }
                    }
                }
               
            /*in the 999 out of 1000 chance that a tree wasn't created at the current
            cellMap index, and in the event that no previous tree overlaps with current
            index then...*/
            }else if(cellMap[x][y] == NULL||\
            (cellMap[x][y] != NULL && cellMap[x][y]->dimension != 250)){
                
                /*if x co-ordinate isn't by edges, and y co-ordinate isn't at top*/
                if(x > 2 && y > 2 && x < (xCellNum-3)){
                
                    /*then search previous cellMap elements that are close by to see
                    if a flower was rendered near to current location*/                    
                    int backSearchX = (rand()%7)-3;
                    int backSearchY = (rand()%3)+1;           
                    if(cellMap[x-backSearchX][y-backSearchY] != NULL &&\
                    cellMap[x-backSearchX][y-backSearchY]->dimension == 20){
                        /*if flower found nearby, increase chances of new flower
                        being created (2 out of 3 chance)*/
                        if(probOutOf(2,3)){
                            cellMap[x][y] = createEntity("flower", x,y);
                        }
                    }else{
                        /*if no flower found chances of flower are 1 out of 100
                        and 99 out of 100 times there is a 1 out of 500 chance that 
                        a blueFlower will be created*/
                        if(probOutOf(1,100)){
                            cellMap[x][y] = createEntity("flower", x,y);
                        }else if(probOutOf(1,500)){
                            cellMap[x][y] = createEntity("blueFlower",x,y);
                        }else{
                            /*if blueFlower nearby, increase chances of creating
                            a blue flower to 5 out of 6*/
                            if(cellMap[x-backSearchX][y-backSearchY] &&\
                            cellMap[x-backSearchX][y-backSearchY]->dimension == 10){
                                if(probOutOf(5,6)){
                                    cellMap[x][y] = createEntity("blueFlower",x,y);
                                } 
                            }
                        }
                    }
                        
                /*else if co-ordinates are by edges (meaning that searching nearby index's
                of cellMap could result in overspilling its boundaries) then...*/
                }else{
                    /*there is a 1 out of 100 chance of creating a flower, 99 out of 100
                    times there is a 1 out of 500 chance of creating a blue flower*/
                    if(probOutOf(1,100)){
                        cellMap[x][y] = createEntity("flower",x,y);
                    }else if(probOutOf(1,500)){
                        cellMap[x][y] = createEntity("blueFlower",x,y);
                    }
                }
                
              
            }

        }
    }
    
   
}

/* 'val' out of 'prob' chance of returning 1, otherwise 0*/
int probOutOf(int val, int prob)
{
    if(rand() % prob < val){
        return 1;
    }else{
        return 0;
    }
}


/*create entity specificied by string argument, at the
location specified by arguments int x, int y*/
Entity* createEntity(char name[], int x, int y)
{
    Entity *ent = (Entity*) malloc(sizeof(Entity));
    if(ent == NULL){
        printf("ERROR: malloc failed to allocated memory\n"); 
    }
    
    if(!strcmp(name, "flower")){

        ent->x = x;
        ent->y = y;
        ent->dimension = 20;
        ent->pollinated = 0;
        ent->nectar = 100;
        ent->colonised = 0;
        ent->honey = 0;
        return ent;

    }else if(!strcmp(name, "blueFlower")){

        ent->x = x;
        ent->y = y;
        ent->dimension = 10;
        ent->pollinated = 0;
        ent->nectar = 100;
        ent->colonised = 0;
        ent->honey = 0; 
        return ent;
        
    }else if(!strcmp(name, "tree")){

        ent->x = x;
        ent->y = y;
        ent->dimension = 250;
        ent->pollinated = 0;
        ent->nectar = 0;
        ent->colonised = 0;
        ent->honey = 0;     
        return ent;
                
    }else{
        printf("ERROR: %s is an unrecognised name.", name);
        return ent;
    }
}


void cleanup()
{
	/* Shut down SDL */

	SDL_Quit();
}
