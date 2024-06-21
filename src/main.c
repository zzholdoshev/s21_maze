#include <curses.h>
#include <stdio.h>

#include "a1_maze.h"
#include "global.h"
#include "graph.h"

// Display logo stored in "banner.txt"
void print_logo() {
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  printw("\n");
  attron(COLOR_PAIR(2));
  FILE *file = fopen("banner.txt", "r");
  if (file != NULL) {
    char *line = NULL;
    ssize_t read;
    size_t len = 0;
    while ((read = getline(&line, &len, file)) != -1) {
      printw("%s", line);
    }
    fclose(file);
    free(line);
    attroff(COLOR_PAIR(2));
  }
}

// Ask user to select from n options with prompt using arrow keys
int option_select(const char *prompt, int n, const char **options) {
  int current_option = 0;
  while (true) {
    clear();
    addstr(prompt);
    for (int i = 0; i < n; ++i) {
      if (i == current_option) {
        attron(A_REVERSE);  // Invert colors for the selected option
      }
      printw("\t%s\n", options[i]);
      if (i == current_option) {
        attroff(A_REVERSE);  // Disable color inversion
      }
    }
    print_logo();
    refresh();

    int ch = getch();
    switch (ch) {
      case KEY_UP:
        current_option = (current_option + n - 1) % n;
        break;
      case KEY_DOWN:
        current_option = (current_option + 1) % n;
        break;
      case ENTER:
        clear();
        printw("Selected: %s\n", options[current_option]);
        refresh();
        return current_option;
        break;
    }
  }
}

// Ask user to hit a single key with prompt
int key_prompt(const char *prompt) {
  addstr(prompt);
  return getch();
}

// Ask user to input a filepath and open it with mode
FILE *file_prompt(const char *mode) {
  FILE *file;
  char filepath[256];
  clear();
  refresh();
  printw(
      "Enter the path to the maze or the cave, e.g. files/maze, "
      "files/maze2, files/q_maze:\n");
  getstr(filepath);
  file = fopen(filepath, mode);
  return file;
}

// Check if n and m are within limits,
// return ERROR_DIMENSIONS if < 0
// replace with 50 if > 50
int n_m_checker(int *n, int *m) {
  int res = 0;
  if (*n <= 0 || *m <= 0) {
    res = ERROR_DIMENSIONS;
  } else if (*n > 50 || *m > 50) {
    clear();
    key_prompt(
        "The maximum size of a maze/cave is 50x50! The size was reduced to 50\n"
        "Press any key to continue");
    if (*n > 50) *n = 50;
    if (*m > 50) *m = 50;
  }
  return res;
}

// Return 1 if cave_a is equal to cave_b, return 0 if not
int cave_equal(int n, int m, int cave_a[n][m], int cave_b[n][m]) {
  int res = 1;
  for (int i = 0; i < n && res; ++i) {
    for (int j = 0; j < m && res; ++j) {
      if (cave_a[i][j] != cave_b[i][j]) res = 0;
    }
  }
  return res;
}

// Display cave
void print_cave(int n, int m, int cave[n][m]) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (cave[i][j] == 1) {
        attron(COLOR_PAIR(1));
        printw("%2c", ' ');
      } else {
        attroff(COLOR_PAIR(1));
        printw("%2c", ' ');
      }
    }
    printw("\n");
  }
  attroff(COLOR_PAIR(1));
}

// Run step of a CA producing maze-like results
void game_of_maze_core(int n, int m, int maze[n][m], int copy[n][m]) {
  int offset[8][2] = {{1, -1}, {1, 0},  {1, 1},   {0, 1},
                      {-1, 1}, {0, -1}, {-1, -1}, {-1, 0}};
  for (int j = 1; j < n - 1; ++j) {
    for (int k = 1; k < m - 1; ++k) {
      int neighbours = 0;
      for (int q = 0; q < 8; ++q) {
        if (maze[j + offset[q][0]][k + offset[q][1]] == 1) ++neighbours;
      }
      if (maze[j][k] == 1 && (neighbours < 1 || neighbours > 4)) {
        copy[j][k] = 0;
      } else if (maze[j][k] == 0 && (neighbours == 3)) {
        copy[j][k] = 1;
      }
    }
  }
}

