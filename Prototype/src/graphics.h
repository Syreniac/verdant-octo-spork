#include "generic.h"		

typedef struct GraphicsData GraphicsData;

struct GraphicsData{
  SDL_Window *window;
  SDL_Rect navigationOffset;
  SDL_Renderer *renderer;
  SDL_Texture *workerTexture;
  SDL_Texture *nodeTexture;
  SDL_Texture *hiveTexture;
};

void blitGameObject(SDL_Rect objectRect,
                    GraphicsData *graphicsData,
                    SDL_Texture *texture,
                    double angle,
                    SDL_Point *center,
                    SDL_RendererFlip flip);
					
SDL_Texture *loadTextureFromFile(char *file_name, GraphicsData *graphicsData);
void paintBackground(GraphicsData *graphicsData, int r, int g, int b);