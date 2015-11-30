#include "generic.h"		


typedef struct GraphicsData GraphicsData;
typedef struct Person Person;
typedef struct Rainy Rainy;

enum WeatherStatus {/*Should belong in game_objects.h, but graphics.c needs to access it*/
  Sun = 0,
  Cloud = 1,
  Rain = 2,
  Snow = 3
};

/*1 means 100% chance of rain after cloud, 2 means 50% chance and so on*/
#define CHANCE_OF_RAIN 1
/*chance of sun turning to cloud after TICKSPERWEATHER*/
#define CHANCE_OF_CLOUD 3


struct GraphicsData{
  SDL_Window *window;
  SDL_Rect navigationOffset;
  SDL_Renderer *renderer;
  SDL_Texture *workerTexture;
  SDL_Texture *nodeTexture;
  SDL_Texture *hiveTexture;
  SDL_Texture *treeTexture;
  SDL_Texture *grassTexture;
  struct Rainy *rainy;
  struct Person *person;
};

struct Person{
/*would have used an array here but seems to be causing problems on other platforms*/
	SDL_Texture *graphic1; 
	SDL_Texture *graphic2;
};

struct Rainy{
/*would have used an array here but seems to be causing problems on other platforms*/
	SDL_Texture *graphic1;
	SDL_Texture *graphic2;
	SDL_Texture *graphic3;		
	SDL_Texture *graphic4;
	SDL_Texture *graphic5;
	SDL_Texture *graphic6;		
};


void paintWeatherLayer(GraphicsData *graphicsData, enum WeatherStatus present_weather, SDL_Texture *texture);

void blitGameObject(SDL_Rect objectRect,
                    GraphicsData *graphicsData,
                    SDL_Texture *texture,
                    double angle,
                    SDL_Point *center,
                    SDL_RendererFlip flip);
                    
void blitParallaxTreeTops(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Texture *texture);
						  
void blitRainRandomly(GraphicsData *graphicsData);

void blitTiledBackground(GraphicsData *graphicsData, SDL_Texture *texture);
SDL_Texture *loadTextureFromFile(char *file_name, GraphicsData *graphicsData);
void paintBackground(GraphicsData *graphicsData, int r, int g, int b);

