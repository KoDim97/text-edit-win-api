#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "menu.h"
#include "scroll.h"
#include "models.h"
#include "textout.h"


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("Reader");

int WINAPI WinMain(HINSTANCE hThisInstance,
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
    wincl.style = CS_DBLCLKS | CS_OWNDC;                 /* Catch float-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = "Reader"; /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Text reader"),       /* Title Text */
               WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               lpszArgument               /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0))
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


/**
Open window to choose new file

PARAM[IN] hwnd - context device
PARAM[IN] sfile - name of new file

RETURN - TRUE if successfully, FALSE if otherwise
**/

bool ShowDialog(HWND hwnd, TCHAR* sfile)
{

    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(sfile, sizeof(TCHAR)*MAX_PATH);
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = sfile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = NULL;
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = TEXT("Please Select a File");
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lCustData = 0;
    ofn.lpfnHook = 0;
    ofn.lpTemplateName = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    return GetOpenFileName(&ofn);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static HWND hWndEdit;
    static struct AppModel appmodel;
    static TEXTMETRIC tm;
    static PAINTSTRUCT ps;
    static RECT rect;
    switch (message)                  /* handle the messages */
    {
    case WM_COMMAND:
    {
        switch((int)LOWORD(wParam))
        {
        case IDM_OPEN:
        {
            char name[1000];
            if(!ShowDialog(hwnd, name))
            {
                break;
            }
            free(appmodel.viewmodel.virtualShift);
            free(appmodel.stormodel.shift);
            if(FileReading(hwnd, name, &appmodel))
            {
                appmodel.viewmodel.virtualShift = NULL;
                InitPoint(&(appmodel.viewmodel.scrollPos));
                InitPoint(&(appmodel.viewmodel.curPlace));
                StorageStructBuilding(&appmodel);
                if (appmodel.viewmodel.withFormating){
                    CreateVirtualStrings(&appmodel);
                }
                InitScroll(hwnd, &appmodel);
            }
            InvalidateRect(hwnd, 0, TRUE);
            break;
        }
        case IDM_MODE_FORMATING:
        {
            if (!appmodel.viewmodel.withFormating)
            {
                int curAbsolutePosition = appmodel.stormodel.shift[appmodel.viewmodel.curPlace.y];
                CreateVirtualStrings(&appmodel);
                appmodel.viewmodel.withFormating = TRUE;
                FindNewPos(&appmodel, curAbsolutePosition);
                InitScroll(hwnd, &appmodel);
                InvalidateRect(hwnd, 0, TRUE);
            }
            break;
        }
        case IDM_MODE_NO_FORMATING:
        {
            if (appmodel.viewmodel.withFormating)
            {
                int maxLineInWin = appmodel.viewmodel.WindowHeight / appmodel.viewmodel.LetterHeight;
                int curAbsolutePosition = appmodel.viewmodel.virtualShift[appmodel.viewmodel.curPlace.y];
                free(appmodel.viewmodel.virtualShift);
                appmodel.viewmodel.virtualShift = NULL;
                appmodel.viewmodel.withFormating = FALSE;
                FindNewPos(&appmodel, curAbsolutePosition);
                if (appmodel.viewmodel.curPlace.y > appmodel.stormodel.numOfString - maxLineInWin){
                    appmodel.viewmodel.curPlace.y = appmodel.stormodel.numOfString - maxLineInWin;
                }
                InitScroll(hwnd, &appmodel);
                InvalidateRect(hwnd, 0, TRUE);
            }
            break;
        }
        case IDM_EXIT:
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0L);
        }
        }
        break;
    }
    case WM_CREATE:
    {
        CREATESTRUCT* pcs;
        pcs = (CREATESTRUCT*)lParam;
        char* name = (char*)pcs->lpCreateParams;
        HDC hdc;
        hdc = GetDC(hwnd);
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT)); //Установка моноширинного шрифта
        GetTextMetrics(hdc, &tm);
        if (FileReading(hwnd, name, &appmodel))
        {
            appmodel.viewmodel.withFormating = FALSE; // по умолчанию
            appmodel.viewmodel.LetterWidth = tm.tmAveCharWidth;
            appmodel.viewmodel.LetterHeight = tm.tmHeight + tm.tmExternalLeading;
            appmodel.viewmodel.virtualShift = NULL;
            InitPoint(&(appmodel.viewmodel.scrollPos));
            InitPoint(&(appmodel.viewmodel.curPlace));
            StorageStructBuilding(&appmodel);
        }
        else
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0L);
        }
        ReleaseDC(hwnd, hdc);
        break;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        MyTextOut(hdc, &appmodel, ps);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_KEYDOWN:
    {
        int curString = appmodel.viewmodel.curPlace.y;
        int maxLineInWin = appmodel.viewmodel.WindowHeight / appmodel.viewmodel.LetterHeight;
        int numOfString = appmodel.viewmodel.withFormating ? appmodel.viewmodel.numOfVirtualStr : appmodel.stormodel.numOfString;
        int curSymb = appmodel.viewmodel.curPlace.x;
        int maxSymbInLine = appmodel.viewmodel.WindowWidth / appmodel.viewmodel.LetterWidth;
        switch(wParam)
        {
        case VK_PRIOR: //PAGE UP
            if (curString > maxLineInWin)
            {
                appmodel.viewmodel.curPlace.y -= maxLineInWin;
                appmodel.viewmodel.scrollPos.y -= ((int)appmodel.viewmodel.scrollRatio.y * maxLineInWin);
            }
            else
            {
                appmodel.viewmodel.curPlace.y = 0;
                appmodel.viewmodel.scrollPos.y = 0;
            }
            InvalidateRect(hwnd, 0, TRUE);
            break;
        case VK_NEXT: //PAGE DOWN
            if ((curString + 2 * maxLineInWin) < numOfString)
            {
                appmodel.viewmodel.curPlace.y += maxLineInWin;
                appmodel.viewmodel.scrollPos.y += ((int)appmodel.viewmodel.scrollRatio.y * maxLineInWin);
            }
            else
            {
                appmodel.viewmodel.curPlace.y = numOfString - maxLineInWin;
                appmodel.viewmodel.scrollPos.y = (int)appmodel.viewmodel.scrollRatio.y * numOfString - maxLineInWin;
            }
            InvalidateRect(hwnd, 0, TRUE);
            break;
        case VK_DOWN:
            if (curString + maxLineInWin < numOfString)
            {
                appmodel.viewmodel.curPlace.y++;
                appmodel.viewmodel.scrollPos.y += (int)appmodel.viewmodel.scrollRatio.y;
                ScrollWindow(hwnd, 0, -appmodel.viewmodel.LetterHeight, NULL, NULL);
            }
            break;
        case VK_UP:
            if (curString)
            {
                appmodel.viewmodel.curPlace.y--;
                appmodel.viewmodel.scrollPos.y -= (int)appmodel.viewmodel.scrollRatio.y;
                ScrollWindow(hwnd, 0, appmodel.viewmodel.LetterHeight, NULL, NULL);
            }
            break;
        case VK_RIGHT:
            if (curSymb + maxSymbInLine < appmodel.stormodel.maxStringLength)
            {
                appmodel.viewmodel.curPlace.x++;
                appmodel.viewmodel.scrollPos.x += (int)appmodel.viewmodel.scrollRatio.x;
                InvalidateRect(hwnd, 0, TRUE);
            }
            break;
        case VK_LEFT:
            if (curSymb > 0)
            {
                appmodel.viewmodel.curPlace.x--;
                appmodel.viewmodel.scrollPos.x -= (int)appmodel.viewmodel.scrollRatio.x;
                InvalidateRect(hwnd, 0, TRUE);
            }
            break;
        }
        SetScrollPos(hwnd, SB_VERT, appmodel.viewmodel.scrollPos.y, TRUE);
        UpdateWindow(hwnd);
        break;

    }
    case WM_VSCROLL:
    {
        int curString = appmodel.viewmodel.curPlace.y;
        int maxLineInWin = appmodel.viewmodel.WindowHeight / appmodel.viewmodel.LetterHeight;
        int numOfString = appmodel.viewmodel.withFormating ? appmodel.viewmodel.numOfVirtualStr : appmodel.stormodel.numOfString;
        switch ((int)LOWORD(wParam))
        {
        case SB_LINEUP:
            if (curString)
            {
                appmodel.viewmodel.curPlace.y--;
                appmodel.viewmodel.scrollPos.y -= (int)appmodel.viewmodel.scrollRatio.y;
                ScrollWindow(hwnd, 0, appmodel.viewmodel.LetterHeight, NULL, NULL);
            }
            break;
        case SB_PAGEUP:
            if (curString > maxLineInWin)
            {
                appmodel.viewmodel.curPlace.y -= maxLineInWin;
                appmodel.viewmodel.scrollPos.y -= ((int)appmodel.viewmodel.scrollRatio.y * maxLineInWin);
            }
            else
            {
                appmodel.viewmodel.curPlace.y = 0;
                appmodel.viewmodel.scrollPos.y = 0;
            }
            InvalidateRect(hwnd, 0, TRUE);
            break;
        case SB_THUMBPOSITION:
            appmodel.viewmodel.scrollPos.y = HIWORD(wParam);
            appmodel.viewmodel.curPlace.y = appmodel.viewmodel.scrollPos.y * appmodel.viewmodel.scrollRatio.y;
            if (appmodel.viewmodel.curPlace.y == appmodel.stormodel.numOfString)
            {
                appmodel.viewmodel.curPlace.y -= maxLineInWin;
            }
            InvalidateRect(hwnd, 0, TRUE);
            break;
        case SB_LINEDOWN:
            if (curString + maxLineInWin < numOfString)
            {
                appmodel.viewmodel.curPlace.y++;
                appmodel.viewmodel.scrollPos.y += (int)appmodel.viewmodel.scrollRatio.y;
                ScrollWindow(hwnd, 0, -appmodel.viewmodel.LetterHeight, NULL, NULL);
            }
            break;
        case SB_PAGEDOWN:
            if ((curString + 2 * maxLineInWin) < numOfString)
            {
                appmodel.viewmodel.curPlace.y += maxLineInWin;
                appmodel.viewmodel.scrollPos.y += ((int)appmodel.viewmodel.scrollRatio.y * maxLineInWin);
            }
            else
            {
                appmodel.viewmodel.curPlace.y = numOfString - maxLineInWin;
                appmodel.viewmodel.scrollPos.y = (int)appmodel.viewmodel.scrollRatio.y * (numOfString - maxLineInWin);
            }
            InvalidateRect(hwnd, 0, TRUE);
            break;
        }
        SetScrollPos(hwnd, SB_VERT, appmodel.viewmodel.scrollPos.y, TRUE);
        SetScrollPos(hwnd, SB_HORZ, appmodel.viewmodel.scrollPos.x, TRUE);
        UpdateWindow(hwnd);
        break;
    }

    case WM_HSCROLL:
    {
        int curSymb = appmodel.viewmodel.curPlace.x;
        int maxSymbInLine = appmodel.viewmodel.WindowWidth / appmodel.viewmodel.LetterWidth;
        switch ((int)LOWORD(wParam))
        {
        case SB_LINERIGHT:
            if (curSymb + maxSymbInLine < appmodel.stormodel.maxStringLength)
            {
                appmodel.viewmodel.curPlace.x++;
                appmodel.viewmodel.scrollPos.x += (int)appmodel.viewmodel.scrollRatio.x;
            }
            break;
        case SB_LINELEFT:
            if (curSymb > 0)
            {
                appmodel.viewmodel.curPlace.x--;
                appmodel.viewmodel.scrollPos.x -= (int)appmodel.viewmodel.scrollRatio.x;
            }
            break;
        case SB_PAGELEFT:
            if (curSymb > maxSymbInLine)
            {
                appmodel.viewmodel.curPlace.x -= maxSymbInLine;
                appmodel.viewmodel.scrollPos.x -= (int)appmodel.viewmodel.scrollRatio.x * maxSymbInLine;
            }
            else
            {
                appmodel.viewmodel.curPlace.x = 0;
                appmodel.viewmodel.scrollPos.x = 0;
            }
            break;
        case SB_PAGERIGHT:
            if ((curSymb + 2 * maxSymbInLine) < appmodel.stormodel.maxStringLength)
            {
                appmodel.viewmodel.curPlace.x += maxSymbInLine;
                appmodel.viewmodel.scrollPos.x += (int)appmodel.viewmodel.scrollRatio.x * maxSymbInLine;
            }
            else
            {
                appmodel.viewmodel.curPlace.x = appmodel.stormodel.maxStringLength - maxSymbInLine;
                appmodel.viewmodel.scrollPos.x = (int)appmodel.viewmodel.scrollRatio.x * (appmodel.stormodel.maxStringLength - maxSymbInLine);
            }
            break;
        case SB_THUMBPOSITION:
            appmodel.viewmodel.scrollPos.x = HIWORD(wParam);
            appmodel.viewmodel.curPlace.x = appmodel.viewmodel.scrollPos.x * appmodel.viewmodel.scrollRatio.x;
            if (appmodel.viewmodel.curPlace.x == appmodel.stormodel.maxStringLength)
            {
                appmodel.viewmodel.curPlace.x -= maxSymbInLine;
            }
            break;
        }
        SetScrollPos(hwnd, SB_HORZ, appmodel.viewmodel.scrollPos.x, TRUE);
        InvalidateRect(hwnd, 0, TRUE);
        break;
    }

    case WM_SIZE:
    {
        MoveWindow(hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        RebuildViewModel(hwnd, &appmodel, rect);
        if (appmodel.stormodel.buff != NULL){
            if (appmodel.viewmodel.withFormating)
            {
                int curAbsolutePosition = appmodel.viewmodel.virtualShift[appmodel.viewmodel.curPlace.y];
                free(appmodel.viewmodel.virtualShift);
                CreateVirtualStrings(&appmodel);
                FindNewPos(&appmodel, curAbsolutePosition);
            }
            InitScroll(hwnd, &appmodel);

        }
        InvalidateRect(hwnd, 0, TRUE);
        break;
    }
    case WM_DESTROY:
        free(appmodel.stormodel.buff);
        free(appmodel.stormodel.shift);
        if(appmodel.viewmodel.virtualShift != NULL){
           free(appmodel.viewmodel.virtualShift);
        }
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
