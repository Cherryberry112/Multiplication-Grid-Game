#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

#define ROWS 6
#define COLS 6
#define MAX_MOVES 36

int grid[ROWS][COLS] = {
    {1, 2, 3, 4, 5, 6},
    {7, 8, 9, 10, 12, 14},
    {15, 16, 18, 20, 21, 24},
    {25, 27, 28, 30, 32, 35},
    {36, 40, 42, 45, 48, 49},
    {54, 56, 63, 64, 72, 81}
};

int used[ROWS][COLS]; // 0=empty,1=player,2=computer
int selection_array[9] = {1,2,3,4,5,6,7,8,9};
int ptr1, ptr2; // Indexes in selection_array
int move_count = 0;
int current_player = 1; // 1 = human, 2 = computer

void init_colors() {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);    // Default
    init_pair(2, COLOR_GREEN, COLOR_BLACK);    // Player (Green)
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);  // Computer (Magenta)
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);   // Pointer
    init_pair(5, COLOR_RED, COLOR_BLACK);      // Invalid move
    init_pair(6, COLOR_CYAN, COLOR_BLACK);     // Active pointer
    init_pair(7, COLOR_BLACK, COLOR_GREEN);    // Player win highlight
    init_pair(8, COLOR_BLACK, COLOR_MAGENTA);  // Computer win highlight
    init_pair(9, COLOR_WHITE, COLOR_BLUE);     // Title
    init_pair(10, COLOR_BLACK, COLOR_YELLOW);  // Victory animation 1
    init_pair(11, COLOR_BLACK, COLOR_CYAN);    // Victory animation 2
    init_pair(12, COLOR_BLACK, COLOR_RED);     // Victory animation 3
}

void draw_title() {
    attron(COLOR_PAIR(9) | A_BOLD);
    mvprintw(0, 5, "MULTIPLICATION GRID GAME");
    attroff(COLOR_PAIR(9) | A_BOLD);

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(1, 5, "PLAYER: GREEN");
    attroff(COLOR_PAIR(2) | A_BOLD);

    printw(" | ");

    attron(COLOR_PAIR(3) | A_BOLD);
    printw("COMPUTER: MAGENTA");
    attroff(COLOR_PAIR(3) | A_BOLD);

    printw(" | ");

    attron(COLOR_PAIR(4) | A_BOLD);
    printw("POINTERS: YELLOW");
    attroff(COLOR_PAIR(4) | A_BOLD);
}

void highlight_cells(int cells[4][2], int player) {
    int color = (player == 1) ? 7 : 8;
    for (int k = 0; k < 4; k++) {
        int r = cells[k][0];
        int c = cells[k][1];
        move(3 + r, 4 + c * 4);
        attron(COLOR_PAIR(color) | A_BOLD);
        printw("%3d", grid[r][c]);
        attroff(COLOR_PAIR(color) | A_BOLD);
    }
}

void victory_animation(int player, int cells[4][2]) {
    int colors[3] = {10, 11, 12};
    for (int i = 0; i < 9; i++) {
        for (int k = 0; k < 4; k++) {
            int r = cells[k][0];
            int c = cells[k][1];
            move(3 + r, 4 + c * 4);
            attron(COLOR_PAIR(colors[i % 3]) | A_BOLD);
            printw("%3d", grid[r][c]);
            attroff(COLOR_PAIR(colors[i % 3]) | A_BOLD);
        }
        refresh();
        usleep(150000);
    }
}

void draw_grid() {
    int y = 3, x = 4;
    for (int i = 0; i < ROWS; i++) {
        move(y + i, x);
        for (int j = 0; j < COLS; j++) {
            if (used[i][j] == 1)
                attron(COLOR_PAIR(2)); // Player
            else if (used[i][j] == 2)
                attron(COLOR_PAIR(3)); // Computer
            else
                attron(COLOR_PAIR(1));
            printw("%3d ", grid[i][j]);
            attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));
        }
    }
}

