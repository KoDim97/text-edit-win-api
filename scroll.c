#include "scroll.h"
/**
Scroll range and position Initialization

PARAM[IN] hwnd - context device
PARAM[IN] appmodel - pointer on model of application

NO RETURN VALUE
**/
void InitScroll(HWND hwnd, AppModel* appmodel)
{
    InitScrollRatio(appmodel);
    int maxSymbInLine = appmodel->viewmodel.WindowWidth / appmodel->viewmodel.LetterWidth;
    int maxLineInWin = appmodel->viewmodel.WindowHeight / appmodel->viewmodel.LetterHeight;
    int numOfString = appmodel->viewmodel.withFormating ? appmodel->viewmodel.numOfVirtualStr : appmodel->stormodel.numOfString;
    appmodel->viewmodel.scrollPos.y = (int)appmodel->viewmodel.scrollRatio.y * appmodel->viewmodel.curPlace.y;
    int curString = appmodel->viewmodel.curPlace.y;
    int curSymb = appmodel->viewmodel.curPlace.x;

    if (numOfString - maxLineInWin - curString < 0)
    {
        EnableScrollBar(hwnd, SB_VERT, ESB_DISABLE_BOTH);
        if (appmodel->viewmodel.scrollPos.y - (int)appmodel->viewmodel.scrollRatio.y >= 0)
        {
            appmodel->viewmodel.curPlace.y--;
            appmodel->viewmodel.scrollPos.y -= (int)appmodel->viewmodel.scrollRatio.y;
        }
    }
    else
    {
        if (numOfString - maxLineInWin == 0){
            EnableScrollBar(hwnd, SB_VERT, ESB_DISABLE_BOTH);
        }
        else{
            EnableScrollBar(hwnd, SB_VERT, ESB_ENABLE_BOTH);
        }
    }
    if (appmodel->stormodel.maxStringLength - maxSymbInLine - curSymb < 0 || appmodel->viewmodel.withFormating)
    {
        EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
        if (appmodel->viewmodel.scrollPos.x - (int)appmodel->viewmodel.scrollRatio.x >= 0)
        {
            appmodel->viewmodel.curPlace.x --;
            appmodel->viewmodel.scrollPos.x -= (int)appmodel->viewmodel.scrollRatio.x;
        }
    }
    else
    {
        if (appmodel->stormodel.maxStringLength - maxSymbInLine == 0){
            EnableScrollBar(hwnd, SB_HORZ, ESB_DISABLE_BOTH);
        }
        else{
            EnableScrollBar(hwnd, SB_HORZ, ESB_ENABLE_BOTH);
        }

    }
    SetScrollRange(hwnd, SB_VERT, 0, min(numOfString - maxLineInWin, VSCROLL_MAX), TRUE);
    SetScrollRange(hwnd, SB_HORZ, 0, min(appmodel->stormodel.maxStringLength - maxSymbInLine, VSCROLL_MAX), TRUE);
    SetScrollPos(hwnd, SB_VERT, appmodel->viewmodel.scrollPos.y, TRUE);
    SetScrollPos(hwnd, SB_HORZ, appmodel->viewmodel.scrollPos.x, TRUE);
    return;
}

/**
Scroll ratio initialization

PARAM[IN] appmodel - pointer on model of application

NO RETURN VALUE
**/

void InitScrollRatio(AppModel* appmodel)
{
    int maxLineInWin = appmodel->viewmodel.WindowHeight / appmodel->viewmodel.LetterHeight;
    int maxSymbInLine = appmodel->viewmodel.WindowWidth / appmodel->viewmodel.LetterWidth;
    int numOfString = appmodel->viewmodel.withFormating ? appmodel->viewmodel.numOfVirtualStr : appmodel->stormodel.numOfString;
    appmodel->viewmodel.scrollRatio.y = max(0, min(numOfString - maxLineInWin, VSCROLL_MAX)) < VSCROLL_MAX ? 1 :
                                        (float)(numOfString - maxLineInWin) / (float)VSCROLL_MAX;

    appmodel->viewmodel.scrollRatio.x = max(0, min(appmodel->stormodel.maxStringLength - maxSymbInLine, VSCROLL_MAX)) < VSCROLL_MAX ? 1 :
                                        (float)(appmodel->stormodel.maxStringLength - maxSymbInLine) / (float)VSCROLL_MAX;
    return;
}
