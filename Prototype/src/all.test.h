#ifndef MYTEST_GENERIC_TEST_H
#define MYTEST_GENERIC_TEST_H

#include <check.h>
#include "../src/main.h"

Suite *makeMasterSuite(void);
Suite *makeSuiteForGeneric(void);
Suite *makeSuiteForControls(void);

#endif /*MYTEST_GENERIC_TEST_H*/
