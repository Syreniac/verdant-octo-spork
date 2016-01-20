#include "graphics.h"

static void loadBeeGraphics(GraphicsData *graphicsData);
static void loadUIGraphics(GraphicsData *graphicsData);
static void loadWeatherGraphics(GraphicsData *graphicsData);
static void loadHiveGraphics(GraphicsData *graphicsData);
static void loadSpiderGraphics(GraphicsData *graphicsData);
static void loadIceCreamGraphics(GraphicsData *graphicsData);
static void loadShelterGraphics(GraphicsData *graphicsData);
static void loadNodeGraphics(GraphicsData *graphicsData);
static void loadGrass(GraphicsData *graphicsData);

void paintWeatherLayer(GraphicsData *graphicsData, enum WeatherStatus present_weather){
	/* This function creates a Weather struct and fills in the default
     values. Many of these are defined in generic.h */
	Uint8 o_r = 0,o_g = 0,o_b = 0;
	Uint8 o_a = 80;



	switch (present_weather)
			{
		/* Closing the Window will exit the program */
		case Rain:
			o_r = 107;
			o_g = 107;
			o_b = 115;
			/* Tint o_r,o_g,o_b to dark blue */
			break;
		case Sun:
			o_a = 0;
			/* No tint (could set o_a to 0) */
			break;
		case Snow:
			o_r = 200;
			o_g = 200;
			o_b = 200;
			/* Tint o_r,o_g,o_b to light blue-grey */
			break;
		case Cloud:
			o_r = 123;
			o_g = 123;
			o_b = 123;
			/* Tint o_r,o_g,o_b to grey */
			break;
		default:
			fprintf(stderr,"Weather wasn't recognised.\n");
			fflush(stderr);
			exit(1);
	}

	/* SDL_GetRenderDrawColor(graphicsData->renderer,&o_r,&o_g,&o_b,&o_a); */
	SDL_SetRenderDrawColor(graphicsData->renderer,o_r,o_g,o_b,o_a);

	SDL_SetRenderDrawBlendMode(graphicsData->renderer, SDL_BLENDMODE_BLEND);

	SDL_RenderFillRect(graphicsData->renderer, NULL);
	/* SDL_SetRenderDrawColor(graphicsData->renderer,o_r,o_g,o_b,o_a); */
}

void blitGameObject(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Texture *texture, double angle, SDL_Point *center, SDL_RendererFlip flip){
  SDL_Rect tempRect;

  tempRect = objectRect;
  tempRect.x += graphicsData->navigationOffset.x;
  tempRect.y += graphicsData->navigationOffset.y;

  SDL_RenderCopyEx(graphicsData->renderer, texture, NULL, &tempRect, angle, center, flip);
}

void blitRainRandomly(GraphicsData *graphicsData){

	SDL_Rect srcRect;
	SDL_Rect winRect = {0,0,0,0};
	int i;

  	SDL_GetWindowSize(graphicsData->window,&winRect.w, &winRect.h);

	srcRect.w = RAIN_TILE_WIDTH;
	srcRect.h = RAIN_TILE_HEIGHT;

	for(i = 0; i < 50; i++){

	   srcRect.x = (rand()% winRect.w);
	   srcRect.y = (rand()% winRect.h);

	   if(rand()%30){
	      SDL_RenderCopy(graphicsData->renderer,
						 graphicsData->rainy.graphic[rand()%4],
						 NULL,
						 &srcRect);
	   }else{
	      SDL_RenderCopy(graphicsData->renderer,
						 graphicsData->rainy.graphic[(rand()%2)+3],
					         NULL,
						 &srcRect);
	   }
	}

}


void blitParallaxTreeTops(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Texture *texture){
  SDL_Rect tempRect;
  int xParallaxOffset = 0, yParallaxOffset = 0;

  tempRect = objectRect;
  tempRect.x += graphicsData->navigationOffset.x;
  tempRect.y += graphicsData->navigationOffset.y;

  xParallaxOffset -= X_SIZE_OF_SCREEN/2;
  xParallaxOffset += tempRect.x;
  xParallaxOffset /= PARALLAX_INTENSITY;

  yParallaxOffset -= Y_SIZE_OF_SCREEN/2;
  yParallaxOffset += tempRect.y;
  yParallaxOffset /= PARALLAX_INTENSITY;

  tempRect.x += xParallaxOffset;
  tempRect.y += yParallaxOffset;

  SDL_RenderCopyEx(graphicsData->renderer, texture, NULL, &tempRect, 0, NULL, SDL_FLIP_NONE);
}


