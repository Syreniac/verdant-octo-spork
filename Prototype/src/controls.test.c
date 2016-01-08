#include <check.h>
#include "../src/main.h"


/* Test doesn't pass. */
/* From controls.h */
START_TEST(test_handleEvent){
    SDL_Event event;
    /* see startGame in game.c */
    GameData gameData;

    event.type = SDL_MOUSEMOTION;
    /* int handleEvent(SDL_Event *event, GameObjectData *gameObjectData, UIData *uiData, ControlsData *controlsData, GraphicsData *graphicsData); */
    fail_unless(handleEvent(&event,&gameData.gameObjectData,&gameData.uiData,&gameData.controlsData, &gameData.graphicsData) == 0, "isPointInRect function borked");
  }
END_TEST



Suite *main_suite(void)
{
  Suite *controls;
  TCase *tc_core;

  controls = suite_create("main");

  /* Core test case */
  tc_core = tcase_create("core");

  tcase_add_test(tc_core, test_handleEvent);
  suite_add_tcase(controls, tc_core);

  return controls;
}

int main(void)
{
  int number_failed;
  Suite *controls;
  SRunner *controlsr;

  controls = main_suite();
  controlsr = srunner_create(controls);

  srunner_run_all(controlsr, CK_NORMAL);
  number_failed = srunner_ntests_failed(controlsr);
  srunner_free(controlsr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
