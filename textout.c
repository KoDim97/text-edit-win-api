#include "textout.h"
#include "models.h"

/**
Text out in formating mode

PARAM[IN] hdc - descriptor of context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] ps - PAINTSTRUCT structure

RETURN - TRUE if successfully, FALSE if otherwise
**/
bool Formating(HDC hdc, AppModel* appmodel, PAINTSTRUCT ps)
{
    int maxLineInWin = appmodel->viewmodel.WindowHeight / appmodel->viewmodel.LetterHeight;
    int curString = appmodel->viewmodel.curPlace.y;
    int sumStrInFile = appmodel->viewmodel.numOfVirtualStr;
    int paintBegin = ps.rcPaint.top/appmodel->viewmodel.LetterHeight - 1;
    paintBegin = paintBegin < 0 ? 0: paintBegin;
    int paintEnd = min(ps.rcPaint.bottom/appmodel->viewmodel.LetterHeight, min(maxLineInWin, sumStrInFile - 1 - curString)) + 1;
    for (int i = paintBegin; i < (sumStrInFile - curString) && i < paintEnd; i++)
    {
        TextOut(hdc,
                0,
                i * appmodel->viewmodel.LetterHeight,
                appmodel->stormodel.buff + appmodel->viewmodel.virtualShift[curString + i],
                (appmodel->viewmodel.virtualShift[curString + i + 1] - appmodel->viewmodel.virtualShift[curString + i]));
    }
    return TRUE;
}

/**
Text out without formating mode

PARAM[IN] hdc - descriptor of context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] ps - PAINTSTRUCT structure

RETURN - TRUE if successfully, FALSE if otherwise
**/
bool WithoutFormating(HDC hdc, AppModel* appmodel, PAINTSTRUCT ps)
{
    int sumStrInFile = appmodel->stormodel.numOfString;
    int maxSymbInLine = appmodel->viewmodel.WindowWidth / appmodel->viewmodel.LetterWidth;
    int maxLineInWin = appmodel->viewmodel.WindowHeight / appmodel->viewmodel.LetterHeight;
    int curSymb = appmodel->viewmodel.curPlace.x;
    int curString = appmodel->viewmodel.curPlace.y;
    int paintBegin = ps.rcPaint.top/appmodel->viewmodel.LetterHeight;
    int paintEnd = min(ps.rcPaint.bottom/appmodel->viewmodel.LetterHeight, min(maxLineInWin, sumStrInFile - 1 - curString)) + 1;
    int curLineLength;
    for (int i = paintBegin; i < paintEnd; i++)
    {
        curLineLength = appmodel->stormodel.shift[curString + i + 1] - appmodel->stormodel.shift[curString + i];
        TextOut(hdc,
                0,
                i * appmodel->viewmodel.LetterHeight,
                appmodel->stormodel.buff + appmodel->stormodel.shift[curString + i] + curSymb,
                max(0, min(maxSymbInLine, curLineLength - curSymb)));
    }
    return TRUE;
}

/**
Text out function

PARAM[IN] hdc - descriptor of context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] ps - PAINTSTRUCT structure

RETURN - TRUE if successfully, FALSE if otherwise
**/

bool MyTextOut(HDC hdc, AppModel* appmodel, PAINTSTRUCT ps)
{
    if (appmodel->viewmodel.withFormating)
    {
        Formating(hdc, appmodel, ps);
    }
    else
    {
        WithoutFormating(hdc, appmodel, ps);
    }
    return TRUE;
}
