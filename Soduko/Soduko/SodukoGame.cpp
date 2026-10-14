#include "SodukoGame.h"
#include "Program.h"
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <gdiplusfont.h>
#include <gdiplusfontfamily.h>

#pragma comment(lib, "gdiplus.lib")

using namespace std;
using namespace Gdiplus;

void SudokuGame::initializeGame(level difficulty)
{
    this->level_m = difficulty;
    selectedRow = -1;
    selectedCol = -1;
    memset(hintGrid, 0, sizeof(hintGrid));

    switch (difficulty) {
    case Easy: {
        int puzzle[9][9] = {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
        };
        memcpy(grid, puzzle, sizeof(grid));
        break;
    }
    case Medium: {
        int puzzle[9][9] = {
            {0, 0, 3, 0, 2, 0, 6, 0, 0},
            {9, 0, 0, 3, 0, 5, 0, 0, 1},
            {0, 0, 1, 8, 0, 6, 4, 0, 0},
            {0, 0, 8, 1, 0, 2, 9, 0, 0},
            {7, 0, 0, 0, 0, 0, 0, 0, 8},
            {0, 0, 6, 7, 0, 8, 2, 0, 0},
            {0, 0, 2, 6, 0, 9, 5, 0, 0},
            {8, 0, 0, 2, 0, 3, 0, 0, 9},
            {0, 0, 5, 0, 1, 0, 3, 0, 0}
        };
        memcpy(grid, puzzle, sizeof(grid));
        break;
    }
    case Hard: {
        int puzzle[9][9] = {
            {0, 0, 0, 0, 0, 0, 0, 1, 2},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 6, 4, 5, 0, 0},
            {0, 0, 0, 0, 2, 0, 0, 0, 0},
            {0, 0, 0, 5, 0, 9, 0, 0, 0},
            {0, 0, 0, 0, 3, 0, 0, 0, 0},
            {0, 0, 8, 7, 1, 0, 6, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 9, 6, 0, 0, 0, 0, 0, 0}
        };
        memcpy(grid, puzzle, sizeof(grid));
        break;
    }
    default:
        memset(grid, 0, sizeof(grid));
        break;
    }

    // Lock pre-filled cells — they cannot be edited by the user
    memcpy(initialGrid, grid, sizeof(initialGrid));
}

void SudokuGame::displayGame(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    int gridSize = 450;
    int cellSize = gridSize / 9;  // 50px per cell
    int startX   = 50;
    int startY   = 50;

    // Draw selected cell highlight
    if (selectedRow >= 0 && selectedRow < 9 && selectedCol >= 0 && selectedCol < 9) {
        HBRUSH hBrush = CreateSolidBrush(RGB(180, 220, 255));
        RECT rect;
        rect.left   = startX + selectedCol * cellSize + 2;
        rect.top    = startY + selectedRow * cellSize + 2;
        rect.right  = rect.left + cellSize - 3;
        rect.bottom = rect.top  + cellSize - 3;
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
    }

    // Draw grid lines
    for (int i = 0; i <= 9; ++i) {
        int x = startX + i * cellSize;
        int y = startY + i * cellSize;
        int penWidth = (i % 3 == 0) ? 3 : 1;

        HPEN hPen = CreatePen(PS_SOLID, penWidth, RGB(0, 0, 0));
        SelectObject(hdc, hPen);
        MoveToEx(hdc, x, startY, nullptr);
        LineTo(hdc, x, startY + gridSize);
        DeleteObject(hPen);

        hPen = CreatePen(PS_SOLID, penWidth, RGB(0, 0, 0));
        SelectObject(hdc, hPen);
        MoveToEx(hdc, startX, y, nullptr);
        LineTo(hdc, startX + gridSize, y);
        DeleteObject(hPen);
    }

    // GDI+ for perfectly centered text
    Graphics graphics(hdc);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

    FontFamily family(L"Arial");
    Font font(&family, 26, FontStyleBold, UnitPixel);

    SolidBrush fixedBrush(Color(255, 30,  30,  30));   // Black for pre-filled cells
    SolidBrush userBrush (Color(255, 30,  80, 200));   // Blue  for user-entered
    SolidBrush hintBrush (Color(255,  0, 150,  60));   // Green for hints

    StringFormat sf;
    sf.SetAlignment(StringAlignmentCenter);
    sf.SetLineAlignment(StringAlignmentCenter);

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                wchar_t str[2] = { (wchar_t)(grid[i][j] + L'0'), L'\0' };

                RectF cellRect(
                    (REAL)(startX + j * cellSize),
                    (REAL)(startY + i * cellSize),
                    (REAL)cellSize,
                    (REAL)cellSize
                );

                SolidBrush* brush;
                if (hintGrid[i][j])          brush = &hintBrush;   // green
                else if (initialGrid[i][j])  brush = &fixedBrush;  // black
                else                         brush = &userBrush;   // blue

                graphics.DrawString(str, 1, &font, cellRect, &sf, brush);
            }
        }
    }

    EndPaint(hWnd, &ps);
}

