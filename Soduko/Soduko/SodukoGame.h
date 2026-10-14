#pragma once
#include <iostream>
#include <cmath>
#include <windows.h>

using namespace std;

enum level { Easy, Medium, Hard };

class SudokuGame
{
private:
    level level_m;
    int grid[9][9];
    int initialGrid[9][9];
    int hintGrid[9][9];   // tracks which cells were filled by hints (shown in green)
    int selectedRow;
    int selectedCol;

public:

    // Constructor
    SudokuGame() : level_m(Easy), selectedRow(-1), selectedCol(-1) {
        memset(grid, 0, sizeof(grid));
        memset(initialGrid, 0, sizeof(initialGrid));
        memset(hintGrid, 0, sizeof(hintGrid));
    };

    void initializeGame(level);
    void selectCell(int row, int col);
    void getSelectedCell(int &row, int &col);
    bool isCellEditable(int row, int col);
    bool setCellValue(int row, int col, int val);
    bool isComplete();
    bool isValidBoard();
    bool getHint();
    void showSolution(HWND hWnd);

    void displayGame(HWND hWnd);

    // Solves the Sudoku using backtracking
    bool solve();

    bool isAvailable(int row, int col, int num);

    // Backtracking to be called in the solve function
    bool backtrack();
};

