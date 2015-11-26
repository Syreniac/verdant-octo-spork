#include "generic.h"		

typedef struct GraphicsData GraphicsData;

struct GraphicsData{
  SDL_Window *window;
  SDL_Rect navigationOffset;
  SDL_Renderer *renderer;
  SDL_Texture *workerTexture;
  SDL_Texture *nodeTexture;
  SDL_Texture *hiveTexture;
  struct GrassCollection *grassCollection;
};

typedef struct GrassCollection{
  SDL_Texture *grass1Texture;
  SDL_Texture *grass2Texture;
  SDL_Texture *grass3Texture;
}GrassCollection;

void blitGameObject(SDL_Rect objectRect,
                    GraphicsData *graphicsData,
                    SDL_Texture *texture,
                    double angle,
                    SDL_Point *center,
                    SDL_RendererFlip flip);
                    
void blitTiledBackground(SDL_Rect objectRect,
						 GraphicsData *graphicsData,
						 GrassCollection *grassCollection);                 
					
SDL_Texture *loadTextureFromFile(char *file_name, GraphicsData *graphicsData);
void paintBackground(GraphicsData *graphicsData, int r, int g, int b);
