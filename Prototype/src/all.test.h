#ifndef MYTEST_GENERIC_TEST_H
#define MYTEST_GENERIC_TEST_H

#include <check.h>
#include "../src/main.h"

Suite *makeMasterSuite(void);
Suite *makeSuiteForGeneric(void);
Suite *makeSuiteForAI(void);
Suite *makeSuiteForAnnouncements(void);
Suite *makeSuiteForAudio(void);
Suite *makeSuiteForConfiguration(void);
Suite *makeSuiteForControls(void);
Suite *makeSuiteForGame(void);
Suite *makeSuiteForGame_objects(void);
Suite *makeSuiteForGraphics(void);
Suite *makeSuiteForInit(void);
Suite *makeSuiteForMain(void);
Suite *makeSuiteForUI(void);
Suite *makeSuiteForWorld_generation(void);

#endif /*MYTEST_GENERIC_TEST_H*/
