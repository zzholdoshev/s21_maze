#include <unistd.h>

#include "graph.h"

void print_perfect(int n, int m, int maze[n][m]);

struct Node *create_node(int row, int col) {
  struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
  if (newNode == NULL) {
    perror("Memory allocation failed");
  }
  newNode->row = row;
  newNode->col = col;
  newNode->prev = NULL;
  for (int i = 0; i < 4; ++i) {
    newNode->next[i] = NULL;
  }
  return newNode;
}

struct Queue *create_queue() {
  struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
  if (queue == NULL) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  queue->front = NULL;
  queue->rear = NULL;
  return queue;
}
bool is_empty(struct Queue *queue) { return (queue->front == NULL); }

struct ListNode *create_list_node(struct Node *data) {
  struct ListNode *newNode = (struct ListNode *)malloc(sizeof(struct ListNode));
  if (newNode == NULL) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  newNode->data = data;
  newNode->next = NULL;
  return newNode;
}

void enqueue(struct Queue *queue, struct Node *data) {
  struct ListNode *newNode = create_list_node(data);
  if (is_empty(queue)) {
    queue->front = newNode;
    queue->rear = newNode;
  } else {
    queue->rear->next = newNode;
    queue->rear = newNode;
  }
}

struct Node *dequeue(struct Queue *queue) {
  if (is_empty(queue)) {
    fprintf(stderr, "Queue is empty\n");
    exit(EXIT_FAILURE);
  }
  struct ListNode *temp = queue->front;
  queue->front = queue->front->next;
  if (queue->front == NULL) {
    queue->rear = NULL;
  }
  struct Node *data = temp->data;
  free(temp);
  return data;
}
int get_neighbours(struct Node *node, int n, int m, int maze[n][m],
                   bool visited[n][m]) {
  int offset[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
  int j = 0;
  for (int i = 0; i < 4; ++i) {
    int row = node->row + offset[i][0];
    int col = node->col + offset[i][1];
    if (maze[row][col] != WALL &&
        ((node->prev == NULL ||
          (visited[row][col] != true)))) {  // if possible neighbour cell is not
                                            // a wall and is not a previous cell
      node->next[j++] = create_node(row, col);
    }
  }
  return j;
}
void print_solution(struct Node *neighbour, int n, int m, int maze[n][m]) {
  while (neighbour != NULL && neighbour->row < n && neighbour->col < m) {
    maze[neighbour->row][neighbour->col] = SOLUTION;
    neighbour = neighbour->prev;
  }
}
void freeNode(struct Node *node) {
  if (node == NULL) {
    return;
  }
  // Recursively free the next nodes
  for (int i = 0; i < 4; i++) {
    freeNode(node->next[i]);
  }
  // Free the current node
  free(node);
}

void free_everything(struct Queue *queue, struct Node *last) {
  while (!is_empty(queue)) {
    dequeue(queue);  // free list node objects in queue
  }
  free(queue);
  while (last->prev != NULL) {  // free Nodes objects
    last = last->prev;
  }
  freeNode(last);
}
void fill_visited(int n, int m, bool visited[n][m]) {
  for (int i = 1; i < n - 1; ++i) {
    for (int j = 1; j < m - 1; ++j) {
      visited[i][j] = false;
    }
  }
}

void bfs(struct Node *start, int row, int col, int n, int m, int maze[n][m]) {
  struct Queue *queue = create_queue();
  bool visited[n][m];
  fill_visited(n, m, visited);
  enqueue(queue, start);
  while (!is_empty(queue)) {
    struct Node *current = dequeue(queue);
    visited[current->row][current->col] = true;
    // Explore neighbour cells and enqueue them:
    int count_of_neighbours = get_neighbours(current, n, m, maze, visited);
    if (count_of_neighbours > 0) {
      for (int i = 0; i < 4; ++i) {
        struct Node *neighbour = current->next[i];
        if (neighbour != NULL) {      // if there is a neighbour
          neighbour->prev = current;  // connect to parent
          if (neighbour->row == row &&
              neighbour->col == col) {  // if neighbour cell is target => print
                                        // solution and free memory
            print_solution(neighbour, n, m, maze);
            free_everything(queue, neighbour);
            return;
          }
          enqueue(queue, neighbour);
        }
      }
    }
  }
}