void SudokuGame::selectCell(int row, int col)
{
    if (row >= -1 && row < 9 && col >= -1 && col < 9) {
        selectedRow = row;
        selectedCol = col;
    }
}

void SudokuGame::getSelectedCell(int &row, int &col)
{
    row = selectedRow;
    col = selectedCol;
}

bool SudokuGame::isCellEditable(int row, int col)
{
    if (row >= 0 && row < 9 && col >= 0 && col < 9)
        return initialGrid[row][col] == 0 && hintGrid[row][col] == 0;
    return false;
}

bool SudokuGame::setCellValue(int row, int col, int val)
{
    if (isCellEditable(row, col) && val >= 0 && val <= 9) {
        grid[row][col] = val;
        return true;
    }
    return false;
}

bool SudokuGame::isValidBoard()
{
    for (int i = 0; i < 9; i++) {
        bool seen[10] = { false };
        for (int j = 0; j < 9; j++) {
            int v = grid[i][j];
            if (v == 0 || seen[v]) return false;
            seen[v] = true;
        }
    }
    for (int j = 0; j < 9; j++) {
        bool seen[10] = { false };
        for (int i = 0; i < 9; i++) {
            int v = grid[i][j];
            if (v == 0 || seen[v]) return false;
            seen[v] = true;
        }
    }
    for (int boxRow = 0; boxRow < 3; boxRow++) {
        for (int boxCol = 0; boxCol < 3; boxCol++) {
            bool seen[10] = { false };
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    int v = grid[boxRow * 3 + r][boxCol * 3 + c];
                    if (v == 0 || seen[v]) return false;
                    seen[v] = true;
                }
            }
        }
    }
    return true;
}

bool SudokuGame::isComplete()
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (grid[i][j] == 0) return false;
    return isValidBoard();
}

bool SudokuGame::getHint()
{
    int savedGrid[9][9];
    memcpy(savedGrid, grid, sizeof(savedGrid));

    if (!backtrack()) {
        memcpy(grid, savedGrid, sizeof(grid));
        return false;
    }

    int solvedGrid[9][9];
    memcpy(solvedGrid, grid, sizeof(solvedGrid));
    memcpy(grid, savedGrid, sizeof(grid));

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (savedGrid[i][j] == 0) {
                grid[i][j]     = solvedGrid[i][j];
                hintGrid[i][j] = 1;
                return true;
            }
        }
    }
    return false;
}

void SudokuGame::showSolution(HWND hWnd)
{
    int savedGrid[9][9];
    memcpy(savedGrid, grid, sizeof(savedGrid));

    if (backtrack()) {
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (savedGrid[i][j] == 0) hintGrid[i][j] = 1;
        InvalidateRect(hWnd, NULL, TRUE);
    } else {
        MessageBox(hWnd,
            L"No valid solution exists.\nCheck that you haven't entered any conflicts.",
            L"No Solution Found", MB_OK | MB_ICONWARNING);
        memcpy(grid, savedGrid, sizeof(grid));
    }
}

bool SudokuGame::solve()
{
    return backtrack();
}

bool SudokuGame::isAvailable(int row, int col, int num)
{
    for (int c = 0; c < 9; ++c)
        if (grid[row][c] == num) return false;
    for (int r = 0; r < 9; ++r)
        if (grid[r][col] == num) return false;

    int boxStartRow = row - row % 3;
    int boxStartCol = col - col % 3;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (grid[r + boxStartRow][c + boxStartCol] == num) return false;
    return true;
}

bool SudokuGame::backtrack()
{
    int row = -1, col = -1;
    bool isEmpty = false;

    for (int i = 0; i < 9 && !isEmpty; i++)
        for (int j = 0; j < 9 && !isEmpty; j++)
            if (grid[i][j] == 0) { row = i; col = j; isEmpty = true; }

    if (!isEmpty) return true;

    for (int num = 1; num <= 9; num++) {
        if (isAvailable(row, col, num)) {
            grid[row][col] = num;
            if (backtrack()) return true;
            grid[row][col] = 0;
        }
    }
    return false;
}
