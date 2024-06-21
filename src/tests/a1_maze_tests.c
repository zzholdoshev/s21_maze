#include "a1_maze_tests.h"

#include <check.h>
#include <stdio.h>

void run_suite(Suite *s) {
  SRunner *runner = srunner_create(s);
  srunner_set_fork_status(runner, CK_NOFORK);
  srunner_run_all(runner, CK_NORMAL);
  srunner_free(runner);
}

int main(void) {
  Suite *test_suites[4];

  test_suites[0] = cave_generation_test_suit();
  test_suites[1] = maze_generation_test_suit();
  test_suites[2] = maze_solving_test_suit();
  test_suites[3] = q_learning_test_suit();

  for (int i = 0; i < 4; i++) run_suite(test_suites[i]);
  return 0;
}
