/* This file is close to the bare minimum needed to run a working SDL application.
   It perform 6 steps:
   1. It will initialise SDL
   2. It will create an SDL_Window
   3. It will set the background colour of the window to a colour
   4. It will load and place an image.
   5. It will loop until a key is pressed.
   6. It will then close.
   I will indicate which lines do what */

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

/* If you want to use a different window colour, google for HTML colour codes,
   which are also 6-digit hexidecimal numbers (though you will need to change
   the # to a 0x e.g. #b00000 goes to 0xb00000) */
#define COLOUR 0xb00000
/* This is the name that will appear in the top bar of the application window */
#define NAME "bareMinimum"
/* This is the width and height that we will create our window with */
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

/* This is the path to the image we will load. The path is relative to wherever
   the final, compiled program is run from (in this case, the bin_x64 folder) */
#define IMAGE_PATH "images/blueFlower.bmp"
/* We need to know where on the screen we want to draw the image */
#define IMAGE_X 100
#define IMAGE_Y 100
/* We also need to know how tall and wide an image is in pixels - for the
   default image, this is 20 x 20 pixels */
#define IMAGE_HEIGHT 10
#define IMAGE_WIDTH 10

/* In windows, SDL needs to have a main function that takes these arguments,
   because it actually uses some trickery to run some important functions before
   starting your program. It then calls main(), but always passes in argc and
   argv, so not having these here will make your program not compile! */