void draw_selection_array() {
    int y = 11, x = 5;
    move(y, x);
    for (int i = 0; i < 9; i++) {
        if (i == ptr1 || i == ptr2) {
            attron(COLOR_PAIR(4));
        }
        printw("[%d] ", selection_array[i]);
        if (i == ptr1 || i == ptr2) {
            attroff(COLOR_PAIR(4));
        }
    }

    // Draw pointer indicators
    move(y + 1, x + ptr1 * 4);
    attron(COLOR_PAIR(current_player == 1 ? 6 : 4));
    printw("^  ");
    attroff(COLOR_PAIR(current_player == 1 ? 6 : 4));

    move(y + 2, x + ptr2 * 4);
    attron(COLOR_PAIR(current_player == 1 ? 6 : 4));
    printw("^  ");
    attroff(COLOR_PAIR(current_player == 1 ? 6 : 4));

    // Draw current product
    int product = selection_array[ptr1] * selection_array[ptr2];
    move(y + 3, x);
    printw("Current product: ");
    attron(A_BOLD);
    printw("%d * %d = %d", selection_array[ptr1], selection_array[ptr2], product);
    attroff(A_BOLD);
}

int find_in_grid(int val, int *row, int *col) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == val) {
                *row = i;
                *col = j;
                return 1;
            }
        }
    }
    return 0;
}

int is_move_valid(int val) {
    int r, c;
    if (!find_in_grid(val, &r, &c)) return 0;
    return !used[r][c];
}

void apply_move(int val, int player_id) {
    int r, c;
    if (find_in_grid(val, &r, &c)) {
        used[r][c] = player_id;
        move_count++;
    }
}

// Checks whether the given player (1 or 2) has four in a row in any direction
// If winner found, stores winning cells coordinates in win_cells and returns 1
int check_winner(int player, int win_cells[4][2]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (used[i][j] != player) continue;

            // Horizontal
            if (j <= COLS - 4 &&
                used[i][j+1] == player &&
                used[i][j+2] == player &&
                used[i][j+3] == player) {
                for (int k = 0; k < 4; k++) {
                    win_cells[k][0] = i;
                    win_cells[k][1] = j + k;
                }
                return 1;
            }

            // Vertical
            if (i <= ROWS - 4 &&
                used[i+1][j] == player &&
                used[i+2][j] == player &&
                used[i+3][j] == player) {
                for (int k = 0; k < 4; k++) {
                    win_cells[k][0] = i + k;
                    win_cells[k][1] = j;
                }
                return 1;
            }

            // Diagonal down-right
            if (i <= ROWS - 4 && j <= COLS - 4 &&
                used[i+1][j+1] == player &&
                used[i+2][j+2] == player &&
                used[i+3][j+3] == player) {
                for (int k = 0; k < 4; k++) {
                    win_cells[k][0] = i + k;
                    win_cells[k][1] = j + k;
                }
                return 1;
            }

            // Anti-diagonal down-left
            if (i <= ROWS - 4 && j >= 3 &&
                used[i+1][j-1] == player &&
                used[i+2][j-2] == player &&
                used[i+3][j-3] == player) {
                for (int k = 0; k < 4; k++) {
                    win_cells[k][0] = i + k;
                    win_cells[k][1] = j - k;
                }
                return 1;
            }
        }
    }
    return 0;
}

int select_pointer() {
    int selected_ptr = 0; // 0 for ptr1, 1 for ptr2
    int key;

    while (1) {
        clear();
        draw_title();
        draw_grid();
        draw_selection_array();

        move(15, 5);
        printw("Choose pointer to move (LEFT/RIGHT to switch, ENTER to select):");
        move(16, 5);
        if (selected_ptr == 0) {
            attron(COLOR_PAIR(6));
            printw("Pointer 1 (%d)", selection_array[ptr1]);
            attroff(COLOR_PAIR(6));
            printw("  Pointer 2 (%d)", selection_array[ptr2]);
        } else {
            printw("Pointer 1 (%d)  ", selection_array[ptr1]);
            attron(COLOR_PAIR(6));
            printw("Pointer 2 (%d)", selection_array[ptr2]);
            attroff(COLOR_PAIR(6));
        }

        refresh();
        key = getch();

        if (key == KEY_LEFT) selected_ptr = 0;
        else if (key == KEY_RIGHT) selected_ptr = 1;
        else if (key == '\n') return selected_ptr;
    }
}

