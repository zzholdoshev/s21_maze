#include <stdio.h>
#include <stdlib.h>

#include "a1_maze.h"
#include "global.h"

// Q(s, a) = R(s') + y * maxQ(s', a)
// update rule: Q <= Q(s,a) + alpha * TD error (observed value - expected value)

int offset[4][2] = {{-1, 0},
                    {0, -1},
                    {1, 0},
                    {0, 1}};  // 4 possible directions - up, left, down, right

int get_max_option(int n, int m, int maze[n][m], int i, int j,
                   double q[n][m][NUM_ACTIONS]) {
  double max = -9999;
  int action = -1;
  for (int a = 0; a < 4;
       ++a) {  // get maximum q-value for each action (a) in current state
    int new_i = i + offset[a][0];
    int new_j = j + offset[a][1];
    if (maze[new_i][new_j] != WALL) {
      if (q[i][j][a] > max) {
        max = q[i][j][a];
        action = a;
      }
    }
  }
  return action;
}

double get_max(int n, int m, int maze[n][m], int i, int j,
               double q[n][m][NUM_ACTIONS]) {
  double max = -9999;
  for (int a = 0; a < 4;
       ++a) {  // get maximum q-value for each action (a) in current state
    int new_i = i + offset[a][0];
    int new_j = j + offset[a][1];
    if (maze[new_i][new_j] != WALL) {
      if (q[i][j][a] > max) {
        max = q[i][j][a];
      }
    }
  }
  return max;
}
int get_num_action(int n, int m, int maze[n][m], int i, int j) {
  int actions = 0;
  for (int a = 0; a < 4;
       ++a) {  // get maximum q-value for each action (a) in current state
    int new_i = i + offset[a][0];
    int new_j = j + offset[a][1];
    if (maze[new_i][new_j] != WALL) {
      ++actions;
    }
  }
  return actions;
}

int get_rand_action(int n, int m, int maze[n][m], int i, int j) {
  int possible_actions = get_num_action(n, m, maze, i, j);
  int action = 0;
  action = rand() % possible_actions + 1;
  int action2 = 0;
  int res;
  for (int a = 0; a < 4; ++a) {
    if (maze[i + offset[a][0]][j + offset[a][1]] != WALL) {
      ++action2;
      if (action == action2) {
        res = a;
      }
    }
  }
  return res;
}

int choose_action(int n, int m, int maze[n][m], int i, int j,
                  double q[n][m][NUM_ACTIONS]) {
  if (rand() % 10 == 0) {
    int rand_action = get_rand_action(n, m, maze, i, j);
    return rand_action;
  } else {
    int max_action = get_max_option(n, m, maze, i, j, q);
    return max_action;
  }
}
void write_q_solution(int n, int m, double q[n][m][NUM_ACTIONS], int row,
                      int col) {
  FILE *file;
  file = fopen("q_table", "w");
  fprintf(file, "%d %d %d %d\n", n, m, row, col);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      for (int a = 0; a < NUM_ACTIONS; ++a) {
        fprintf(file, "%.15lf ", q[i][j][a]);
      }
      fprintf(file, "\n");
    }
  }
  fclose(file);
}

void get_random_starting_point(int n, int m, int maze[n][m], int *i, int *j) {
  int success = 0;
  while (success != 1) {
    *i = rand() % n;
    *j = rand() % m;
    if (maze[*i][*j] != WALL && maze[*i][*j] != TARGET) {
      success = 1;
    }
  }
}

void initialize_q(int n, int m, double q[n][m][NUM_ACTIONS]) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      for (int a = 0; a < NUM_ACTIONS; ++a) {
        q[i][j][a] = 0;
      }
    }
  }
}

void learning_step(int n, int m, int maze[n][m], int goal_row, int goal_col,
                   double q[n][m][NUM_ACTIONS], int *reward, int *i, int *j) {
  int action = choose_action(n, m, maze, *i, *j, q);
  int new_i = *i + offset[action][0];
  int new_j = *j + offset[action][1];
  *reward = new_i == goal_row && new_j == goal_col
                ? REWARD
                : -1;  // for the goal cell reward is 10, for ordinary cell -1
  if (maze[new_i][new_j] == SOLUTION) {
    *reward = -100;
  }
  q[*i][*j][action] +=
      ALPHA * ((*reward + DISCOUNT * get_max(n, m, maze, new_i, new_j, q)) -
               q[*i][*j][action]);  // Q-value formula
  maze[*i][*j] = SOLUTION;
  maze[new_i][new_j] = TRAINING;
  *i = new_i;
  *j = new_j;
}

// Return index of Q-agent q[n][m][NUM_ACTIONS] action for position (row, col)
// in maze[n][m]
int choose_q_action(int n, int m, int maze[n][m], int row, int col,
                    double q[n][m][NUM_ACTIONS]) {
  int offset[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
  double max = -9999;
  int action = -1;
  for (int a = 0; a < NUM_ACTIONS; ++a) {
    if (q[row][col][a] > max &&
        maze[row + offset[a][0]][col + offset[a][1]] != WALL &&
        q[row][col][a] != 0.) {
      max = q[row][col][a];
      action = a;
    }
  }
  return action;
}

// Load Q-agent table for maze[n][m] into q[n][m][NUM_ACTIONS], save target
// point to (start_row, start_col)
int load_q_table(int n, int m, int maze[n][m], int *start_row, int *start_col,
                 double q[n][m][NUM_ACTIONS]) {
  FILE *q_table = fopen("q_table", "r");
  int res = 0;
  int a, b, c, d;
  a = b = c = d = 0;
  if (q_table != NULL)
    fscanf(q_table, "%d %d %d %d", &a, &b, &c, &d);
  else if (a < 1 || b < 1 || c < 1 || d < 1)
    res = ERROR_FILE;
  for (int i = 0; res != ERROR_FILE && i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (maze[i][j] == TARGET) {
        *start_row = i;
        *start_col = j;
      }
      for (int a = 0; a < NUM_ACTIONS; ++a) {
        fscanf(q_table, "%lf", &q[i][j][a]);
      }
    }
  }
  fclose(q_table);
  return res;
}

// Check the goal position for the Q-agent currently stored in "q_table" and
// mark it on maze[n][m]. If the agent is trained for a different maze return
// ERROR_MAZE
int detect_target(int n, int m, int maze[n][m]) {
  int res = 0;
  FILE *q_table = fopen("q_table", "r");
  int goal_row = 0;
  int goal_col = 0;
  int size_n, size_m = 0;
  if (q_table != NULL) {
    fscanf(q_table, "%d %d %d %d", &size_n, &size_m, &goal_row, &goal_col);
    if (maze[goal_row][goal_col] == WALL || size_n != n || size_m != m) {
      res = ERROR_MAZE;
    } else {
      maze[goal_row][goal_col] = TRAINING;
    }
    fclose(q_table);
  } else {
    res = ERROR_FILE;
  }
  return res;
}
