#include <check.h>
#include <stdlib.h>

#include "../a1_maze.h"
#include "../global.h"
#include "../q_learning.h"

void q_study(int n, int m, int maze[n][m], int goal_row, int goal_col,
             double q[n][m][NUM_ACTIONS]) {
  initialize_q(n, m, q);
  for (int z = 0; z < EPISODES; ++z) {
    int i = 0;
    int j = 0;
    get_random_starting_point(n, m, maze, &i, &j);
    int reward = -1;
    while (reward != REWARD) {
      learning_step(n, m, maze, goal_row, goal_col, q, &reward, &i, &j);
    }
    clear_maze(n, m, maze);
    maze[goal_row][goal_col] = TARGET;
  }
}

void print_maze(int n, int m, int maze[n][m]) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (maze[i][j] == SOLUTION)
        printf("*");
      else if (maze[i][j] == WALL)
        printf("x");
      else if (maze[i][j] == PASS)
        printf(" ");
      else if (maze[i][j] == TARGET)
        printf("_");
      else
        printf("%d", maze[i][j]);
    }
    printf("\n");
  }
}

void assert_q_solve(int n, int m, int maze[n][m], int goal_row, int goal_col,
                    double q[n][m][NUM_ACTIONS], int start_row, int start_col) {
  int offset[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
  int copy[n][m];
  maze[start_row][start_col] = TARGET;
  maze[goal_row][goal_col] = TARGET;
  copy_cave(n, m, maze, copy);

  int row = start_row, col = start_col;
  do {
    int action = choose_q_action(n, m, maze, row, col, q);
    ck_assert_int_ne(action, -1);
    row += offset[action][0];
    col += offset[action][1];
    if (maze[row][col] != TARGET) maze[row][col] = SOLUTION;
  } while (maze[row][col] != TARGET);

  solve_core(n, m, copy);
  // print_maze(n, m, copy);

  maze[start_row][start_col] = SOLUTION;
  maze[goal_row][goal_col] = SOLUTION;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      ck_assert_int_eq(copy[i][j], maze[i][j]);
    }
  }

  clear_maze(n, m, maze);
}

void assert_q_learn(int n, int m, int maze[n][m], int goal_row, int goal_col) {
  double q[n][m][NUM_ACTIONS];
  q_study(n, m, maze, goal_row, goal_col, q);
  for (int i = 0; i < 25; ++i) {
    int start_row = 0, start_col = 0;
    maze[goal_row][goal_col] = TARGET;
    get_random_starting_point(n, m, maze, &start_row, &start_col);
    clear_maze(n, m, maze);
    assert_q_solve(n, m, maze, goal_row, goal_col, q, start_row, start_col);
  }
}

START_TEST(q_19x19) {
  int n = 19, m = 19;
  int maze[n][m];
  srand(1);
  for (int i = 0; i < 5; ++i) {
    initialize_perfect_maze(n, m, maze);
    clear_maze(n, m, maze);
    for (int j = 0; j < 5; ++j) {
      int goal_row = 1, goal_col = 1;
      get_random_starting_point(n, m, maze, &goal_row, &goal_col);
      assert_q_learn(n, m, maze, goal_row, goal_col);
    }
  }
}
END_TEST

START_TEST(export_import) {
  int n = 9, m = 9;
  int maze[9][9] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -1, -2, -2, -2, -1, -2, -1},
                    {-1, -2, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -2, -2, -1, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -2, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -1, -1, -1, -1}};
  double q[n][m][NUM_ACTIONS];
  q_study(n, m, maze, 1, 1, q);
  write_q_solution(n, m, q, 1, 1);

  int row = 0, col = 0;
  double copy[n][m][NUM_ACTIONS];
  load_q_table(n, m, maze, &row, &col, copy);
  ck_assert_int_eq(row, 1);
  ck_assert_int_eq(col, 1);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      for (int k = 0; k < NUM_ACTIONS; ++k) {
        ck_assert_ldouble_eq_tol(q[i][j][k], copy[i][j][k], 1e-14);
      }
    }
  }
}
END_TEST

START_TEST(target_detection) {
  int n = 9, m = 9;
  int maze[9][9] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -1, -2, -2, -2, -1, -2, -1},
                    {-1, -2, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -2, -2, -1, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -2, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -1, -1, -1, -1}};
  double q[n][m][NUM_ACTIONS];
  q_study(n, m, maze, 3, 3, q);
  write_q_solution(n, m, q, 3, 3);

  detect_target(n, m, maze);

  int goal[9][9] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -1, -6, -2, -2, -1, -2, -1},
                    {-1, -2, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -2, -2, -1, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -2, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -1, -1, -1, -1}};
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      ck_assert_int_eq(maze[i][j], goal[i][j]);
    }
  }
}
END_TEST

Suite *q_learning_test_suit() {
  Suite *s = suite_create("q_learning");
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, q_19x19);
  tcase_add_test(tc, export_import);
  tcase_add_test(tc, target_detection);
  suite_add_tcase(s, tc);
  return s;
}
