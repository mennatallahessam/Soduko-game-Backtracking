// Soduko.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Program.h"
#include "SodukoGame.h"
#include <objidl.h>
#include <gdiplus.h>


// IDs for dynamically-added Game menu items
#define ID_HINT    40001
#define ID_SOLVE   40002
#define ID_GIVEUP  40003

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

SudokuGame *game = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // TODO: Place code here.
    game = new SudokuGame();  // Create game object with Easy level
    game->initializeGame(Easy);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SODUKO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SODUKO));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Shutdown GDI+
    Gdiplus::GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SODUKO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SODUKO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT, 570, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Dynamically add a Game menu with Hint / View Solution / Give Up
        HMENU hMenu = GetMenu(hWnd);
        if (hMenu) {
            HMENU hGameMenu = CreatePopupMenu();
            AppendMenu(hGameMenu, MF_STRING,    ID_HINT,   L"Hint\tH");
            AppendMenu(hGameMenu, MF_STRING,    ID_SOLVE,  L"View Solution\tS");
            AppendMenu(hGameMenu, MF_SEPARATOR, 0,         NULL);
            AppendMenu(hGameMenu, MF_STRING,    ID_GIVEUP, L"Give Up");
            AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGameMenu, L"Game");
            DrawMenuBar(hWnd);
        }
        break;
    }

    case WM_PAINT:
        game->displayGame(hWnd);
        break;

    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        // Grid constants matching displayGame
        int startX = 50;
        int startY = 50;
        int gridSize = 450;
        int cellSize = gridSize / 9;

        if (x >= startX && x < startX + gridSize && y >= startY && y < startY + gridSize) {
            int col = (x - startX) / cellSize;
            int row = (y - startY) / cellSize;
            game->selectCell(row, col);
            InvalidateRect(hWnd, NULL, TRUE);
        } else {
            game->selectCell(-1, -1);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }

    case WM_CHAR:
    {
        wchar_t ch = (wchar_t)wParam;
        int row, col;
        game->getSelectedCell(row, col);

        // Global shortcuts (work even without a cell selected)
        if (ch == L'h' || ch == L'H') { SendMessage(hWnd, WM_COMMAND, ID_HINT,  0); break; }
        if (ch == L's' || ch == L'S') { SendMessage(hWnd, WM_COMMAND, ID_SOLVE, 0); break; }
        if (ch == L'g' || ch == L'G') { SendMessage(hWnd, WM_COMMAND, ID_GIVEUP,0); break; }

        if (row >= 0 && col >= 0) {
            if (ch >= L'1' && ch <= L'9') {
                int val = ch - L'0';
                if (game->setCellValue(row, col, val)) {
                    InvalidateRect(hWnd, NULL, TRUE);
                    if (game->isComplete()) {
                        MessageBox(hWnd, L"Congratulations! You solved the Sudoku!",
                            L"You Win!", MB_OK | MB_ICONINFORMATION);
                    }
                }
            }
            else if (ch == L'0' || ch == 8 || ch == 127) {
                if (game->setCellValue(row, col, 0)) {
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
        }
        break;
    }

    case WM_KEYDOWN:
    {
        int row, col;
        game->getSelectedCell(row, col);
        // If no cell selected, any navigation key selects the first cell
        if (row < 0 && (wParam == VK_TAB || wParam == VK_UP || wParam == VK_DOWN ||
                        wParam == VK_LEFT || wParam == VK_RIGHT)) {
            game->selectCell(0, 0);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        if (row >= 0 && col >= 0) {
            switch (wParam) {
            case VK_UP:    game->selectCell(row > 0 ? row - 1 : 0, col); break;
            case VK_DOWN:  game->selectCell(row < 8 ? row + 1 : 8, col); break;
            case VK_LEFT:  game->selectCell(row, col > 0 ? col - 1 : 0); break;
            case VK_RIGHT: game->selectCell(row, col < 8 ? col + 1 : 8); break;
            case VK_TAB: {
                bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                if (!shift) { col++; if (col > 8) { col = 0; row = row < 8 ? row + 1 : 0; } }
                else        { col--; if (col < 0) { col = 8; row = row > 0 ? row - 1 : 8; } }
                game->selectCell(row, col);
                break;
            }
            case VK_DELETE:
                game->setCellValue(row, col, 0);
                break;
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case ID_NEWGAME_EASY:
        case ID_FILE_NEWGAME:
            game->initializeGame(Easy);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case ID_NEWGAME_MEDIUM:
            game->initializeGame(Medium);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case ID_NEWGAME_HARD:
            game->initializeGame(Hard);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case ID_HINT:
            if (!game->getHint()) {
                MessageBox(hWnd,
                    L"No hint available. Check for conflicts or all cells are filled.",
                    L"Hint", MB_OK | MB_ICONINFORMATION);
            } else {
                InvalidateRect(hWnd, NULL, TRUE);
                if (game->isComplete()) {
                    MessageBox(hWnd, L"The board is now complete!",
                        L"Solved!", MB_OK | MB_ICONINFORMATION);
                }
            }
            break;
        case ID_SOLVE:
            game->showSolution(hWnd);
            break;
        case ID_GIVEUP: {
            int res = MessageBox(hWnd,
                L"Are you sure you want to give up and see the solution?",
                L"Give Up?", MB_YESNO | MB_ICONQUESTION);
            if (res == IDYES) game->showSolution(hWnd);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
