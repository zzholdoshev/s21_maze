#include <check.h>
#include <stdlib.h>

#include "../a1_maze.h"

START_TEST(maze_init) {
  int maze[10][10];
  initialize_maze(10, 10, maze, MAZE, 0);
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      if (i == 0 || j == 0 || i == 9 || j == 9) {
        ck_assert_int_eq(maze[i][j], 1);
      }
    }
  }
}

START_TEST(maze_import) {
  int fn = 0, fm = 0;
  FILE *file;
  file = fopen("files/maze", "r");
  fscanf(file, "%d %d", &fn, &fm);

  int n = fn + fn + 1, m = fm + fm + 1;
  int maze[n][m];
  read_maze_from_file(file, fn, fm, maze);

  int goal[9][9] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -1, -2, -2, -2, -1, -2, -1},
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

START_TEST(maze_export) {
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
  FILE *file = fopen("files/file_test", "w");
  save_maze(n, m, maze, file);
  fclose(file);

  file = fopen("files/file_test", "r");
  int fn, fm;
  fscanf(file, "%d %d", &fn, &fm);
  ck_assert_int_eq(fn, (n - 1) / 2);
  ck_assert_int_eq(fm, (m - 1) / 2);

  int copy[n][m];
  read_maze_from_file(file, fn, fm, copy);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      ck_assert_int_eq(maze[i][j], copy[i][j]);
    }
  }
}
END_TEST

START_TEST(clear_9x9) {
  int n = 9, m = 9;
  int maze[9][9] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1},
                    {-1, -4, -3, -3, -3, -3, -3, -3, -1},
                    {-1, -1, -1, -2, -1, -1, -1, -3, -1},
                    {-1, -2, -1, -2, -2, -2, -1, -3, -1},
                    {-1, -2, -1, -6, -1, -1, -1, -3, -1},
                    {-1, -2, -2, -2, -1, -3, -3, -3, -1},
                    {-1, -1, -1, -1, -1, -3, -1, -1, -1},
                    {-1, -6, -6, -6, -2, -3, -3, -4, -1},
                    {-1, -1, -1, -1, -1, -1, -1, -1, -1}};

  int goal[9][9] = {{-1, -1, -1, -1, -1, -1, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -1, -2, -2, -2, -1, -2, -1},
                    {-1, -2, -1, -2, -1, -1, -1, -2, -1},
                    {-1, -2, -2, -2, -1, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -2, -1, -1, -1},
                    {-1, -2, -2, -2, -2, -2, -2, -2, -1},
                    {-1, -1, -1, -1, -1, -1, -1, -1, -1}};

  clear_maze(n, m, maze);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      ck_assert_int_eq(maze[i][j], goal[i][j]);
    }
  }
}
END_TEST

int crawl_assert(int n, int m, int maze[n][m], int x, int y, int o) {
  const int offset[4][2] = {{1, 0}, {0, 1}, {0, -1}, {-1, 0}};
  // for (int i = 0; i < n; ++i) {
  //   for (int j = 0; j < m; ++j) {
  //     if (i == x && j == y)
  //       printf("!");
  //     else if (i == (x - offset[o][0]) && j == (y - offset[o][1]))
  //       printf("?");
  //     else if (maze[i][j] == SOLUTION)
  //       printf("*");
  //     else if (maze[i][j] == WALL)
  //       printf("x");
  //     else if (maze[i][j] == PASS)
  //       printf(" ");
  //     else
  //       printf("%d", maze[i][j]);
  //   }
  //   printf("\n");
  // }
  // printf("\n");
  ck_assert_int_ne(maze[x][y], SOLUTION);
  maze[x][y] = SOLUTION;

  int res = 1;
  for (int i = 0; i < 4 && res; ++i) {
    if (!(offset[i][0] == -offset[o][0] && offset[i][1] == -offset[o][1])) {
      int nx = x + offset[i][0], ny = y + offset[i][1];
      if (maze[nx][ny] != WALL) {
        crawl_assert(n, m, maze, nx, ny, i);
      }
    }
  }
  return res;
}

void assert_perfect(int n, int m, int maze[n][m]) {
  crawl_assert(n, m, maze, 1, 1, 0);

  for (int i = n - 1; i >= 0; --i) {
    for (int j = m - 1; j >= 0; --j) {
      ck_assert_int_ne(maze[i][j], PASS);
    }
  }
}

START_TEST(gen_49x49) {
  int n = 49, m = 49;
  int maze[n][m];
  srand(1);
  for (int i = 0; i < 100; ++i) {
    initialize_perfect_maze(n, m, maze);
    clear_maze(n, m, maze);
    assert_perfect(n, m, maze);
  }
}
END_TEST

Suite *maze_generation_test_suit() {
  Suite *s = suite_create("maze_generation");
  TCase *tc = tcase_create("basic");
  tcase_add_test(tc, maze_init);
  tcase_add_test(tc, maze_import);
  tcase_add_test(tc, maze_export);
  tcase_add_test(tc, clear_9x9);
  tcase_add_test(tc, gen_49x49);
  suite_add_tcase(s, tc);
  return s;
}