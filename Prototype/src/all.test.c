#include "all.test.h"


/* ================================================= */
/* ========== START OF GENERIC TEST SUITE ========== */
/* ================================================= */


/* Test passes! */
START_TEST(core_isPointInRect){
    SDL_Rect rect;

    rect.x = rect.y = 20;
    rect.h = rect.w = 40;
/* Returns '1' if the point is inside the rectangle. */
    fail_unless(isPointInRect(21,21,rect) == 1, "isPointInRect function test failed!");
    fail_unless(isPointInRect(21,59,rect) == 1, "isPointInRect function test failed!");
    fail_unless(isPointInRect(59,21,rect) == 1, "isPointInRect function test failed!");
  }
END_TEST


/* Test passes! */
START_TEST(limits_isPointInRect){
    SDL_Rect rect;

    rect.x = rect.y = 20;
    rect.h = rect.w = 40;
/* Returns '0' if the point isn't inside the rectangle. */
    fail_unless(isPointInRect(10,10,rect) == 0, "isPointInRect function test failed!");
    fail_unless(isPointInRect(10,21,rect) == 0, "isPointInRect function test failed!");
    fail_unless(isPointInRect(21,10,rect) == 0, "isPointInRect function test failed!");
    fail_unless(isPointInRect(21,61,rect) == 0, "isPointInRect function test failed!");
    fail_unless(isPointInRect(61,21,rect) == 0, "isPointInRect function test failed!");
  }
END_TEST


/* Test passes! */
START_TEST(core_testRectIntersection){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w = 40;
    rectA.h = 30;

    rectB.x = 59;
    rectB.y = 49;
    rectB.w = rectB.h = 40;


    fail_unless(testRectIntersection(rectA, rectB) == 1, "testRectIntersection function test failed!");

  }
END_TEST

/* Test passes! */
START_TEST(limits_testRectIntersection){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w = 40;
    rectA.h = 30;

    rectB.x = 55;
    rectB.y = 61;
    rectB.w = rectB.h = 40;
    fail_unless(testRectIntersection(rectA, rectB) == 0, "testRectIntersection function test failed!");

    rectB.x = 61;
    fail_unless(testRectIntersection(rectA, rectB) == 0, "testRectIntersection function test failed!");

  }
END_TEST





/* Test passes! */
START_TEST(core_isRectEnclosedInRect){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 25;
    rectA.w = 30;
    rectA.h = 26;

    rectB.x = rectB.y = 20;
    rectB.w = 40;
    rectB.h = 30;

    fail_unless(isRectEnclosedInRect(rectA, rectB) == 0, "isRectEnclosedInRect function test failed!");

    rectA.h = 25;
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "isRectEnclosedInRect function test failed!");

    rectA.h = 24;
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "isRectEnclosedInRect function test failed!");

}
END_TEST


/* Test passes! */
/* ensureRectEnclosed: assumption: size of ensure is always smaller than size of limit */
START_TEST(core_ensureRectEnclosed){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w=40;
    rectA.h=30;

    rectB.x = rectB.y=25;
    rectB.w=40;
    rectB.h=36;

    ensureRectEnclosed(&rectA, &rectB); /* the function is a void function, thus the effect is to be checked */
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "ensureRectEnclosed function test failed!");

    rectB.x = rectB.y=15;
    rectB.w=40;
    rectB.h=36;
    ensureRectEnclosed(&rectA, &rectB); /* the function is a void function, thus the effect is to be checked */
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "ensureRectEnclosed function test failed!");

  }
END_TEST


/* Test passes! */
/* the function generates a random number. the testing is to ensure the number is within range */
START_TEST(core_randPi){
    fail_unless(randPi() <= PI, "randPi function test failed!, random number greater than PI");
    fail_unless(randPi() >= 0, "randPi function test failed!, random number less than 0");
  }
END_TEST


/* Test passes! */
START_TEST(core_square){
    fail_unless(square(1.5) == 2.25, "square function test failed!, for a non-integer case");
    fail_unless(square(2) == 4, "square function test failed!, for an integer case");
  }
END_TEST


/* Test passes! */
START_TEST(core_getDistance2BetweenPoints){
    fail_unless(getDistance2BetweenPoints(1.5, 1.5, 2.5, 1.5) == 1, "getDistance2BetweenPoints function test failed!, for a one dimension case");
    fail_unless(getDistance2BetweenPoints(1, 1, 4, 5) == 25, "getDistance2BetweenPoints function test failed!, for a two dimensions case");
  }
END_TEST

