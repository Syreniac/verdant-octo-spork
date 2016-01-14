#include "configuration.h"


typedef struct GraphicsData GraphicsData;
typedef struct Person Person;
typedef struct RoamingArachnid RoamingArachnid;
typedef struct Rainy Rainy;
typedef struct Bee Bee;
typedef struct Shelter shelter;
typedef struct UIEle UIEle;

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

#define SCROLLHANDLE_GRAPHIC 0
#define CROSSBOX_GRAPHIC 1
#define COMPILEBOX_GRAPHIC 2
#define STOP_GRAPHIC 3
#define BLOCK_GRAPHIC 4
#define HIVECELL_GRAPHIC 5
#define HIVECELLMASK_GRAPHIC 6


struct GraphicsData{
  SDL_Window *window;
  SDL_Rect navigationOffset;
  int trackingMode;
  SDL_Renderer *renderer;
  SDL_Texture *nodeTexture;
  SDL_Texture *hiveTexture;
  SDL_Texture *treeStumpTexture;
  SDL_Texture *grassTexture;
  TTF_Font *fonts[2];
  SDL_Texture *droppedIceCreamTexture;
  SDL_Texture *meltedIceCreamTexture;
  struct Shelter *shelter;
  struct Bee *bee;
  struct Rainy *rainy;
  struct Person *person;
  struct RoamingArachnid *roamingArachnid;
  struct UIEle *uiEle;
  SDL_Texture *mainMenuImage;
};

struct Shelter{
	SDL_Texture *graphic[3];
};

struct Bee{
	SDL_Texture *graphic[6];
};

struct Person{
	SDL_Texture *graphic[4];
};

struct RoamingArachnid{
	SDL_Texture *graphic[6];
};

struct Rainy{
	SDL_Texture *graphic[6];
};

struct UIEle{
	/* Need as many array slots as are defined by UIElement_Variety in UI.h, minus the FILLRECT variety. */
	SDL_Texture *graphic[10];
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
void renderRadius(GraphicsData *graphicsData, SDL_Point *point, double radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void renderFillRadius(GraphicsData *graphicsData, SDL_Point *point, double radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void setNavigationOffset(GraphicsData *graphicsData, int x, int y);
