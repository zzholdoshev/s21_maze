#include "a1_maze.h"

#include "global.h"
#include "graph.h"

// converts maze with thin walls into maze with
// thick walls
void read_maze_from_file(FILE *file, int n, int m,
                         int maze[n + n + 1][m + m + 1]) {
  int vertical[n][m];
  int horizontal[n][m];
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      fscanf(file, "%d", &vertical[i][j]);
    }
  }
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      fscanf(file, "%d", &horizontal[i][j]);
    }
  }

  for (int i = 0; i < n + n + 1; ++i) {
    for (int j = 0; j < m + m + 1; ++j) {
      if (i == 0 || j == 0 || (i % 2 == 0 && j % 2 == 0)) {
        maze[i][j] = WALL;
      } else {
        maze[i][j] = PASS;
      }
    }
  }
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (vertical[i][j]) {
        maze[i * 2 + 1][j * 2 + 2] = WALL;
      }
      if (horizontal[i][j]) {
        maze[i * 2 + 2][j * 2 + 1] = WALL;
      }
    }
  }
}

void initialize_maze(int n, int m, int maze[n][m], int mode, double chance) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if ((i == 0 || j == 0 || i == n - 1 || j == m - 1) && mode == MAZE) {
        maze[i][j] = 1;
      } else if (mode == CAVE) {
        if ((double)rand() < (RAND_MAX + 1.) * chance)
          maze[i][j] = 1;
        else
          maze[i][j] = 0;
      }
    }
  }
}

void copy_cave(int n, int m, int cave[n][m], int copy[n][m]) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      copy[i][j] = cave[i][j];
    }
  }
}
void cave_core(int n, int m, int cave[n][m], int copy[n][m], int life_n,
               int death_n) {
  int offset[8][2] = {{1, -1}, {1, 0},  {1, 1},   {0, 1},
                      {-1, 1}, {0, -1}, {-1, -1}, {-1, 0}};

  for (int j = 0; j < n; ++j) {
    for (int k = 0; k < m; ++k) {
      int neighbours = 0;
      for (int q = 0; q < 8; ++q) {
        int x = j + offset[q][0], y = k + offset[q][1];
        if (x < 0 || y < 0 || x >= n || y >= m || cave[x][y] == 1) {
          ++neighbours;
        }
      }
      if (cave[j][k] == 1 && (neighbours < death_n)) {
        copy[j][k] = 0;
      } else {
        if (cave[j][k] == 0 && (neighbours > life_n)) {
          copy[j][k] = 1;
        }
      }
    }
  }
}

void kill_set(int n, int m, int maze[n][m], int target,
              int source) {  // kills set (the same number in >= 1 cells)
  for (int i = 1; i < n - 1; ++i) {
    for (int k = 1; k < m - 1; ++k) {
      if (maze[i][k] == target) {
        maze[i][k] = source;
      }
    }
  }
}

void process_row(int n, int m, int i, int *j, int maze[n][m]) {
  for (int k = 1; k < m - 1; ++k) {
    if (k % 2 == 1) {  // column with numbers
      if (i > 2 && maze[i - 1][k] == PASS) {
        // assign values according to vertical connections (pass
        // in the cell above means there's vertical connection):
        maze[i][k] = maze[i - 2][k];
      } else {
        // if there are no vertical connections above - assign the new set:
        maze[i][k] = (*j)++;
      }
    }
  }
  for (int k = 1; k < m - 1; ++k) {
    if ((rand() % 2 == 0 || i >= n - 3 || k >= m - 2) &&
        maze[i][k] == WALL) {  // n-3 could be last row, all different sets
                               // should be connected in last row
      if (maze[i][k - 1] != maze[i][k + 1]) {
        maze[i][k] = PASS;
        if (k < m - 2) {
          kill_set(n, m, maze, maze[i][k + 1], maze[i][k - 1]);
        }
      }
    }
  }
}

