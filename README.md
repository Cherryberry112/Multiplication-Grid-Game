# 🎮 Multiplication Grid Game

An interactive, terminal-based strategy game built in C using the **ncurses** library. This project was developed to simulate low-level logic, memory organization, and real-time I/O handling.

###  Game Overview

The Multiplication Grid Game is a tactical battle between a human player and the computer.

* **The Grid**: A 6x6 matrix filled with products of numbers from 1 to 9.
* **The Goal**: Be the first to align **four** of your markers in a row, column, or diagonal.
* **The Twist**: You don't just pick a square; you manipulate two pointers on a selection array (1-9). The product of the two selected numbers determines your move on the grid.


###  Key Features

* **Ncurses Interface**: A beautiful terminal UI with color-coded markers (Green for Player, Magenta for Computer).
* **Smart AI**: A computer opponent that calculates moves based on available products.
* **Architecture Simulation**:
* **Memory Management**: Uses 2D arrays and pointers to simulate low-level data access.
* **Instruction Logic**: Implements win-condition checking through optimized grid scanning.
* **Real-time I/O**: Handles keyboard interrupts and screen refreshes dynamically.



###  Technical Stack

* **Language**: C
* **Library**: `ncurses` (for terminal graphics)
* **Concepts**: Pointer arithmetic, 2D array manipulation, and game state logic.

### 📂 Repository Structure

* **`game.c`**: The complete source code including game logic, AI, and UI rendering.
* **`Multiplication_Game_Report.pdf`**: Detailed technical report covering architectural principles, challenges, and insights gained.
* **`game play.mp4`**: A video walkthrough of the game.

###  How to Run

To play the game on a Linux/Unix system (or macOS/WSL):

1. **Install ncurses** (if not already installed):
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev

```


2. **Compile the code**:
```bash
gcc game.c -o multiplication_game -lncurses

```


3. **Run the game**:
```bash
./multiplication_game

```

### 💡 Future Improvements

* **Advanced AI**: Implementing a minimax algorithm for a more challenging opponent.
* **Multiplayer**: Adding a local 1v1 mode.
* **Custom Grids**: Allowing players to choose different grid sizes and win-stretch lengths.

