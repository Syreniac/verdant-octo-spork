#include "init.h"

InitData initialise(void){
  /* This function will initialise the SDL library and create a blank window.
     I'll line by line comment what I'm doing here. */
  InitData initData;

  srand(time(NULL));
  
  /* Initialise SDL library (must be called before any other SDL_function),
  SDL_INIT_VIDEO flag initialise only the video SDL subsystem||SAM: audio must be called in a similar fashion||*/
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
  {
    /* If the SDL_Init function doesn't return -1, then something has gone wrong
    and we should probably close (after giving a helpful error message!) */
  	printf("Could not initialize SDL: %s\n", SDL_GetError());
  	exit(1);
  }

  /* Now that SDL has been initialised by call SDL_Init, we can make the window
     The arguments are:
        - The window's name
        - X coords to open the window at (we don't need to specify something)
        - Y coords like above
        - The X size of the window in pixels
        - The Y size like above
        - SDL configuration options which can be found online in the API documentation*/

	initData.graphicsData.window = SDL_CreateWindow(PROGRAM_NAME,
										   SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           X_SIZE_OF_SCREEN, Y_SIZE_OF_SCREEN,
                                           SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
										   
    initData.graphicsData.renderer = SDL_CreateRenderer(initData.graphicsData.window,
                                               -1,
                                               SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_PRESENTVSYNC);
	assert(initData.graphicsData.renderer != NULL);

	/*Audio needs to be initialized at the very start too.*/
	audioSystem(&initData.audioData);
	
	loadMusic("music01.wav" , 0, &initData.audioData);
	loadMusic("music02.wav" , 1, &initData.audioData);
	loadMusic("music03.wav" , 1, &initData.audioData);
	printf("ALL MUSIC LOADED\n");
	loadSoundEffect("returnFlower.wav", "returnFlower", &initData.audioData);
	loadSoundEffect("thunder.wav", "thunder", &initData.audioData);
	
  return initData;
}

void uninitialise(void){
  /* This function should cleanup the SDL library for us */
  SDL_Quit();
}

void audioSystem(AudioData *AudioSettings){
	
	AudioSettings->audio_rate = 44100;
	AudioSettings->audio_format = AUDIO_S16SYS;
	AudioSettings->audio_channels = 2;
	AudioSettings->audio_buffers = 4096;
	AudioSettings->music = NULL;
	
	AudioSettings->seasonal_music_count[0] = 0;
	AudioSettings->seasonal_music_count[1] = 0;
	AudioSettings->seasonal_music_count[2] = 0;
	AudioSettings->seasonal_music_count[3] = 0;
	AudioSettings->seasonal_music_count[4] = 0;

	if(Mix_OpenAudio(AudioSettings->audio_rate, AudioSettings->audio_format, AudioSettings->audio_channels, AudioSettings->audio_buffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}
	
}

int game_welcome_page(GraphicsData graphicsData, AudioData audioData){

   InitData initData;

   int menuRunning = 1;
   SDL_Rect rect;
   SDL_Event event;
   UI_Element *element;

   initData.graphicsData = graphicsData;
   initData.audioData = audioData;

/*   gameData.graphicsData = graphicsData; */

   /* create a black background*/


   /* create box 1 (title), size fixed*/

   rect.x = 0;
   rect.y = 0;
   rect.w = 40;
   rect.h = 40;


   initData.uiData.root = calloc(1,sizeof(UI_Element));

   element = calloc(1,sizeof(UI_Element));

   element->rect.x = 100;
   element->rect.y = 100;
   element->rect.w = 50;
   element->rect.h = 50;
   element->actions = malloc(sizeof(UI_Action)*3);
   element->num_of_actions = 3;
   element->parent = NULL;
   element->child = NULL;
   element->sibling = NULL;

   UIConfigure_FillRect(element,&element->actions[0],0,100,100);
   UIConfigure_Counter(element,&element->actions[1]);
   UIConfigure_LeftClickRect(element,&element->actions[2]);
       UITrigger_Bind(&element->actions[2],&element->actions[1],-1,UITRIGGER_PLUSONE);

   UIElement_Reparent(element,initData.uiData.root);


   /* create box 2 (start), expanded*/

   /* create box 3 (load), not expanded*/

   /* create box 4 (tutorial) not expanded*/

   playMusic(&initData.audioData,0);   
   
   while(menuRunning){

      UIRoot_Execute(&initData.uiData,UPDATE);

      paintBackground(&initData.graphicsData,0,200,100);
      UIRoot_Execute(&initData.uiData,RENDER_BASE,&initData.graphicsData);

      SDL_RenderPresent(initData.graphicsData.renderer);
    	while (SDL_PollEvent(&event))
    	{
    		switch (event.type)
    		{
    			/* Closing the Window will exit the program */
    			case SDL_MOUSEBUTTONDOWN:
   				   UIRoot_Execute(&initData.uiData,LEFT_CLICK,&event);
             printf("execute\n");
   				   break;
    			case SDL_QUIT:
    				exit(0);
    				break;
    		}
    	}
      menuRunning = (!initData.uiData.root->child->actions[1].status);
      //menuRunning = !&initData.uiData.root->child->actions[1].status;
   } /*delay 2s first*/

   fadeOutMusic(&initData.audioData);
   /*stopMusic(&initData.audioData, 0);*/
   gameStart(graphicsData,audioData);
   return 0;
}

/*
int game_welcome_loop(){
    the game_welcome_page would only response for up arrow, down arrow, enter, click on start, continue or tutorial

   return 0;
}
*/
