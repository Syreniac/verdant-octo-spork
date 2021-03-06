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
  	fprintf(stderr,"Could not initialize SDL: %s\n", SDL_GetError());
	fflush(stderr);
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
	/*Audio needs to be initialized at the very start too.*/
    TTF_Init();

  initData.graphicsData = createGraphicsData();

	audioSystem(&initData.audioData);


	loadMusic("sound/music01.wav" , 0, &initData.audioData);

	loadMusic("sound/music02.wav" , 1, &initData.audioData);

	loadMusic("sound/music03.wav" , 1, &initData.audioData);

	/*loadMusic("sound/music04.wav" , 1, &initData.audioData);*/
	
	loadSoundEffect("sound/returnFlower.wav", "returnFlower", &initData.audioData);

	loadSoundEffect("sound/thunder.wav", "thunder", &initData.audioData);

	loadSoundEffect("sound/garryScream.wav", "garryScream", &initData.audioData);

	loadSoundEffect("sound/applause.wav", "applause", &initData.audioData);

	loadSoundEffect("sound/winterComing.wav", "winterComing", &initData.audioData);
	
	
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
	AudioSettings->soundEffect = NULL;

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
   SDL_Event event;
   UI_Element *element;
   UI_Element *element2;

   int win_x,win_y;

   initData.graphicsData = graphicsData;
   initData.audioData = audioData;

/*   gameData.graphicsData = graphicsData; */

   /* create a black background*/


   /* create box 1 (title), size fixed*/

   SDL_GetWindowSize(graphicsData.window,&win_x,&win_y);

   initUIData(&initData.uiData);

   initData.uiData.graphicsData = &initData.graphicsData;

   initData.uiData.root = UIElement_Create(0,0,win_x,win_y,2);
   UIConfigure_DisplayImage(initData.uiData.root,&initData.uiData.root->actions[0],graphicsData.mainMenuImage);
   UIConfigure_PercOffsetRect(initData.uiData.root,&initData.uiData.root->actions[1],0.0,0.0,1.0,1.0,
                                                                                     0,  0,  0,  0,  0);

   element = UIElement_Create(0,0,0,0,3);
   UIConfigure_Counter(element,&element->actions[0],1,0);
   UIConfigure_LeftClickRect(element,&element->actions[1]);
       UITrigger_Bind(&element->actions[1],&element->actions[0],-1,UITRIGGER_PLUSONE);
   UIConfigure_PercOffsetRect(element,&element->actions[2],0.55, 0.69, 0.9, 0.82,
                                                           0,    0,  0,0,0);

   UIElement_Reparent(element,initData.uiData.root);

    /*element = UIElement_Create((win_x *3)/4 - 100, (win_y * 3)/4, 150, 50, 1);
    UIConfigure_FillRect(element,&element->actions[0],100,100,0);
    UIElement_Reparent(element,initData.uiData.root);*/

    element2 = UIElement_Create((win_x - 30), win_y - 30, 30, 30, 4);
    UIConfigure_FillRect(element2, &element2->actions[0],228,240,3);
	UIConfigure_PercPosition(element2, &element2->actions[1],1.0,1.0,-30,-30,0);
	UIConfigure_LeftClickRect(element2, &element2->actions[2]);
		UITrigger_Bind(&element2->actions[2],&element2->actions[3],0,1);
	UIConfigure_MuteSound(element2,&element2->actions[3]);
    UIElement_Reparent(element2,initData.uiData.root);
	UIRoot_Pack(&initData.uiData);

	initData.uiData.audioData = &initData.audioData;

   playMusic(&initData.audioData,0);

   while(menuRunning){

      /*UIRoot_Execute(&initData.uiData,UPDATE,0);*/

      //paintBackground(&initData.graphicsData,0,200,100);
      UIRoot_Execute(&initData.uiData,UPDATE,0);
      SDL_RenderPresent(initData.graphicsData.renderer);
    	while (SDL_PollEvent(&event))
    	{
        initData.uiData.event = &event;
    		switch (event.type)
    		{
  				case SDL_WINDOWEVENT:
  					switch (event.window.event){
  						case SDL_WINDOWEVENT_RESIZED:
  							UIRoot_Execute(&initData.uiData,WINDOW_RESIZE,0);
  							break;
  					}
            break;
    			/* Closing the Window will exit the program */
    			case SDL_MOUSEBUTTONDOWN:
   				   UIRoot_ExecuteUpwards(&initData.uiData,LEFT_CLICK,1);
   				   break;
    			case SDL_QUIT:
    				exit(0);
    				break;
    		}
    	}
		if (Mix_Playing(1) == 0) {
			playMusic(&initData.audioData,0);
		}
      menuRunning = (!initData.uiData.root->child->actions[0].status);
   }

   UIRoot_Destroy(&initData.uiData);

   fadeOutMusic(&initData.audioData);

   return 0;
}

/*
int game_welcome_loop(){
    the game_welcome_page would only response for up arrow, down arrow, enter, click on start, continue or tutorial

   return 0;
}
*/
