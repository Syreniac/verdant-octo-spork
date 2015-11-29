#include "generic.h"		


typedef struct GraphicsData GraphicsData;
typedef struct Person Person;

enum WeatherStatus {/*Should belong in game_objects.h, but graphics.c needs to access it*/
  Rain = 0,
  Sun = 1,
  Snow = 2,
  Cloud = 3
};

struct GraphicsData{
  SDL_Window *window;
  SDL_Rect navigationOffset;
  SDL_Renderer *renderer;
  SDL_Texture *workerTexture;
  SDL_Texture *nodeTexture;
  SDL_Texture *hiveTexture;
  SDL_Texture *treeTexture;
  SDL_Texture *grassTexture;
  struct Person *person;
};

struct Person{
	SDL_Texture *graphic[2];
};


void paintWeatherLayer(GraphicsData *graphicsData, enum WeatherStatus present_weather, SDL_Texture *texture);

void blitGameObject(SDL_Rect objectRect,
                    GraphicsData *graphicsData,
                    SDL_Texture *texture,
                    double angle,
                    SDL_Point *center,
                    SDL_RendererFlip flip);
                    
void blitParallaxTreeTops(SDL_Rect objectRect,
						  GraphicsData *graphicsData,
						  SDL_Texture *texture,
						  double angle,
						  SDL_Point *center,
						  SDL_RendererFlip flip);

void blitTiledBackground(GraphicsData *graphicsData, SDL_Texture *texture);
SDL_Texture *loadTextureFromFile(char *file_name, GraphicsData *graphicsData);
void paintBackground(GraphicsData *graphicsData, int r, int g, int b);