// Run a CA producing maze-like results step-by-step until a step makes no
// change or the user presses escape
void game_of_maze(int n, int m, int maze[n][m]) {
  int copy[n][m];
  int res;
  do {
    clear();
    print_cave(n, m, maze);
    refresh();
    copy_cave(n, m, maze, copy);
    game_of_maze_core(n, m, copy, maze);
    res = key_prompt("Press any key for next step");
  } while (!cave_equal(n, m, maze, copy) && res != ESCAPE);
}
// Ask the user for field paramaters for a maze-like CA
// Run the CA repedeatly each time reseting the field until the user presses
// escape
int cellular_maze_prompt() {
  int n = 0, m = 0, res = 0;
  double chance = 0;
  do {
    clear();
    refresh();
    printw(
        "Enter NxM dimensions of the maze and a chance for a cell to be "
        "initialised (e.g. \"40 40 0.5\") \n");
    scanw("%d %d %lf", &n, &m, &chance);
    res = n_m_checker(&n, &m);
    if (!res) {
      int cave[n][m];
      while (res != ESCAPE) {
        initialize_maze(n, m, cave, CAVE, chance);
        initialize_maze(n, m, cave, MAZE, 0);
        game_of_maze(n, m, cave);
        clear();
        print_cave(n, m, cave);
        refresh();
        res = key_prompt("Press any key to try again, ESC to get back");
      }
    } else if (res == ERROR_DIMENSIONS) {
      res = key_prompt(
          "ERROR: dimensions should be > 0\n"
          "Press any key to try again, ESC to get back");
    }
  } while (res && res != ESCAPE);
  return res;
}

// Run a CA producing cave-like results step-by-step
// or automatically (if millisec > 0) with parameters life_n and death_n
void run_cave_automata(int n, int m, int cave[n][m], long millisec, int life_n,
                       int death_n) {
  int copy[n][m];
  do {
    clear();
    print_cave(n, m, cave);
    refresh();
    copy_cave(n, m, cave, copy);
    cave_core(n, m, copy, cave, life_n, death_n);
    if (millisec > 0) {
      usleep(millisec);
    } else {
      key_prompt("Press any key for next step");
    }
  } while (!cave_equal(n, m, cave, copy));
}

// Ask the user for preffered display mode and rule paramaters for cave-like CA
void cave_automata_settings_prompt(long *millisec, int *life_n, int *death_n) {
  const char *prompt = "Select cave generation display mode\n";
  const char *options[2] = {"Step-by-step generation (use any key)",
                            "N milliseconds pause between frames"};
  int cave_option = option_select(prompt, 2, options);
  clear();
  refresh();
  if (cave_option == 1) {
    printw(
        "Enter pause between frames in milliseconds (200 is "
        "recommended)\n");
    scanw("%ld", millisec);
    *millisec *= 1000;  // from millisec to microsec
  } else {
    *millisec = 0;
  }

  clear();
  printw(
      "Enter number of neighbours needed for birth and a number of neighbours "
      "needed for life (e.g. \"5 4\") \n");
  scanw("%d %d", life_n, death_n);
}

// Ask the user for a filepath to a starting state for a cave-like CA and CA
// parameters, then run the CA repedeatly each time requesting the  until the
// user presses escape
int cave_from_file_prompt() {
  int res = 0;
  while (res == 0) {
    FILE *file = file_prompt("r");
    if (file == NULL) {
      res = ERROR_FILE;
    } else {
      int n = 0, m = 0;
      fscanf(file, "%d %d", &n, &m);
      int cave[n][m];
      for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
          fscanf(file, "%d", &cave[i][j]);
        }
      }

      clear();
      print_cave(n, m, cave);
      refresh();
      res = key_prompt("Press any key to try again, ESC to get back");
      if (res != ESCAPE) res = 0;
    }
  }
  return res;
}