/* Test passes! */
START_TEST(core_fitRectToWorld){
   SDL_Rect rectA;
   rectA.x = rectA.y = 895;
   rectA.w = 222;
   rectA.h = 11;


   fitRectToWorld(&rectA);
   fail_unless(rectA.x + rectA.w <= X_SIZE_OF_WORLD, "fitRectToWorld function test failed!");
   fail_unless(rectA.x >= 0, "fitRectToWorld function test failed!");
   fail_unless(rectA.y + rectA.h <= Y_SIZE_OF_WORLD, "fitRectToWorld function test failed!");
   fail_unless(rectA.y >= 0, "fitRectToWorld function test failed!");
}
END_TEST


/* Test passes! */
START_TEST(core_generateRandomCoordOffset){
    /* the offset is only half of radius, please double check */
    /* the function generates a random number. the testing is to ensure the number is within range*/
    fail_unless(generateRandomCoordOffset(5) <= 5.0/2, "randPi function test failed!, random number greater than PI");
    fail_unless(generateRandomCoordOffset(5) >= -5.0/2, "randPi function test failed!, random number less than 0");
  }
END_TEST

START_TEST(core_getDistance2BetweenRects){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w = 40;
    rectA.h = 30;

    rectB.x = rectB.y = 20;
    rectB.w = 46;
    rectB.h = 38;
    fail_unless(getDistance2BetweenRects(rectA, rectB) == 25, "getDistance2BetweenRects function test failed!");
  }
END_TEST




/* Test passes! */
/* the function should return angle between the centre of 2 rectangles */
START_TEST(core_getAngleBetweenRects){
    SDL_Rect rectA;
    SDL_Rect rectB;
    double val = 180.0/PI;

    rectA.x = rectA.y = -1;
    rectA.w = rectA.h = 2;

    rectB.x = rectB.y = 0;
    rectB.w = -12;
    rectB.h = 12;
    fail_unless((int)(getAngleBetweenRects(&rectA, &rectB) * val) == 135, "getAngleBetweenRects function test failed!");
  }
END_TEST


/* Test passes!*/
START_TEST(core_getCenterOfRect){
    SDL_Rect rect;
    SDL_Point point;

    rect.x = rect.y = -1;
    rect.w=4;
    rect.h=6;
    point = getCenterOfRect(rect);
    fail_unless(pow((point.x-1),2)+pow((point.y-2),2) == 0, "getCenterOfRect function test failed!");
  }
END_TEST

/* Test passes!*/
START_TEST(core_getRectFromInvRect){
   SDL_Window *window;
   SDL_Rect rect;
   int from_left, from_right, from_top, from_bot;
   from_left=-2;
   from_right=1;
   from_top=-4;
   from_bot=3;
   window = SDL_CreateWindow(
      "An SDL2 window",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      640,
      480,
      SDL_WINDOW_OPENGL
   );

   rect = getRectFromInvRect(window, from_left, from_top, from_right, from_bot);
   fail_unless(rect.x == -2+640, "getRectFromInvRect function test failed1 %d", rect.x);
   fail_unless(rect.y == -4+480, "getRectFromInvRect function test failed2 %d", rect.y);
   fail_unless(rect.w == (640-1), "getRectFromInvRect function test failed3 %d", rect.w);
   fail_unless(rect.h == (480-3), "getRectFromInvRect function test failed4 %d", rect.h);

   SDL_DestroyWindow(window);


  }
END_TEST

/* Test passes!*/
START_TEST(core_getRectFromPercRect){
   SDL_Window *window;
   SDL_Rect rect;
   float from_left, from_right, from_top, from_bot;
   from_left=0.2;
   from_right=0.3;
   from_top=0.4;
   from_bot=0.5;
   window = SDL_CreateWindow(
      "An SDL2 window",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      640,
      480,
      SDL_WINDOW_OPENGL
   );

   rect = getRectFromPercRect(window, from_left, from_top, from_right, from_bot);
   fail_unless(rect.x == 640*0.2, "getRectFromPercRect function test failed");
   fail_unless(rect.y == 480*0.4, "getRectFromPercRect function test failed");
   fail_unless(rect.w == 640*0.3, "getRectFromPercRect function test failed");
   fail_unless(rect.h == 480*0.5, "getRectFromPercRect function test failed");

   SDL_DestroyWindow(window);

  }
END_TEST