void blitTiledBackground(GraphicsData *graphicsData, SDL_Texture *texture){
	int i, j, xbgShifter, ybgShifter;
	SDL_Rect rect;

	int window_x, window_y;

	SDL_GetWindowSize(graphicsData->window,&window_x,&window_y);


	xbgShifter = (graphicsData->navigationOffset.x - X_INITIAL_SCREEN_OFFSET) % GRASS_TILE_WIDTH;
	ybgShifter = (graphicsData->navigationOffset.y - Y_INITIAL_SCREEN_OFFSET) % GRASS_TILE_HEIGHT;

	for(i = -GRASS_TILE_WIDTH; i < window_x + GRASS_TILE_WIDTH; i+= GRASS_TILE_WIDTH){
		for(j = -GRASS_TILE_HEIGHT; j < window_y + GRASS_TILE_HEIGHT; j+= GRASS_TILE_HEIGHT){

			SDL_Rect dstRect;
			dstRect.x = i;
			dstRect.y = j;
			dstRect.w = GRASS_TILE_WIDTH;
			dstRect.h = GRASS_TILE_HEIGHT;

			dstRect.x += xbgShifter;
			dstRect.y += ybgShifter;

			SDL_RenderCopy(graphicsData->renderer, texture, NULL, &dstRect);

		}
	}

	rect.x = window_x;

	if((window_x - graphicsData->navigationOffset.x) > X_SIZE_OF_WORLD){
		rect.y = 0;
		rect.w = (window_x - graphicsData->navigationOffset.x) - X_SIZE_OF_WORLD;
		rect.x = window_x - rect.w;
		rect.h = window_y;
		SDL_SetRenderDrawColor(graphicsData->renderer, 0, 0, 0, 100);
		SDL_RenderFillRect(graphicsData->renderer, &rect);
		graphicsData->navigationOffset.x+= ceil((double)rect.w/10.0);
	}

	if((window_y - graphicsData->navigationOffset.y) > Y_SIZE_OF_WORLD){
		rect.w = rect.x;
		rect.x = 0;
		rect.h = (window_y - graphicsData->navigationOffset.y) - Y_SIZE_OF_WORLD;
		rect.y = window_y - rect.h;
		SDL_SetRenderDrawColor(graphicsData->renderer, 0, 0, 0, 100);
		SDL_RenderFillRect(graphicsData->renderer, &rect);
		graphicsData->navigationOffset.y+= ceil((double)rect.h/10.0);
	}
}


SDL_Texture *loadTextureFromFile(char *file_name, GraphicsData *graphicsData, char toggleAlpha){
	SDL_Surface *image;
	SDL_Texture *texture;

	image = SDL_LoadBMP(file_name);
	if(image == NULL){
		fprintf(stderr,"Image loading has failed: %s\n", SDL_GetError());
		fflush(stderr);
		assert(image != NULL);
	}
	if(toggleAlpha == 1){
		SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(image->format, 255, 255, 255));
	}
	texture = SDL_CreateTextureFromSurface(graphicsData->renderer, image);
	if(texture == NULL){
		fprintf(stderr,"Texture conversion has failed: %s\n", SDL_GetError());
		fflush(stderr);
		assert(texture != NULL);
	}
	SDL_FreeSurface(image);
	return texture;
}