int main(int argc, char *argv[]){
  /* I will explain what these variables are for as we go along */
  SDL_Window *window;
  SDL_Surface *image;
  SDL_Rect rectangle;
  SDL_Event event;

  /* If you think of your program as working with data in multiple layers:
     Screen <-> Operating System <-> SDL <-> Your Code
     Then before we can push information to the Operating system, which will
     then handle making the data display on the Screen, SDL needs to know how to
     make the connection between itself and the operating system. Otherwise, we
     just push information into the SDL layer and it will never be able to make
     the screen change.

     To do this, we call SDL_Init. This is a function defined in SDL itself, so
     there won't be a direct definition or declaration here in this code. For
     most projects, there should be no functions, structs or defines that start
     with SDL_(name) just to prevent confusion. In this code, at least, anything
     that starts SDL_(name) is coming straight from SDL and is not something I've
     made.

     SDL_Init takes one argument, which is the flags for which subsystem(s) you
     want to enable. For example, here we will just use SDL_INIT_VIDEO, which is
     #define'd somewhere in SDL itself. If we wanted to initialise video and
     audio systems, we would pass SDL_INIT_VIDEO|SDL_INIT_AUDIO as an argument.

     This works to combine them with a bitwise OR operation (compare how || in
     an if means 'or' to | meaning bitwise OR), which you can learn about in the
     Software Architecture lectures and notes. For our purposes here, it
     combines them by magic.

     SDL_Init returns 0 if it works ok. I haven't see it fail, but if it does,
     we should exit the program before bad things happen. We don't need to keep
     this integer, so I'm not going to use a variable for it.*/

   if(SDL_Init(SDL_INIT_VIDEO) != 0){
     return(1);
   }

  /* Once SDL is initialised, the next step is to create a window. A window is
     the part of the application that you can move around on the screen, with a
     title bar and everything.

     The SDL_CreateWindow function takes a few arguments:
     1. int   - The X position of the top left corner of the window at creation.
                You can also use a #define from SDL called
                SDL_WINDOWPOS_UNDEFINED to create it wherever the operating
                system decides.
     2. int   - The Y position of the top left corner of the window at creation.
                SDL uses the Y axis running DOWN the screen rather than up as
                you mathematically inclined people might expect. You can also
                use SDL_WINDOWPOS_UNDEFINED to create it wherever.
     3. int   - The width of the window in pixels.
     4. int   - the height of the window in pixels
     5. flags - these flags determine certain things about the window, e.g.
                SDL_WINDOW_RESIZABLE makes the window resizable, and are
                combined using the bitwise OR like in SDL_Init

     SDL_CreateWindow returns a pointer to a window rather than the window
     struct itself. This doesn't matter too much to us, as we never really
     modify the window directly ourself and just pass it to functions, but this
     is why we made the 'window' variable at the top of main a pointer rather
     than just a plain version of SDL_Window.*/
  window = SDL_CreateWindow(
    "An SDL2 window",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    640,
    480,
    SDL_WINDOW_OPENGL);

  /* Even though we have a window, it will not actually have anything
     determining its appearance. In fact, until you specifically tell it to
     change, it will keep the pixels on the screen at whatever they were when
     the window was originally created!

     This is an important thing about SDL: the pixels on your screen will not
     change until you specifically tell SDL to push the data to your operating
     system and let that put the pixels on the screen.

     SDL_FillRect is the most simple function for setting a rectangular area to
     a specific colour. It takes 3 arguments:
     1. SDL_Surface * - A pointer to the SDL_Surface that we will be drawing on.
     2. SDL_Rect *    - A pointer to the rectangular area that we will be
                        filling with colour. If instead, we pass NULL here, we
                        fill the whole surface which is a nice shortcut.
     3. int           - The integer code for the colour we want. There's a sort
                        of convention that we use 6-bit hexidecimal numbers for
                        this.

     SDL_Surface is a versatile class that forms the basis of much of what we
     will need to do with SDL. It represents an array of pixels, with set colours
     and so on, which we can both use to write data to and read data from. In
     this case, we are getting the SDL_Surface which the main application window
     is representing using SDL_GetWindowSurface(), which just takes a pointer to
     an SDL_Window as an argument.

     SDL_Rect is a very simple struct, which just has integers x,y,w and h in,
     representing the x,y coordinates of the top left square and the width
     and height of the rectangle. Remember that the Y-axis is upside down in
     SDL! */

   SDL_FillRect(SDL_GetWindowSurface(window),
                NULL,
                COLOUR);

  /* Loading a BMP in SDL is straightforward. We just use SDL_LoadBMP(), which
     just takes the path to the .bmp we want to load.

     It gives us the pointer to an SDL_Surface object which contains all the
     information which we need to draw the image onto other SDL_Surfaces, such
     as the one on the main application window. */

  image = SDL_LoadBMP("images/blueFlower.bmp");

  /* Then we need to make the SDL_Rect that is going to be used to draw our
     image into the right size, which here is just copying in the #defines from
     above.

     Also, we write in the position we want to the have the rectangle drawn at
     by adjust the .x and .y values of it, which here means setting them to the
     #defined values above.*/

  rectangle.w = 50;
  rectangle.h = 50;
  rectangle.x = IMAGE_X;
  rectangle.y = IMAGE_Y;

  /* Once we have an image and a rectangle that we are happy with, we need to
     draw it onto the screen. We do this by calling the function SDL_BlitSurface,
     which takes 4 arguments:
     1. SDL_Surface * - a pointer to the surface which are drawing from
     2. SDL_Rect *    - a pointer to the rectangle representing what area of the
                        image we are drawing from we want to draw. If we pass
                        NULL, we'll copy the whole thing.
     3. SDL_Surface * - a pointer to the surface we are drawing on to.
     4. SDL_Rect      - a pointer to the rectangle representing where on the
                        surface we are drawing to we want the drawing to happen.*/

  SDL_BlitSurface(image,
                  NULL,
                  SDL_GetWindowSurface(window),
                  NULL);

  /* Every time we decide that we want to make the display on the screen change
     to what we've told it to look like (in this case, after we've told it to
     change colour), we need to call SDL_UpdateWindowSurface(). This takes the
     pointer to the window we want to update - in this case it's just the window
     variable we stored from earlier. */

  SDL_UpdateWindowSurface(window);

  /* Because we want the program to stay open, we need to use a loop to keep it
     running, otherwise, the program will simply exit once it creates the window,
     probably even before you can see what's on it!

     In proper code, you should probably never use an infinite loop like this,
     but it works here for what I'm trying to show you.*/

  while(1){
  /* For many SDL applications, especially where the graphics on the screen need
     to change with each frame, you'll need to do two steps.

     The first is to make the whole screen return to some default state, which
     in this case is the colour we #define'd back up at the top. If we didn't do
     this, we'd potentially end up with loose pixels from where we draw in images
     that move with each frame. */

     SDL_FillRect(SDL_GetWindowSurface(window),
                  NULL,
                  COLOUR);

  /* Once the screen has been reset to a default state, we'd then need to draw
     in all the images and other objects that we want to display on the screen.
     SDL only does this if we specifically tell it to, it doesn't have any ways
     of keeping an image on the screen beyond redrawing it every frame */


     SDL_BlitSurface(image,
                     NULL,
                     SDL_GetWindowSurface(window),
                     NULL);

  /* Because we've updated the graphics we want to show on the screen, we'll
     need to call SDL_UpdateWindowSurface() again to make sure that the window
     of our program displays the latest data we've drawn in. If we don't do this,
     the window will never seem to change even though we're adjusting the data
     that we're pushing in. */

     SDL_UpdateWindowSurface(window); 

  /* Whilst the loop is running, we need to make sure that the SDL application
     can still respond to things like being clicked on, otherwise it will seem
     to the operating system that it has crashed.

     We do this by calling SDL_PollEvent(), which grabs the first event in the
     automatic queue that SDL makes for you. For this to work, you need to pass
     in a pointer to an SDL_Event struct so the data can be written safely, and
     the SDL_PollEvent returns 1 if there are any events left or 0 if there are
     no events left to check.

     One of the simplest ways of making sure we're always responding to events
     is to have another loop here, checking whether we've looked at all the
     events that are happening this run through */

     while(SDL_PollEvent(&event)){
       /* SDL_Event structs are designed to have multiple different categories
          in them, which allows them to be flexibly used for all sorts of
          different events (e.g. mouse clicking vs keyboard input).

          Here, we're going to respond to two different events using a switch on
          event.type, which is technially an enum inside the SDL_Event struct.

          The first we're goingt to use here is any keyboard button being pressed,
          and the second is SDL_Quit, which is the event triggered if someone
          closes the application's window. Both of these will close SDL and end
          the program. */
          switch(event.type){
            case SDL_QUIT:
              /* SDL_Quit() takes no arguments, and should be called at the end
                 of your program, and it just undoes whatever SDL_Init has done.
                 In theory, nothing bad will happen if you don't, but better to
                 be safe than sorry! */
              SDL_Quit();
              exit(0);
              break;
            case SDL_KEYDOWN:
              SDL_Quit();
              exit(0);
              break;
          }
     }

  }

  return(0);

}