/* Test passes!*/
START_TEST(core_fileToString){
/* testing method: generate a new text file with specified content, read the content, and delete the file*/
   FILE *fp;
   char *ch=NULL;
   char filename[]="this_is_test_file.txt";
   fp=fopen(filename, "w");
   fprintf(fp, "testing words");

   fclose(fp);
   fp=fopen(filename, "r");

   ch = fileToString(fp);
   fclose(fp);
   remove(filename);

   fail_unless(strcmp(ch, "testing words") == 0, "fileToString function test failed");

  }
END_TEST

/* Test passes!*/
START_TEST(core_getPointFromInvPoint){
   SDL_Window *window;
   SDL_Point point;
   int x,y;
   x=-120;
   y=-150;
   window = SDL_CreateWindow(
      "An SDL2 window",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      640,
      480,
      SDL_WINDOW_OPENGL
   );

   point = getPointFromInvPoint(window, x, y);
   fail_unless(point.x == -120+640, "getPointFromInvPoint function test failed");
   fail_unless(point.y == -150+480, "getPointFromInvPoint function test failed");
   SDL_DestroyWindow(window);

  }
END_TEST

/* Test passes!*/
START_TEST(core_getPointFromPerc){

   SDL_Window *window;
   SDL_Point point;
   float x,y; /*x and y are in range of 0 to 1*/
   x=0.2;
   y=0.3;
   window = SDL_CreateWindow(
      "An SDL2 window",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      640,
      480,
      SDL_WINDOW_OPENGL
   );

   point = getPointFromPerc(window, x, y);
   fail_unless(point.x == 640*0.2, "getPointFromPerc function test failed");
   fail_unless(point.y == 480*0.3, "getPointFromPerc function test failed");

   SDL_DestroyWindow(window);

   }
END_TEST

/* Test passes!*/
START_TEST(core_shrinkRectToFit){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectB.x = 0;
    rectA.y = rectB.y = 0;
    rectA.w = rectB.w = 25;
    rectA.h = rectB.h = 36;

    rectB.h = 35;
    shrinkRectToFit(&rectA, &rectB);

    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "shrinkRectToFit function test failed");

    rectA.w = 27;
    shrinkRectToFit(&rectA, &rectB);
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "shrinkRectToFit function test failed");


   }
END_TEST





/* ================================================= */
/* ========= START OF CONTROLS TEST SUITE ========== */
/* ================================================= */


/* Test segfaults. Matt's input: "SDL_Events are weird." */
/* From controls.h */

/* Test passes!*/
START_TEST(test_handleEvent){
    SDL_Event event;
    GameData gameData;
    SDL_Init(SDL_INIT_VIDEO);
    /* see startGame in game.c */

    event.type = SDL_KEYDOWN;
    event.key.type = SDL_KEYDOWN;
    event.key.state = SDL_PRESSED;

    fail_unless(handleEvent(&event,&gameData.gameObjectData,&gameData.uiData,&gameData.controlsData, &gameData.graphicsData) == 1, "handleEvent function failed.");

    event.key.type = 0;
    event.key.state = 0;
    event.type = SDL_QUIT;
    event.quit.type = SDL_QUIT;

    fail_unless(handleEvent(&event,&gameData.gameObjectData,&gameData.uiData,&gameData.controlsData, &gameData.graphicsData) == 0, "handleEvent function failed.");
    SDL_Quit();
  }
END_TEST

/* Test passes!*/
START_TEST(test_keydown){
    SDL_Event event;
    GameData gameData;
    SDL_Init(SDL_INIT_VIDEO);
    /* see startGame in game.c */

    event.type = SDL_KEYDOWN;
    event.key.type = SDL_KEYDOWN;
    event.key.state = SDL_PRESSED;

    fail_unless(keydown(&gameData.controlsData, &gameData.gameObjectData,&gameData.graphicsData, &gameData.uiData,&event) == 1, "keydown function failed.");
    SDL_Quit();

  }
END_TEST

/* Test passes!*/
START_TEST(test_keyup){
    SDL_Event event;
    GameData gameData;
    SDL_Init(SDL_INIT_VIDEO);
    /* see startGame in game.c */

    event.type = SDL_KEYUP;
    event.key.type = SDL_KEYUP;
    event.key.state = SDL_RELEASED;

    fail_unless(keyup(&gameData.controlsData, &gameData.gameObjectData,&gameData.uiData,&event) == 1, "keyup function failed.");
    SDL_Quit();

  }
END_TEST

