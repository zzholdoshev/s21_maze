#ifndef A1_MAZE_SRC_Q_LEARNING_
#define A1_MAZE_SRC_Q_LEARNING_

#include "global.h"

void initialize_q(int n, int m, double q[n][m][NUM_ACTIONS]);
void get_random_starting_point(int n, int m, int maze[n][m], int *i, int *j);
void learning_step(int n, int m, int maze[n][m], int goal_row, int goal_col,
                   double q[n][m][NUM_ACTIONS], int *reward, int *i, int *j);
void write_q_solution(int n, int m, double q[n][m][NUM_ACTIONS], int row,
                      int col);
int choose_q_action(int n, int m, int maze[n][m], int row, int col,
                    double q[n][m][NUM_ACTIONS]);
int load_q_table(int n, int m, int maze[n][m], int *start_row, int *start_col,
                 double q[n][m][NUM_ACTIONS]);
int detect_target(int n, int m, int maze[n][m]);
int get_max_option(int n, int m, int maze[n][m], int i, int j,
                   double q[n][m][NUM_ACTIONS]);

#endif  // A1_MAZE_SRC_Q_LEARNING_