int select_new_value(int *ptr) {
    int new_ptr = *ptr;
    int key;

    while (1) {
        clear();
        draw_title();
        draw_grid();
        draw_selection_array();

        move(15, 5);
        printw("Choose new value (LEFT/RIGHT to move, ENTER to confirm):");
        move(16, 5);
        printw("Current selection: %d", selection_array[new_ptr]);

        // Highlight the pointer being moved
        move(11, 5 + new_ptr * 4);
        attron(COLOR_PAIR(6));
        printw("[%d]", selection_array[new_ptr]);
        attroff(COLOR_PAIR(6));

        refresh();
        key = getch();

        if (key == KEY_LEFT && new_ptr > 0) new_ptr--;
        else if (key == KEY_RIGHT && new_ptr < 8) new_ptr++;
        else if (key == '\n') {
            if (new_ptr != (ptr == &ptr1 ? ptr2 : ptr1)) {
                *ptr = new_ptr;
                return selection_array[*ptr];
            } else {
                move(17, 5);
                attron(COLOR_PAIR(5));
                printw("Cannot select the same pointer! Try again.");
                attroff(COLOR_PAIR(5));
                getch();
            }
        }
    }
}

void player_move() {
    int result;
    while (1) {
        int ptr_to_move = select_pointer();
        int original_ptr1 = ptr1;
        int original_ptr2 = ptr2;

        if (ptr_to_move == 0) {
            select_new_value(&ptr1);
        } else {
            select_new_value(&ptr2);
        }

        result = selection_array[ptr1] * selection_array[ptr2];
        if (is_move_valid(result)) {
            apply_move(result, 1);
            break;
        } else {
            ptr1 = original_ptr1;
            ptr2 = original_ptr2;
            move(17, 5);
            attron(COLOR_PAIR(5));
            printw("Invalid move (%d). Press any key to try again...", result);
            attroff(COLOR_PAIR(5));
            getch();
        }
    }
}

void computer_move() {
    int best_result = -1;
    int best_ptr1 = ptr1;
    int best_ptr2 = ptr2;

    // Try moving ptr1 to find first valid move
    for (int i = 0; i < 9; i++) {
        if (i == ptr2) continue;
        int test_ptr1 = i;
        int val = selection_array[test_ptr1] * selection_array[ptr2];
        if (is_move_valid(val)) {
            best_result = val;
            best_ptr1 = test_ptr1;
            best_ptr2 = ptr2;
            break;
        }
    }
    // If not found try moving ptr2
    if (best_result == -1) {
        for (int i = 0; i < 9; i++) {
            if (i == ptr1) continue;
            int test_ptr2 = i;
            int val = selection_array[ptr1] * selection_array[test_ptr2];
            if (is_move_valid(val)) {
                best_result = val;
                best_ptr1 = ptr1;
                best_ptr2 = test_ptr2;
                break;
            }
        }
    }
    // Apply best move found
    if (best_result != -1) {
        ptr1 = best_ptr1;
        ptr2 = best_ptr2;
        apply_move(best_result, 2);
        clear();
        draw_title();
        draw_grid();
        draw_selection_array();
        move(15, 5);
        printw("Computer moves: %d * %d = %d",
               selection_array[ptr1], selection_array[ptr2], best_result);
        refresh();
        getch();
    } else {
        // No valid moves possible (should rarely occur)
        move_count = MAX_MOVES; // Force game end
    }
}

int main() {
    int win_cells[4][2];
    srand(time(0));
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    init_colors();

    ptr1 = rand() % 9;
    do {
        ptr2 = rand() % 9;
    } while (ptr2 == ptr1);

    while (move_count < MAX_MOVES) {
        current_player = 1;
        clear();
        draw_title();
        draw_grid();
        draw_selection_array();
        refresh();

        player_move();

        if (check_winner(1, win_cells)) {
            clear();
            draw_title();
            draw_grid();
            highlight_cells(win_cells, 1);
            draw_selection_array();
            refresh();
            victory_animation(1, win_cells);
            mvprintw(15, 5, "PLAYER WINS! Press any key to exit.");
            refresh();
            getch();
            break;
        }
        if (move_count >= MAX_MOVES) break;

        current_player = 2;
        computer_move();

        if (check_winner(2, win_cells)) {
            clear();
            draw_title();
            draw_grid();
            highlight_cells(win_cells, 2);
            draw_selection_array();
            refresh();
            victory_animation(2, win_cells);
            mvprintw(15, 5, "COMPUTER WINS! Press any key to exit.");
            refresh();
            getch();
            break;
        }
    }

    if (move_count >= MAX_MOVES && !check_winner(1, win_cells) && !check_winner(2, win_cells)) {
        mvprintw(17, 5, "Game ended in a DRAW! Press any key to exit.");
        refresh();
        getch();
    }

    endwin();
    return 0;
}

