#include "generic.h"


typedef struct GraphicsData GraphicsData;
typedef struct Person Person;
typedef struct Rainy Rainy;
typedef struct Bee Bee;

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

#define BEE_FLAP_GRAPHIC_1 0
#define BEE_FLAP_GRAPHIC_2 1


struct GraphicsData{
  SDL_Window *window;
  SDL_Rect navigationOffset;
  SDL_Renderer *renderer;
  SDL_Texture *nodeTexture;
  SDL_Texture *hiveTexture;
  SDL_Texture *treeTexture;
  SDL_Texture *grassTexture;
  TTF_Font *fonts[2];
  SDL_Texture *droppedIceCreamTexture;
  SDL_Texture *meltedIceCreamTexture;
  struct Bee *bee;
  struct Rainy *rainy;
  struct Person *person;
  SDL_Texture *mainMenuImage;
};

struct Bee{
	SDL_Texture *graphic[6];
};

struct Person{
	SDL_Texture *graphic[4];
};

struct Rainy{
	SDL_Texture *graphic[6];
};


void paintWeatherLayer(GraphicsData *graphicsData, enum WeatherStatus present_weather);

void blitGameObject(SDL_Rect objectRect,
                    GraphicsData *graphicsData,
                    SDL_Texture *texture,
                    double angle,
                    SDL_Point *center,
                    SDL_RendererFlip flip);

void blitParallaxTreeTops(SDL_Rect objectRect, GraphicsData *graphicsData, SDL_Texture *texture);

void blitRainRandomly(GraphicsData *graphicsData);

void blitTiledBackground(GraphicsData *graphicsData, SDL_Texture *texture);
SDL_Texture *loadTextureFromFile(char *file_name, GraphicsData *graphicsData, char toggleAlpha);
void paintBackground(GraphicsData *graphicsData, int r, int g, int b);
