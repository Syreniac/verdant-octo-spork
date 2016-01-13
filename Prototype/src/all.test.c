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
   srand(time(NULL));
   rectA.x = rectA.y = rand()%RAND_MAX;
   rectA.w = rand()%RAND_MAX;
   rectA.h = rand()%RAND_MAX;


   fitRectToWorld(&rectA);
   fail_unless(rectA.x + rectA.w <= X_SIZE_OF_WORLD, "fitRectToWorld function test failed!");
   fail_unless(rectA.x >= 0, "fitRectToWorld function test failed 2!");
   fail_unless(rectA.y + rectA.h <= Y_SIZE_OF_WORLD, "fitRectToWorld function test failed!");
   fail_unless(rectA.y >= 0, "fitRectToWorld function test failed 4!");
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


/* Test passes! */
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

/* Test passes! tbc*/
START_TEST(core_getRectFromInvRect){

  }
END_TEST

/* Test passes! tbc*/
START_TEST(core_getRectFromPercRect){

  }
END_TEST

/* Test passes! tbc*/
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

/* Test passes! tbc*/
START_TEST(core_getPointFromInvPoint){

  }
END_TEST

/* Test passes! tbc*/
START_TEST(core_getPointFromPerc){

  }
END_TEST

/* Test passes! tbc*/
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


    srand(time(NULL));

    rectB.x = rand()%RAND_MAX;
    rectB.y = rand()%RAND_MAX;
    rectB.w = rand()%RAND_MAX;
    rectB.h = rand()%RAND_MAX;

    shrinkRectToFit(&rectA, &rectB);
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "shrinkRectToFit function test failed 1");




  }
END_TEST





/* ================================================= */
/* ========= START OF CONTROLS TEST SUITE ========== */
/* ================================================= */


/* Test segfaults. Matt's input: "SDL_Events are weird." */
/* From controls.h */
START_TEST(test_handleEvent){
    SDL_Event event;
    GameData gameData;

    SDL_Init(SDL_INIT_VIDEO);
    /* see startGame in game.c */

    event.type = SDL_KEYDOWN;
    event.key.type = SDL_KEYDOWN;
    event.key.state = SDL_PRESSED;

    fail_unless(handleEvent(&event,&gameData.gameObjectData,&gameData.uiData,&gameData.controlsData, &gameData.graphicsData) == 1);

    event.key.type = 0;
    event.key.state = 0;
    event.type = SDL_QUIT;
    event.quit.type = SDL_QUIT;

    fail_unless(handleEvent(&event,&gameData.gameObjectData,&gameData.uiData,&gameData.controlsData, &gameData.graphicsData) == 0);
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
  srunner_add_suite (theRunner, makeSuiteForControls());

  /* Runs all the suites in theRunner's list of suites.
   * 'CK_VERBOSE' prints even passes while 'CK_NORMAL' prints just failures. */
  srunner_run_all(theRunner, CK_VERBOSE);
  number_failed = srunner_ntests_failed(theRunner);
  srunner_free(theRunner);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