// Ask the user for field parameters for a cave-like CA and CA
// parameters, then run the CA repedeatly each time reseting the field until the
// user presses escape
int cave_gen_from_scratch_prompt() {
  int n = 0, m = 0, res = 0;
  double chance = 0;
  do {
    clear();
    refresh();
    printw(
        "Enter NxM dimensions of the cave and a chance for a cell to be "
        "initialised (e.g. \"40 40 0.5\") \n");
    scanw("%d %d %lf", &n, &m, &chance);
    res = n_m_checker(&n, &m);
    if (!res) {
      long millisec;
      int life_n, death_n;
      cave_automata_settings_prompt(&millisec, &life_n, &death_n);

      int cave[n][m];
      while (res != ESCAPE) {
        initialize_maze(n, m, cave, CAVE, chance);
        run_cave_automata(n, m, cave, millisec, life_n, death_n);
        clear();
        print_cave(n, m, cave);
        refresh();
        res = key_prompt("Press any key to try again, ESC to get back");
      }
    } else if (res == ERROR_DIMENSIONS) {
      res = key_prompt(
          "ERROR: dimensions should be > 0\n"
          "Press any key to try again, ESC to get back");
    }
  } while (res && res != ESCAPE);
  return res;
}

// Display maze[n][m]
void print_maze(int n, int m, int maze[n][m]) {
  init_pair(4, COLOR_BLACK, COLOR_GREEN);
  init_pair(5, COLOR_BLACK, COLOR_RED);
  clear();
  refresh();
  for (int i = 0; i < n; ++i) {
    for (int k = 0; k < m; ++k) {
      switch (maze[i][k]) {
        case WALL:
          attron(COLOR_PAIR(1));
          printw("%2c", ' ');
          break;
        case TARGET:
          attron(COLOR_PAIR(4));
          printw("%2c", 'X');
          attroff(COLOR_PAIR(4));
          break;
        case SOLUTION:
          attron(COLOR_PAIR(4));
          printw("%2c", ' ');
          attroff(COLOR_PAIR(4));
          break;
        case TRAINING:
          attron(COLOR_PAIR(5));
          printw("%2c", 'X');
          attroff(COLOR_PAIR(5));
          break;
        default:
          attroff(COLOR_PAIR(1));
          printw("%2c", ' ');
          break;
      }
    }
    printw("\n");
  }
  attroff(COLOR_PAIR(1));
}

// Q-agent training for maze[n][m] with goal point (goal_row, goal_col)
void q_learning(int n, int m, int maze[n][m], int goal_row, int goal_col) {
  int answer =
      key_prompt("Do you want to see the process of training the agent? Y/N\n");
  clear();
  refresh();
  double q[n][m][NUM_ACTIONS];
  initialize_q(n, m, q);
  for (int z = 0; z < EPISODES; ++z) {
    int i = 0;
    int j = 0;
    get_random_starting_point(n, m, maze, &i,
                              &j);  // each time agent starts on random cell
    int reward = -1;
    for (int w = 0; reward != REWARD; ++w) {
      learning_step(n, m, maze, goal_row, goal_col, q, &reward, &i, &j);
      if ((z % 1000 == 0 || z == 9999) &&
          (w % (n * m / 100 + 1) == 0 || reward == REWARD) &&
          (answer == 'y' || answer == 'Y')) {
        print_maze(n, m, maze);
        refresh();
        usleep(10000);
      }
    }

    if (answer != 'y' && answer != 'Y') {
      if (z % ((EPISODES / 100)) == 0) {
        printw("#");
        usleep(10000);
        refresh();
      }
    }
    clear_maze(n, m, maze);
    maze[goal_row][goal_col] = TARGET;
  }
  refresh();
  write_q_solution(n, m, q, goal_row, goal_col);
}

