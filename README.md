# Sudoku Solver Project

## Overview
This repository contains a **Sudoku solver** written in C. The solver implements a classic backtracking algorithm enhanced with constraint propagation to efficiently prune the search space. The codebase is organized into a small set of header and source files that define the puzzle representation, solving logic, and a simple command‑line interface.

## Technique
The solver uses the following techniques:
- **Backtracking search** – recursively attempts to fill empty cells while respecting Sudoku rules.
- **Constraint propagation** – after each placement, the algorithm updates the list of possible candidates for the remaining cells, eliminating choices that would violate row, column, or 3×3 box constraints.
- **Heuristic ordering** – cells with the fewest candidates are tried first, dramatically reducing the number of recursive calls.

These strategies together allow the program to solve even the hardest puzzles in a fraction of a second on a modern CPU.

## How to Run
1. **Prerequisites** – A C compiler (e.g., GCC, Clang, MSVC) and a terminal.
2. **Build** – From the project root run:
   ```
   gcc -Wall -O2 -o sudoku_solver Soduko/*.c
   ```
   Adjust the compiler command if you are using a different toolchain.
3. **Execute** – Provide a puzzle file (9×9 grid with `0` for empty cells) as an argument:
   ```
   ./sudoku_solver puzzles/example.txt
   ```
   The program prints the solved board to standard output.

## Development
The project was started as a learning exercise in algorithmic problem solving and C programming. The code follows a modular structure:
- `framework.h` / `framework.c` – core data structures and helper functions for board manipulation.
- `solver.h` / `solver.c` – implementation of the backtracking algorithm and heuristics.
- `main.c` – parses input, invokes the solver, and displays results.

All source files are placed under the `Soduko` folder. The code follows standard C conventions and is documented with inline comments for clarity.

## Author
Developed by **Mennatallah Essam Mounir**.

---
*Feel free to fork the repository, experiment with different heuristics, or integrate the solver into larger applications.*
