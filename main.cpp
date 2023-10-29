#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include "mano.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int tileSize = 70;
int numOfTiles = 10;

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (GetModuleHandle(NULL), "ikona");
    wincl.hIconSm = LoadIcon (GetModuleHandle(NULL), "ikona");
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(255, 0, 0));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("ISMINUOTOJAS"),  /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           0,       /* Windows decides the position */
           0,       /* where the window ends up on the screen */
           numOfTiles*tileSize+16,               /* The programs width in pixels*/
           numOfTiles*tileSize+58,               /* and height */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           LoadMenu(NULL, MAKEINTRESOURCE(IDR_MYMENU)),           /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::string filename = "bomb_location2.csv";

    HANDLE fileHandle = CreateFile(
        filename.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return 1;
    }

    std::vector<int> integerVector;
    int intValue;
    DWORD bytesRead;

    while (ReadFile(fileHandle, &intValue, sizeof(int), &bytesRead, NULL) && bytesRead > 0) {
        integerVector.push_back(intValue);
    }

    CloseHandle(fileHandle);

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            for (int row = 0; row < numOfTiles; row++) {
                for (int col = 0; col < numOfTiles; col++) {
                    int buttonID = 1000 + row * numOfTiles + col; // Calculate a unique ID for each button
                    std::cout << buttonID << std::endl;

                    CreateWindow(
                    "BUTTON",
                    "Press me",
                    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    col * tileSize, row * tileSize, tileSize, tileSize,
                    hwnd,
                    (HMENU)buttonID,
                    (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                    NULL
                    );
                }
            }
                break;

        case WM_COMMAND:
            switch(LOWORD(wParam)){
                case 1000:
                    DialogBox(NULL, MAKEINTRESOURCE(MYDIALOG), hwnd, (DLGPROC)DialogProc);
                    break;
                case ID_PROGRAM_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                 case ID_PROGRAM_HELP:
                    MessageBox(hwnd, TEXT("Test your luck - click on any one of the tiles"), "Help page", MB_OK);
                    break;
                 default:
                    DialogBox(NULL, MAKEINTRESOURCE(MYDIALOG), hwnd, (DLGPROC)DialogProc);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage (0);      /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
    LPTSTR edittxt;
    switch(uMsg){
        case WM_INITDIALOG:
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)){
            case DIALOG_BUTTON:
                HWND edit = GetDlgItem(hDlg, TEKSTAS);
                int length = GetWindowTextLength(edit);
                edittxt = new TCHAR[length+1];
                GetWindowText(edit, edittxt, length+1);
                SetWindowText(GetParent(hDlg), edittxt);
                EndDialog(hDlg, 0);
                return TRUE;
            }
        return TRUE;
            case WM_CLOSE:
                EndDialog(hDlg, 0);
                return TRUE;
            case WM_DESTROY:
                EndDialog(hDlg, 0);
                return TRUE;
    }
    return FALSE;
}