// Show the solution of agent currently stored in "q_table" for maze[n][m]
// step-by-step. If the agent is trained for a different maze return ERROR_MAZE
int print_q_solution(int n, int m, int maze[n][m]) {
  int offset[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
  double q[n][m][NUM_ACTIONS];
  int res = 0;
  int row = 1;
  int col = 1;

  res = load_q_table(n, m, maze, &row, &col, q);

  if (res != ERROR_FILE) {
    while (maze[row][col] != TRAINING && res != ESCAPE && res != ERROR_MAZE) {
      int action = choose_q_action(n, m, maze, row, col, q);
      if (action != -1) {
        row += offset[action][0];
        col += offset[action][1];
        if (maze[row][col] != TRAINING) maze[row][col] = SOLUTION;
      } else {
        res = ERROR_MAZE;
      }
    }
  }
  return res;
}

// Ask user to mark targets on maze[n][m],
// last marked position is saved to (row, col)
int select_targets(int n, int m, int maze[n][m], int targets, int *row,
                   int *col) {
  int res = 0;
  int selected = 0;
  while (res != ESCAPE && selected < targets) {
    int prev = maze[*row][*col];
    maze[*row][*col] = TARGET;
    print_maze(n, m, maze);
    printw(
        "\nUse arrow keys to position start('X') (if any) and end(also 'X') "
        "points, press Enter to set a point, Enter to solve a maze\n"
        "Training the agent requires only end point");
    refresh();
    res = getch();
    maze[*row][*col] = prev;
    switch (res) {
      case KEY_UP:
        --*row;
        break;
      case KEY_DOWN:
        ++*row;
        break;
      case KEY_LEFT:
        --*col;
        break;
      case KEY_RIGHT:
        ++*col;
        break;
      case ENTER:
        if (prev != WALL && prev != TARGET) {
          ++selected;
          maze[*row][*col] = TARGET;
        }
        break;
    }
    if (*row < 0) *row = n - 1;
    if (*row > n - 1) *row = 0;
    if (*col < 0) *col = m - 1;
    if (*col > m - 1) *col = 0;
  }
  if (res != ESCAPE) res = 0;
  return res;
}
// Ask user to choose an action to for maze[n][m],
// such as solving the maze in normal mode, training a Q-agent on it,
// solving the maze with a Q-agent or saving the maze to a file
int solve_maze_prompt(int n, int m, int maze[n][m]) {
  int res = 0;
  int row = 1;
  int col = 1;
  FILE *file;
  clear();
  do {
    print_maze(n, m, maze);
    res = key_prompt(
        "\nPress any key to generate/load new maze, ESC to get back\n"
        "'x' to solve the maze in NORMAL mode\n"
        "'q' to TRAIN an agent to solve the maze using MACHINE LEARNING\n"
        "'Q' to set the starting point for the TRAINED agent to SOLVE the "
        "maze\n"
        "'s' to save the currently open maze to a file");
    switch (res) {
      case 'x':
        res = select_targets(n, m, maze, 2, &row, &col);
        if (res == 0) {
          solve_core(n, m, maze);
          print_maze(n, m, maze);
          key_prompt("\nPress any key to try again");
        }
        break;
      case 'q':
        res = select_targets(n, m, maze, 1, &row, &col);
        if (res == 0) {
          q_learning(n, m, maze, row, col);
          key_prompt("\nThe agent is trained! Press any key to continue");
        }
        break;
      case 'Q':
        res = detect_target(n, m, maze);
        if (res == 0) res = select_targets(n, m, maze, 1, &row, &col);
        if (res == 0) res = print_q_solution(n, m, maze);
        print_maze(n, m, maze);
        if (res == 0) key_prompt("\nMaze solved! Press any key to try again");
        break;
      case 's':
        file = file_prompt("w");
        save_maze(n, m, maze, file);
        fclose(file);
        key_prompt("\nMaze has been saved. Press any key");
        break;
    }
    clear_maze(n, m, maze);
  } while (res == 0);
  return res;
}

// Ask the user for dimensions to be used for generating a perfect maze,
// generate perfect mazes and allow the user to solve or save them repedeatly
// until the user presses escape
int perfect_maze_prompt() {
  int res = 0;
  int n = 0, m = 0;
  do {
    clear();
    refresh();
    printw(
        "Enter NxM dimensions of the maze. \n"
        "If the dimension is bigger than 40 it's recommended to zoom out "
        "(CTRL + '-')\n");
    scanw("%d %d", &n, &m);
    res = n_m_checker(&n, &m);
    n = n * 2;
    m = m * 2;
    if (m % 2 == 0) {
      m -= 1;
    }
    if (n % 2 == 0) {
      n -= 1;
    }
    if (res == ERROR_DIMENSIONS)
      res = key_prompt(
          "ERROR: dimensions should be > 0\n"
          "Press any key to try again, ESC to get back");
  } while (res != 0 && res != ESCAPE);

  if (res != ESCAPE) {
    n += 2;
    m += 2;
    int maze[n][m];
    res = 0;
    while (res == 0) {
      initialize_perfect_maze(n, m, maze);
      res = solve_maze_prompt(n, m, maze);
    }
  }
  return res;
}

// Ask the user for a filepath to a maze, then allow the user to solve or save
// them repedeatly each time requesting the file path until the user presses
// escape
int maze_from_file_prompt() {
  int res = 0;
  while (res == 0) {
    FILE *file = file_prompt("r");
    if (file == NULL) {
      res = ERROR_FILE;
    } else {
      int n = 0, m = 0;
      fscanf(file, "%d %d", &n, &m);
      int maze[2 * n + 1][2 * m + 1];
      read_maze_from_file(file, n, m, maze);
      n = 2 * n + 1;  // maze with thick walls is 2x bigger than
      m = 2 * m + 1;  // maze with thin walls
      fclose(file);

      res = solve_maze_prompt(n, m, maze);
    }
  }
  return res;
}

// UI entry point
void maze_initializer() {
  const char *prompt =
      "\nUse arrow keys to choose, enter to select, ESC to get back, any key "
      "to try again\n";
  const char *options[6] = {
      "Read the maze from the file",
      "Read the cave from the file",
      "Generate a maze-like structure using cellular automata",
      "Generate a cave using cellular automata",
      "Generate a perfect maze",
      "Exit"};

  int current_option = 0;
  while (current_option != 5) {
    current_option = option_select(prompt, 6, options);

    int user_input = 0;
    while (user_input != ESCAPE) {
      switch (current_option) {
        case 0:
          // read maze from file
          user_input = maze_from_file_prompt();
          break;
        case 1:
          // read cave from file
          user_input = cave_from_file_prompt();
          break;
        case 2:
          user_input = cellular_maze_prompt();
          break;
        case 3:
          // generate cave or maze using cellular automata
          user_input = cave_gen_from_scratch_prompt();
          break;
        case 4:
          // generate perfect maze using Eller's algorithm
          user_input = perfect_maze_prompt();
          break;
        default:
          user_input = ESCAPE;
          break;
      }
      clear();
      if (user_input == ERROR_FILE) {
        user_input = key_prompt(
            "Error opening file OR the agent was not trained! Press any "
            "key to try again, ESC to get back");
      } else if (user_input == ERROR_MAZE) {
        user_input = key_prompt(
            "Error: the agent was trained for another maze! Press any "
            "key to try again, ESC to get back");
      }
    }
  }
}

int main() {
  initscr();             // Initialize ncurses
  keypad(stdscr, TRUE);  // Enable keypad for arrow keys
  start_color();
  srand(time(NULL));
  init_pair(1, COLOR_BLACK, COLOR_YELLOW);
  maze_initializer();
  endwin();
}
