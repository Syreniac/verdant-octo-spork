#include <check.h>
#include "../src/main.h"

#define VALID_HEXFILE "assets/test.m7"


/* Test passes! */
/* From generic.h */
START_TEST(test_isPointInRect){
    SDL_Rect rect;

    rect.x = rect.y = 20;
    rect.h = rect.w = 40;

    fail_unless(isPointInRect(10,10,rect) == 0, "isPointInRect function borked");
    fail_unless(isPointInRect(21,21,rect) == 1, "isPointInRect function borked");
    fail_unless(isPointInRect(10,21,rect) == 0, "isPointInRect function borked");
    fail_unless(isPointInRect(21,10,rect) == 0, "isPointInRect function borked");
    fail_unless(isPointInRect(21,59,rect) == 1, "isPointInRect function borked");
    fail_unless(isPointInRect(59,21,rect) == 1, "isPointInRect function borked");
    fail_unless(isPointInRect(21,61,rect) == 0, "isPointInRect function borked");
    fail_unless(isPointInRect(61,21,rect) == 0, "isPointInRect function borked");
  }
END_TEST

/* From generic.h */
START_TEST(test_testRectIntersection){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w = 40;
    rectA.h = 30;

    rectB.x = 55;
    rectB.y = 61;
    rectB.w = rectB.h = 40;

    fail_unless(testRectIntersection(rectA, rectB) == 1, "testRectIntersection function borked");

    rectB.x = 61;
    fail_unless(testRectIntersection(rectA, rectB) == 0, "testRectIntersection function borked");

    rectB.y = 49;
    fail_unless(testRectIntersection(rectA, rectB) == 1, "testRectIntersection function borked");

  }
END_TEST


/* From generic.h */
START_TEST(test_isRectEnclosedInRect){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w = 40;
    rectA.h = 30;

    rectB.x = rectB.y = 25;
    rectB.w = 30;
    rectB.h = 26;

    fail_unless(isRectEnclosedInRect(rectA, rectB) == 0, "isRectEnclosedInRect function borked");

    rectB.h = 25;
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "isRectEnclosedInRect function borked");

    rectB.h = 24;
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "isRectEnclosedInRect function borked");

}
END_TEST

/* From generic.h */
/*ensureRectEnclosed: assumption: size of ensure is always smaller than size of limit*/
START_TEST(test_ensureRectEnclosed){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w=40;
    rectA.h=30;

    rectB.x = rectB.y=25;
    rectB.w=40;
    rectB.h=36;

    ensureRectEnclosed(&rectA, &rectB); /* the function is a void function, thus the effect is to be checked */
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "ensureRectEnclosed function borked");

    rectB.x = rectB.y=15;
    rectB.w=40;
    rectB.h=36;
    ensureRectEnclosed(&rectA, &rectB); /* the function is a void function, thus the effect is to be checked */
    fail_unless(isRectEnclosedInRect(rectA, rectB) == 1, "ensureRectEnclosed function borked");

  }
END_TEST


/* From generic.h */
/* the function generates a random number. the testing is to ensure the number is within range */
START_TEST(test_randPi){
    fail_unless(randPi() <= PI, "randPi function borked, random number greater than PI");
    fail_unless(randPi() >= 0, "randPi function borked, random number less than 0");
  }
END_TEST


/* From generic.h */
START_TEST(test_square){
    fail_unless(square(1.5) == 2.25, "square function borked, for a non-integer case");
    fail_unless(square(2) == 4, "square function borked, for an integer case");
  }
END_TEST


/* From generic.h */
START_TEST(test_getDistance2BetweenPoints){
    fail_unless(getDistance2BetweenPoints(1.5, 1.5, 2.5, 1.5) == 1, "getDistance2BetweenPoints function borked, for a one dimension case");
    fail_unless(getDistance2BetweenPoints(1, 1, 4, 5) == 25, "getDistance2BetweenPoints function borked, for a two dimensions case");
  }
END_TEST

/* From generic.h */
START_TEST(test_getDistance2BetweenRects){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = 20;
    rectA.w = 40;
    rectA.h = 30;

    rectB.x = rectB.y = 20;
    rectB.w = 46;
    rectB.h = 38;
    fail_unless(getDistance2BetweenRects(rectA, rectB) == 5, "getDistance2BetweenRects function borked");
  }
END_TEST

/* From generic.h */
START_TEST(test_fitRectToWorld){
    /* not quite understand logic of this fit, to discuss */
    /* not understand when whole rectangle should be present when below lower bound, only 1 pixel is present when above upper bound */
    /* fail_unless(fitRectToWorld()); */
    }
END_TEST

/* From generic.h */
START_TEST(test_generateRandomCoordOffset){
    /* the offset is only half of radius, please double check */
    /* the function generates a random number. the testing is to ensure the number is within range*/
    fail_unless(generateRandomCoordOffset(5) <= 5.0/2, "randPi function borked, random number greater than PI");
    fail_unless(generateRandomCoordOffset(5) >= -5.0/2, "randPi function borked, random number less than 0");
  }
END_TEST


/* From generic.h */
/* the function should return angle between the centre of 2 rectangles */
START_TEST(test_getAngleBetweenRects){
    SDL_Rect rectA;
    SDL_Rect rectB;

    rectA.x = rectA.y = -1;
    rectA.w = rectA.h = 2;

    rectB.x = rectB.y = 0;
    rectB.w = 12;
    rectB.h = -12;
    fail_unless(getAngleBetweenRects(&rectA, &rectB) == 135, "getAngleBetweenRects function borked");
  }
END_TEST


/* From generic.h */
/* TODO: fix this test. "SDL_Point" is a variable type, not a function.
START_TEST(test_getCenterOfRect){
    SDL_Rect rect;
    SDL_Point point;

    rect.x = rect.y = -1;
    rect.w=4;
    rect.h=6;
    point = SDL_Point(rect);
    fail_unless(pow((point.x-1),2)+pow((point.y-2),2) == 0, "getCenterOfRect function borked");
  }
END_TEST
*/

Suite *main_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("main");

  /* Core test case */
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_isPointInRect);
  tcase_add_test(tc_core, test_testRectIntersection);
  tcase_add_test(tc_core, test_isRectEnclosedInRect);
  tcase_add_test(tc_core, test_ensureRectEnclosed);
  tcase_add_test(tc_core, test_randPi);
  tcase_add_test(tc_core, test_square);
  tcase_add_test(tc_core, test_getDistance2BetweenPoints);
  tcase_add_test(tc_core, test_getDistance2BetweenRects);
  tcase_add_test(tc_core, test_fitRectToWorld);
  tcase_add_test(tc_core, test_generateRandomCoordOffset);
  tcase_add_test(tc_core, test_getAngleBetweenRects);
//  tcase_add_test(tc_core, test_getCenterOfRect);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = main_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