void paintBackground(GraphicsData *graphicsData, int r, int g, int b){
	/* We don't need to know alpha, because the alpha for a background
	   will always be 1 (it doesn't make sense otherwise) */
	/* I don't know if keeping the colours from before is strictly necessary, but
       I'm putting it in anyway! */
	Uint8 o_r,o_g,o_b,o_a;
	SDL_Rect rect;

	rect.x = 0;
	rect.y = 0;
	/* This reads the width and height of the window into the space shown by
	   the pointers */
	SDL_GetWindowSize(graphicsData->window,&rect.w,&rect.h);

	SDL_GetRenderDrawColor(graphicsData->renderer,&o_r,&o_g,&o_b,&o_a);
	SDL_SetRenderDrawColor(graphicsData->renderer,r,g,b,SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(graphicsData->renderer, &rect);
    	SDL_SetRenderDrawColor(graphicsData->renderer,o_r,o_g,o_b,o_a);
}

void renderRadius(GraphicsData *graphicsData, SDL_Point *point, double radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	double currOffsetX, currOffsetY;
	point->x += graphicsData->navigationOffset.x;
	point->y += graphicsData->navigationOffset.y;

	SDL_SetRenderDrawColor(graphicsData->renderer, r,g,b, a);

    for (currOffsetY = 1; currOffsetY <= radius; currOffsetY ++) {
         currOffsetX = floor(sqrt((2.0 * radius * currOffsetY) - (pow(currOffsetY, 2))));
         SDL_RenderDrawPoint(graphicsData->renderer, point->x - currOffsetX, point->y + radius - currOffsetY);
         SDL_RenderDrawPoint(graphicsData->renderer, point->x - currOffsetX, point->y - radius + currOffsetY);
         SDL_RenderDrawPoint(graphicsData->renderer, point->x + currOffsetX, point->y + radius - currOffsetY);
         SDL_RenderDrawPoint(graphicsData->renderer, point->x + currOffsetX, point->y - radius + currOffsetY);
    }

}

void renderFillRadius(GraphicsData *graphicsData, SDL_Point *point, double radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	double currOffsetX, currOffsetY;
	point->x += graphicsData->navigationOffset.x;
	point->y += graphicsData->navigationOffset.y;

	SDL_SetRenderDrawColor(graphicsData->renderer, r,g,b, a);

    for (currOffsetY = 1; currOffsetY <= radius; currOffsetY ++) {
         currOffsetX = floor(sqrt((2.0 * radius * currOffsetY) - (pow(currOffsetY, 2))));
         SDL_RenderDrawLine(graphicsData->renderer,
         					point->x - currOffsetX,
         					point->y + radius - currOffsetY,
         					point->x + currOffsetX,
         					point->y + radius - currOffsetY);
         if(currOffsetY + 1 <= radius){
        	 SDL_RenderDrawLine(graphicsData->renderer,
         						point->x - currOffsetX,
         						point->y - radius + currOffsetY,
         						point->x + currOffsetX,
         						point->y - radius + currOffsetY);
         }

    }

}

void setNavigationOffset(GraphicsData *graphicsData, int x, int y){
	int sx,sy;
	SDL_GetWindowSize(graphicsData->window,&sx,&sy);
	if(x + sx > X_SIZE_OF_WORLD){
		x = X_SIZE_OF_WORLD - sx;
	}
	else if(x < 0){
		x = 0;
	}
	if(y + sy > Y_SIZE_OF_WORLD){
		y = Y_SIZE_OF_WORLD - sy;
	}
	else if(y < 0){
		y = 0;
	}

	x = -x;
	y = -y;

	graphicsData->navigationOffset.x = x;
	graphicsData->navigationOffset.y = y;
}

void centerCameraOnPoint(GraphicsData *graphicsData, int x, int y){
	int win_x, win_y;
	SDL_GetWindowSize(graphicsData->window,&win_x,&win_y);
	x = x - win_x/2;
	y = y - win_y/2;
	setNavigationOffset(graphicsData,x,y);
}

GraphicsData createGraphicsData(void){
	GraphicsData graphicsData;

	graphicsData.window = SDL_CreateWindow(PROGRAM_NAME,25,25,
																		     X_SIZE_OF_SCREEN, Y_SIZE_OF_SCREEN,
																		     SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);

	graphicsData.renderer = SDL_CreateRenderer(graphicsData.window,-1,
																			    	 SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);

	graphicsData.mainMenuImage = loadTextureFromFile("images/mainMenuImage.bmp", &graphicsData, 0);


	/* initialise navigationOffset values */
	graphicsData.navigationOffset.x = X_INITIAL_SCREEN_OFFSET; /*setting initial x offset to center of world*/
	graphicsData.navigationOffset.y = Y_INITIAL_SCREEN_OFFSET; /*setting initial y offset ot center of world*/
	graphicsData.fonts[0] = TTF_OpenFont("font/Oxygen-Regular.ttf",16);
	graphicsData.fonts[1] = TTF_OpenFont("font/Oxygen-Regular.ttf",12);

	graphicsData.trackingMode = 0;

	loadBeeGraphics(&graphicsData);
	loadUIGraphics(&graphicsData);
	loadWeatherGraphics(&graphicsData);
	loadHiveGraphics(&graphicsData);
	loadSpiderGraphics(&graphicsData);
	loadIceCreamGraphics(&graphicsData);
	loadShelterGraphics(&graphicsData);
	loadNodeGraphics(&graphicsData);
	loadGrass(&graphicsData);

	return graphicsData;
}

static void loadBeeGraphics(GraphicsData *graphicsData){

	graphicsData->bee.graphic[BEE_FLAP_GRAPHIC_1] = loadTextureFromFile("images/bee.bmp", graphicsData, 1);
	graphicsData->bee.graphic[BEE_FLAP_GRAPHIC_2] = loadTextureFromFile("images/bee2.bmp",graphicsData, 1);

	graphicsData->bee.graphic[BEE_FLAP_GRAPHIC_1 + CARRYING_FLOWER_INDEX_OFFSET] = loadTextureFromFile("images/beeWithFlower.bmp", graphicsData, 1);
	graphicsData->bee.graphic[BEE_FLAP_GRAPHIC_2 + CARRYING_FLOWER_INDEX_OFFSET] = loadTextureFromFile("images/beeWithFlower2.bmp",graphicsData, 1);

	graphicsData->bee.graphic[BEE_FLAP_GRAPHIC_1 + CARRYING_ICECREAM_INDEX_OFFSET] = loadTextureFromFile("images/beeWithIcecream.bmp", graphicsData, 1);
	graphicsData->bee.graphic[BEE_FLAP_GRAPHIC_2 + CARRYING_ICECREAM_INDEX_OFFSET] = loadTextureFromFile("images/beeWithIcecream2.bmp",graphicsData, 1);
}

static void loadUIGraphics(GraphicsData *graphicsData){

	graphicsData->uiEle.graphic[SCROLLHANDLE_GRAPHIC] = loadTextureFromFile("images/UI/scrollhandle.bmp",graphicsData, 1);

	graphicsData->uiEle.graphic[CROSSBOX_GRAPHIC] = loadTextureFromFile("images/UI/crossbox.bmp",graphicsData, 1);
	graphicsData->uiEle.graphic[COMPILEBOX_GRAPHIC] = loadTextureFromFile("images/UI/compilebox.bmp",graphicsData, 1);
	graphicsData->uiEle.graphic[STOP_GRAPHIC] = loadTextureFromFile("images/UI/stop.bmp",graphicsData, 1);
	graphicsData->uiEle.graphic[BLOCK_GRAPHIC] = loadTextureFromFile("images/UI/block.bmp",graphicsData, 1);
	graphicsData->uiEle.graphic[HIVECELL_GRAPHIC] = loadTextureFromFile("images/UI/hivecell.bmp",graphicsData, 1);
	graphicsData->uiEle.graphic[HIVECELLMASK_GRAPHIC] = loadTextureFromFile("images/UI/hivecellmask.bmp",graphicsData, 1);
}

static void loadWeatherGraphics(GraphicsData *graphicsData){

  graphicsData->rainy.graphic[0] = loadTextureFromFile("images/rain/rain1.bmp", graphicsData, 1);
  graphicsData->rainy.graphic[1] = loadTextureFromFile("images/rain/rain2.bmp", graphicsData, 1);
  graphicsData->rainy.graphic[2] = loadTextureFromFile("images/rain/rain3.bmp", graphicsData, 1);
  graphicsData->rainy.graphic[3] = loadTextureFromFile("images/rain/rain4.bmp", graphicsData, 1);
  graphicsData->rainy.graphic[4] = loadTextureFromFile("images/rain/rain5.bmp", graphicsData, 1);
  graphicsData->rainy.graphic[5] = loadTextureFromFile("images/rain/rain6.bmp", graphicsData, 1);
}

static void loadHiveGraphics(GraphicsData *graphicsData){
  graphicsData->hiveTexture = loadTextureFromFile("images/beehive.bmp", graphicsData, 1);
}

static void loadSpiderGraphics(GraphicsData *graphicsData){

  /*roamingSpider graphics*/
  graphicsData->roamingArachnid.graphic[SPIDER] = loadTextureFromFile("images/spider.bmp", graphicsData, 1);
  graphicsData->roamingArachnid.graphic[SPIDER2] = loadTextureFromFile("images/spider2.bmp", graphicsData, 1);
  graphicsData->roamingArachnid.graphic[SPIDER15] = loadTextureFromFile("images/spider15.bmp", graphicsData, 1);
  graphicsData->roamingArachnid.graphic[SPIDER15b] = loadTextureFromFile("images/spider15.bmp", graphicsData, 1);

  graphicsData->roamingArachnid.graphic[SPIDER_DEAD] = loadTextureFromFile("images/spider_dead2.bmp", graphicsData, 1);
  graphicsData->roamingArachnid.graphic[SPIDER_FIGHTING] = loadTextureFromFile("images/spiderFighting.bmp", graphicsData, 1);
}

static void loadIceCreamGraphics(GraphicsData *graphicsData){

	graphicsData->person.graphic[WITH_ICE_CREAM_STRIDE1] = loadTextureFromFile("images/person/withIceCream1.bmp", graphicsData, 1);
	graphicsData->person.graphic[WITH_ICE_CREAM_STRIDE2] = loadTextureFromFile("images/person/withIceCream2.bmp", graphicsData, 1);

	graphicsData->person.graphic[WITH_ICE_CREAM_STRIDE1 + NO_ICECREAM_INDEX_OFFSET] = loadTextureFromFile("images/person/withoutIceCream1.bmp", graphicsData, 1);
	graphicsData->person.graphic[WITH_ICE_CREAM_STRIDE2 + NO_ICECREAM_INDEX_OFFSET] = loadTextureFromFile("images/person/withoutIceCream2.bmp", graphicsData, 1);

	graphicsData->droppedIceCreamTexture = loadTextureFromFile("images/person/droppedIceCream.bmp", graphicsData, 1);
	graphicsData->meltedIceCreamTexture = loadTextureFromFile("images/person/meltedIceCream.bmp", graphicsData, 1);

}

static void loadShelterGraphics(GraphicsData *graphicsData){

  graphicsData->shelter.graphic[SUMMER_INDEX] =loadTextureFromFile("images/tree1.bmp",graphicsData,1);

  graphicsData->shelter.graphic[AUTUMN_INDEX] = loadTextureFromFile("images/treeAutumn1.bmp",graphicsData, 1);

  graphicsData->shelter.graphic[WINTER_INDEX] =  loadTextureFromFile("images/treeWinter.bmp",graphicsData, 1);
	graphicsData->treeStumpTexture = loadTextureFromFile("images/stump.bmp",graphicsData, 1);
}

static void loadNodeGraphics(GraphicsData *graphicsData){
  graphicsData->nodeTexture[0] = loadTextureFromFile("images/blueFlower.bmp",graphicsData, 1);
  graphicsData->nodeTexture[1] = loadTextureFromFile("images/redFlower.bmp",graphicsData, 1);
  graphicsData->nodeTexture[2] = loadTextureFromFile("images/yellowFlower.bmp",graphicsData, 1);
}

static void loadGrass(GraphicsData *graphicsData){
  graphicsData->grass.graphic[SUMMER_INDEX] = loadTextureFromFile("images/grass/grass4.bmp",graphicsData, 0);
  graphicsData->grass.graphic[AUTUMN_INDEX] = loadTextureFromFile("images/grass/autumnGrass4.bmp", graphicsData, 0);
}
