#include "graphics.h"

void blitGameObject(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Texture *texture, double angle, SDL_Point *center, SDL_RendererFlip flip){
  SDL_Rect tempRect;

  tempRect = objectRect;
  tempRect.x += graphicsData->navigationOffset.x;
  tempRect.y += graphicsData->navigationOffset.y;

  SDL_RenderCopyEx(graphicsData->renderer, texture, NULL, &tempRect, angle, NULL, SDL_FLIP_NONE);
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
