#include <check.h>

#include "../a1_maze.h"
#include "a1_maze_tests.h"

START_TEST(cave_init) {
  int cave[10][10];
  initialize_maze(10, 10, cave, CAVE, 0.5);
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      ck_assert(cave[i][j] == 0 || cave[i][j] == 1);
    }
  }
}

START_TEST(cave_gen) {
  int cave[10][10] = {
      {1, 0, 1, 0, 0, 0, 0, 1, 1, 0}, {0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
      {0, 0, 1, 0, 1, 0, 1, 1, 0, 1}, {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
      {0, 0, 0, 1, 1, 0, 0, 1, 1, 1}, {0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
      {1, 1, 0, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 1, 1},
      {1, 0, 0, 0, 0, 1, 1, 0, 0, 0}, {0, 1, 1, 0, 0, 1, 1, 0, 0, 0}};
  int copy[10][10];
  int ll = 4, dl = 3;
  copy_cave(10, 10, cave, copy);
  cave_core(10, 10, cave, copy, ll, dl);
  copy_cave(10, 10, copy, cave);
  cave_core(10, 10, cave, copy, ll, dl);
  copy_cave(10, 10, copy, cave);
  cave_core(10, 10, cave, copy, ll, dl);
  copy_cave(10, 10, copy, cave);
  cave_core(10, 10, cave, copy, ll, dl);
  copy_cave(10, 10, copy, cave);
  cave_core(10, 10, cave, copy, ll, dl);
  copy_cave(10, 10, copy, cave);
  cave_core(10, 10, cave, copy, ll, dl);
  copy_cave(10, 10, copy, cave);
  cave_core(10, 10, cave, copy, ll, dl);
  copy_cave(10, 10, copy, cave);

  int goal[10][10] = {
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 1, 1, 1, 1, 1, 0, 0, 1, 1}, {1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
      {1, 1, 0, 0, 0, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      ck_assert_int_eq(copy[i][j], goal[i][j]);
    }
  }
}

Suite *cave_generation_test_suit() {
  Suite *s = suite_create("cave_generation");
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, cave_init);
  tcase_add_test(tc, cave_gen);
  suite_add_tcase(s, tc);
  return s;
}