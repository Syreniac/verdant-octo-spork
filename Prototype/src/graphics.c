#include "graphics.h"


void paintWeatherLayer(GraphicsData *graphicsData, enum WeatherStatus present_weather, SDL_Texture *texture){
	/* This function creates a Weather struct and fills in the default
     values. Many of these are defined in generic.h */
	Uint8 o_r,o_g,o_b, o_a = 155;

	printf("Current weather is: %d\n", present_weather);

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
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND); /* Only blending worker textures currently */
	SDL_RenderFillRect(graphicsData->renderer, NULL);
	/* SDL_SetRenderDrawColor(graphicsData->renderer,o_r,o_g,o_b,o_a); */
}

void blitGameObject(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Texture *texture, double angle, SDL_Point *center, SDL_RendererFlip flip){
  SDL_Rect tempRect;

  tempRect = objectRect;
  tempRect.x += graphicsData->navigationOffset.x;
  tempRect.y += graphicsData->navigationOffset.y;

  SDL_RenderCopyEx(graphicsData->renderer, texture, NULL, &tempRect, angle, NULL, SDL_FLIP_NONE);
}


void blitParallaxTreeTops(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Texture *texture, double angle, SDL_Point *center, SDL_RendererFlip flip){
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
  
  printf("xParallaxOffset = %d\n", xParallaxOffset);
  tempRect.x += xParallaxOffset;
  tempRect.y += yParallaxOffset;
  
  SDL_RenderCopyEx(graphicsData->renderer, texture, NULL, &tempRect, angle, NULL, SDL_FLIP_NONE);
}


void blitTiledBackground(GraphicsData *graphicsData, SDL_Texture *texture){
	int i, j, xbgShifter, ybgShifter, nextSeed = rand();

	xbgShifter = (graphicsData->navigationOffset.x - X_INITIAL_SCREEN_OFFSET) % GRASS_TILE_WIDTH;
	ybgShifter = (graphicsData->navigationOffset.y - Y_INITIAL_SCREEN_OFFSET) % GRASS_TILE_HEIGHT;

	for(i = -GRASS_TILE_WIDTH; i < X_SIZE_OF_SCREEN; i+= GRASS_TILE_WIDTH){
		for(j = -GRASS_TILE_HEIGHT; j < Y_SIZE_OF_SCREEN; j+= GRASS_TILE_HEIGHT){

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
}


SDL_Texture *loadTextureFromFile(char *file_name, GraphicsData *graphicsData){
	SDL_Surface *image;
	SDL_Texture *texture;

	printf("loading texture file %s\n",file_name);
	image = SDL_LoadBMP(file_name);
	if(image == NULL){
		printf("Image loading has failed: %s\n", SDL_GetError());
		assert(image != NULL);
	}
	SDL_SetColorKey(image, SDL_TRUE, SDL_MapRGB(image->format, 255, 255, 255));
	texture = SDL_CreateTextureFromSurface(graphicsData->renderer, image);
	if(texture == NULL){
		printf("Texture conversion has failed: %s\n", SDL_GetError());
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
