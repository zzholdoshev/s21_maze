#ifndef A1_MAZE_SRC_A1_GRAPH_H_
#define A1_MAZE_SRC_A1_GRAPH_H_

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "global.h"

struct Node *create_node(int x, int y);
struct Queue *create_queue();
bool is_empty(struct Queue *queue);
struct ListNode *create_list_node(struct Node *data);
void enqueue(struct Queue *queue, struct Node *data);
struct Node *dequeue(struct Queue *queue);
void bfs(struct Node *start, int row, int col, int n, int m, int maze[n][m]);

struct Node {
  struct Node *prev;
  struct Node *next[4];
  int row;
  int col;
};

struct Queue {
  struct ListNode *front;
  struct ListNode *rear;
};

struct ListNode {
  struct Node *data;
  struct ListNode *next;
};

#endif  // A1_MAZE_SRC_A1_GRAPH_H_
