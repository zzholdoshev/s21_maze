#ifndef A1_MAZE_SRC_A1_MAZE_H_
#define A1_MAZE_SRC_A1_MAZE_H_

#include <stdio.h>

#include "global.h"
#include "q_learning.h"

void read_maze_from_file(FILE *file, int n, int m,
                         int maze[n + n + 1][m + m + 1]);

void initialize_maze(int n, int m, int maze[n][m], int mode, double chance);
void copy_cave(int n, int m, int cave[n][m], int copy[n][m]);
void cave_core(int n, int m, int cave[n][m], int copy[n][m], int life_n,
               int death_n);

void kill_set(int n, int m, int maze[n][m], int target, int source);
void process_row(int n, int m, int i, int *j, int maze[n][m]);
void process_column(int n, int m, int i, int maze[n][m]);
void initialize_perfect_maze(int n, int m, int maze[n][m]);
void save_maze(int n, int m, int maze[n][m], FILE *file);
void clear_maze(int n, int m, int maze[n][m]);
void solve_core(int n, int m, int maze[n][m]);

int get_max_option(int n, int m, int maze[n][m], int i, int j,
                   double q[n][m][NUM_ACTIONS]);
void q_learning(int n, int m, int maze[n][m], int goal_row, int goal_col);
void print_maze(int n, int m, int maze[n][m]);

#endif  // A1_MAZE_SRC_A1_MAZE_H_