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
#include <algorithm>
#include <cstdlib>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int tileSize = 70;
int numOfTiles = 10;
int score = 0;

bool isABomb(int ButtonID){
    std::string filename = "bomb_location2.csv";
    const int maxArraySize = 100;

      HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return 1;
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE || dwFileSize == 0) {
        CloseHandle(hFile);
        std::cerr << "File is empty or could not determine file size." << std::endl;
        return 1;
    }

    std::vector<int> BombLocations;
    DWORD bytesRead;
    char buffer[1024]; // You can adjust the buffer size as needed

    while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
        for (DWORD i = 0; i < bytesRead; ++i) {
            if (isdigit(buffer[i])) {
                int intValue = atoi(&buffer[i]);
                BombLocations.push_back(intValue);
                while (i < bytesRead && isdigit(buffer[i])) {
                    ++i; // Move to the next non-digit character
                }
            }
        }
    }

    CloseHandle(hFile);

    if((std::find(BombLocations.begin(), BombLocations.end(), ButtonID)) != BombLocations.end()){
        return true;
    }
    else{
        return false;
    }
}

void draw_image(HWND hwnd, int coordX, int coordY){
    HDC hdc;
    hdc = GetDC(hwnd);
    HANDLE bmp = LoadImage(NULL, "bomb3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if(bmp==NULL){
        MessageBox(hwnd, "Failed to load image", "Error", MB_OK|MB_ICONERROR);
        return;
    }
    HDC dcmem = CreateCompatibleDC(NULL);
    if(SelectObject(dcmem, bmp)==NULL){
        MessageBox(hwnd, "Failed to upload", "Error", MB_OK|MB_ICONERROR);
        DeleteDC(dcmem);
        return;
    }
    BITMAP bm;
    GetObject(bmp, sizeof(bm), &bm);
    if(BitBlt(hdc, coordX, coordY, 70, 70, dcmem, 0, 0, SRCCOPY) == 0){
        MessageBox(hwnd, "Failed to blit", "Error", MB_OK|MB_ICONERROR);;
        DeleteDC(dcmem);
        return;
    }
    DeleteDC(dcmem);
}

/*BOOL CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
}*/

INT_PTR CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hwnd, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}


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


    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            for (int row = 0; row < numOfTiles; row++) {
                for (int col = 0; col < numOfTiles; col++) {
                    int buttonID = 1000 + row * numOfTiles + col; // Calculate a unique ID for each button
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
                case ID_PROGRAM_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                 case ID_PROGRAM_HELP:
                    MessageBox(hwnd, TEXT("Test your luck - click on any one of the tiles"), "Help page", MB_OK);
                    break;
                 default:
                    if(isABomb(LOWORD(wParam))){
                        std::cout << LOWORD(wParam) << " is a BOMB. Your score = " << score << std::endl;
                        score = 0;

                        RECT buttonRect;
                        HWND hwndButton = GetDlgItem(hwnd, LOWORD(wParam)); // Get the button's handle using its ID
                        int buttonX, buttonY;
                        if (hwndButton != NULL) {
                            GetWindowRect(hwndButton, &buttonRect); // Get the screen coordinates of the button

                            // Convert the screen coordinates to client coordinates of the parent window
                            ScreenToClient(hwnd, (LPPOINT)&buttonRect);
                            ScreenToClient(hwnd, ((LPPOINT)&buttonRect) + 1);

                            buttonX = buttonRect.left;
                            buttonY = buttonRect.top;
                        }
                        draw_image(hwnd, buttonX, buttonY);
                        //MessageBox(hwnd, TEXT("Test your luck - click on any one of the tiles"), "Help page", MB_OK);
                        //DialogBox(NULL, MAKEINTRESOURCE(MYDIALOG), hwnd, (DLGPROC)DialogProc);
                        system("pause");
                    }
                    else{
                        score++;
                    }
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