void process_column(int n, int m, int i, int maze[n][m]) {
  int current = 0;
  int prev = 0;
  for (int k = 1; k < m - 1; ++k) {
    if (i < n - 2) {  // proccess all except last horizontal wall
      if (k % 2 == 1) {
        current = maze[i - 1][k];
        if (rand() % 2 == 0 &&
            current == prev) {  // if it's the first number of set - the
                                // vertical connection to lower cell is
                                // obligatory to avoid isolated areas
          maze[i][k] = WALL;
        } else {
          maze[i][k] = PASS;
        }
        prev = current;
      } else {
        if (k >= m - 2 && maze[i][k - 1] == PASS) {
          maze[i][k] = PASS;
        }
      }
    } else {
      if (maze[i - 1][k] !=
          WALL) {  // last row => no vertical connections to the lower row as
                   // there are no more rows, just vertical walls
        maze[i][k] = PASS;
      }
    }
  }
}
void initialize_perfect_maze(int n, int m, int maze[n][m]) {
  int j = 1;
  for (int i = 0; i < n; ++i) {
    for (int k = 0; k < m; ++k) {
      if (i % 2 == 1) {    // row with sets of numbers
        if (k % 2 == 0) {  // columns with vertical walls or passes between sets
          maze[i][k] = WALL;
        } else {           // columns with sets (numbers)
          maze[i][k] = 0;  // initialize 0 where sets will be
        }
      } else {  // row with horizontal walls
        maze[i][k] =
            WALL;  // initialize horizontal walls and intersections by default
      }
      if (i == 0 || k == 0 || k == m - 1 || i == n - 1) {
        maze[i][k] = WALL;
      }
    }
  }
  for (int i = 1; i < n - 1; ++i) {
    if (i % 2 == 1) {
      process_row(n, m, i, &j, maze);
    } else {
      process_column(n, m, i, maze);
    }
  }
}

void solve_core(int n, int m, int maze[n][m]) {
  int first[2] = {0};
  int second[2] = {0};
  for (int i = 1; i < n - 1; ++i) {
    for (int j = 1; j < m - 1; ++j) {
      if (maze[i][j] == TARGET) {
        if (first[0] == 0) {
          first[0] = i;
          first[1] = j;
        } else {
          second[0] = i;
          second[1] = j;
        }
      }
    }
  }
  struct Node *start = create_node(first[0], first[1]);
  bfs(start, second[0], second[1], n, m, maze);
}

void clear_maze(int n, int m, int maze[n][m]) {
  for (int i = 1; i < n - 1; ++i) {
    for (int j = 1; j < m - 1; ++j) {
      if (maze[i][j] != WALL) {
        maze[i][j] = PASS;
      }
    }
  }
}

void save_maze(int n, int m, int maze[n][m], FILE *file) {
  if (file != NULL) {
    if (n % 2 == 0) {
      n -= 1;
    }
    if (m % 2 == 0) {
      m -= 1;
    }

    fprintf(file, "%d %d\n", ((n) / 2), ((m) / 2));
    for (int i = 1; i < n; ++i) {
      for (int j = 1; j < m; ++j) {
        if (i % 2 == 0 || j % 2 == 0) continue;

        int value;
        if (maze[i][j + 1] == WALL || j > m - 3) {
          value = 1;
        } else {
          value = 0;
        }
        if (j > m - 3) {
          fprintf(file, "%d\n", value);
        } else {
          fprintf(file, "%d ", value);
        }
      }
    }
    fprintf(file, "\n");
    for (int i = 2; i < n; ++i) {
      for (int j = 1; j < m; ++j) {
        if (i % 2 == 1 || j % 2 == 0) continue;

        int value;
        if (maze[i][j] == WALL || i > n - 3) {
          value = 1;
        } else {
          value = 0;
        }
        if (j > m - 3) {
          fprintf(file, "%d\n", value);
        } else {
          fprintf(file, "%d ", value);
        }
      }
    }
  }
}