START_TEST(test_panScreen){
    SDL_Event event;
    GameData gameData;
    int delta_t = 1;
    SDL_Init(SDL_INIT_VIDEO);
    /* see startGame in game.c */

    gameData.controlsData.keys[ARROW_RIGHT] = 1;
    gameData.graphicsData.navigationOffset.x = 50;

    /*fail if cannot panScreen*/
    panScreen(&gameData.graphicsData, &gameData.controlsData, delta_t);
    fail_unless(gameData.graphicsData.navigationOffset.x == 0, "panScreen function failed. %d", gameData.graphicsData.navigationOffset.x);

    SDL_Quit();

  }
END_TEST

START_TEST(test_initControlData){
    SDL_Event event;
    GameData gameData;
    int total, i;
    SDL_Init(SDL_INIT_VIDEO);
    /* see startGame in game.c */

    initControlData(&gameData.controlsData);

    total = 0;
    for(i=0; i<MAX_KEYS; i++){
      total += gameData.controlsData.keys[i];
   }
    fail_unless(total == 0, "initControlData function failed.");

    /*fail if cannot perform the function initControlData*/
    SDL_Quit();

  }
END_TEST






Suite *makeMasterSuite(void)
{
  Suite *master = suite_create ("Master");
  return master;
}


Suite *makeSuiteForGeneric(void)
{
  Suite *generic = suite_create ("Generic");;
  TCase *core = tcase_create("core");

  /* Adds tests to the 'core' testcase. */
  tcase_add_test(core, core_isPointInRect);
  tcase_add_test(core, limits_isPointInRect);
  tcase_add_test(core, core_testRectIntersection);
  tcase_add_test(core, limits_testRectIntersection);
  tcase_add_test(core, core_isRectEnclosedInRect);
  tcase_add_test(core, core_ensureRectEnclosed);
  tcase_add_test(core, core_randPi);
  tcase_add_test(core, core_square);
  tcase_add_test(core, core_getDistance2BetweenPoints);
  tcase_add_test(core, core_fitRectToWorld);
  tcase_add_test(core, core_generateRandomCoordOffset);
  tcase_add_test(core, core_getDistance2BetweenRects);
  tcase_add_test(core, core_getAngleBetweenRects);
  tcase_add_test(core, core_getCenterOfRect);

  tcase_add_test(core, core_getRectFromInvRect);
  tcase_add_test(core, core_getRectFromPercRect);
  tcase_add_test(core, core_fileToString);
  tcase_add_test(core, core_getPointFromInvPoint);
  tcase_add_test(core, core_getPointFromPerc);
  tcase_add_test(core, core_shrinkRectToFit);

  /* Adds core to the testcases in the suite. */
  suite_add_tcase(generic, core);

  return generic;
}


Suite *makeSuiteForControls(void)
{
  Suite *controls = suite_create ("Controls");
  TCase *core = tcase_create("core");

  /* Adds tests to the 'core' testcase. */
  tcase_add_test(core, test_handleEvent);
  tcase_add_test(core, test_keydown);
  tcase_add_test(core, test_keyup);
  tcase_add_test(core, test_panScreen);
  tcase_add_test(core, test_initControlData);

  /* Adds tc_core to the testcases in the suite. */
  suite_add_tcase(controls, core);

  return controls;
}


int main(void)
{
  int number_failed;
  SRunner *theRunner;

  /* Creates a suite runner, which is aware of all suites added into it.
   * MasterSuite is an empty suite just to allow theRunner to be allocated. */
  theRunner = srunner_create(makeMasterSuite());

  /* Adds suites into theRunner's list of suites to run. */
  srunner_add_suite (theRunner, makeSuiteForGeneric());
/*  srunner_add_suite (theRunner, makeSuiteForAI());
  srunner_add_suite (theRunner, makeSuiteForAnnouncements());
  srunner_add_suite (theRunner, makeSuiteForAudio());
  srunner_add_suite (theRunner, makeSuiteForConfiguration());*/
  srunner_add_suite (theRunner, makeSuiteForControls());
/*  srunner_add_suite (theRunner, makeSuiteForGame());
  srunner_add_suite (theRunner, makeSuiteForGame_objects());
  srunner_add_suite (theRunner, makeSuiteForGraphics());
  srunner_add_suite (theRunner, makeSuiteForInit());
  srunner_add_suite (theRunner, makeSuiteForMain());
  srunner_add_suite (theRunner, makeSuiteForUI());
  srunner_add_suite (theRunner, makeSuiteForWorld_generation());*/

  /* Runs all the suites in theRunner's list of suites.
   * 'CK_VERBOSE' prints even passes while 'CK_NORMAL' prints just failures. */
  srunner_run_all(theRunner, CK_VERBOSE);
  number_failed = srunner_ntests_failed(theRunner);
  srunner_free(theRunner);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
