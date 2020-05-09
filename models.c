#include "models.h"
#include <math.h>

/**
Set zero value for point structure

PARAM[IN] p - point on point structure

NO RETURN VALUE
**/
void InitPoint(POINT* p){
    p->x = 0;
    p->y = 0;
    return;
}

/**
Reading chosen file

PARAM[IN] hwnd - context device
PARAM[IN] filename - name of new file
PARAM[IN] appmodel - pointer on model of application

RETURN - TRUE if successfully, FALSE if otherwise
**/

bool FileReading(HWND hwnd, char* filename, AppModel* appmodel)
{
    FILE* f = fopen(filename, "rb");
    if (f == NULL)
    {
        MessageBox(hwnd, "Could not open the file!", "Error!", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    fseek(f, 0, SEEK_END);
    appmodel->stormodel.fileSize = ftell(f);
    rewind(f);
    char* checkAlloc;
    checkAlloc = (char*)realloc(appmodel->stormodel.buff, sizeof(char) * (appmodel->stormodel.fileSize + 2));
    if (checkAlloc!= NULL)
    {
        appmodel->stormodel.buff = checkAlloc;
        int writtenSymb = fread(appmodel->stormodel.buff, sizeof(char), appmodel->stormodel.fileSize, f);
        if (writtenSymb != (unsigned)appmodel->stormodel.fileSize)
        {
            MessageBox(hwnd, "Can not read the file!", "Error!", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        appmodel->stormodel.buff[appmodel->stormodel.fileSize + 1] = 0;
    }
    else
    {
        MessageBox(hwnd, "Not enough memory!", "Error!", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    fclose(f);
    return TRUE;
}

/**
Creating storage structure

PARAM[IN] appmodel - pointer on model of application

RETURN - TRUE if successfully, FALSE if otherwise
**/
bool StorageStructBuilding(AppModel* appmodel)
{
    int maxLength = 0;
    appmodel->stormodel.numOfString = 1;
    for (int i = 0; i < appmodel->stormodel.fileSize; i++)
    {
        if (appmodel->stormodel.buff[i] == '\n')
        {
            appmodel->stormodel.numOfString++;
        }
    }
    appmodel->stormodel.shift = (int*)calloc(sizeof(int), appmodel->stormodel.numOfString + 2);
    int j = 1;
    appmodel->stormodel.shift[0] = 0; // Сдвиг первой строки относительно начала буфера - 0
    for (int i = 0; j < appmodel->stormodel.numOfString; i++)
    {
        if (appmodel->stormodel.buff[i] == '\n')
        {
            maxLength = i - 1 - appmodel->stormodel.shift[j - 1]; // Вычисление длины текущей строки, -1 из-за '\r'
            if (maxLength > appmodel->stormodel.maxStringLength)
            {
                appmodel->stormodel.maxStringLength = maxLength; // поиск самой длинной строки
            }
            appmodel->stormodel.shift[j++] = i + 1;
        }
    }
    appmodel->stormodel.shift[appmodel->stormodel.numOfString] = appmodel->stormodel.fileSize;
    return TRUE;
}

/**
Changing view model

PARAM[IN] hwnd - context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] rect - structure for window rectangle sizes

NO RETURN VALUE
**/
void RebuildViewModel(HWND hwnd, AppModel* appmodel, RECT rect)
{
    GetClientRect(hwnd, &rect);
    appmodel->viewmodel.WindowWidth = rect.right;
    appmodel->viewmodel.WindowHeight = rect.bottom;
    return;
}

/**
Creating virtual strings

PARAM[IN] appmodel - pointer on model of application

NO RETURN VALUE
**/
void CreateVirtualStrings(AppModel* appmodel)
{
    int maxSymbInLine = appmodel->viewmodel.WindowWidth / appmodel->viewmodel.LetterWidth;
    int maxDividing = ceil((float)appmodel->stormodel.maxStringLength / (float)maxSymbInLine);
    int maxStrings = maxDividing * appmodel->stormodel.numOfString;
    int numOfVirtualStr = 0;
    appmodel->viewmodel.virtualShift = (int*)calloc(sizeof(int), maxStrings + 2);
    appmodel->viewmodel.virtualShift[0] = 0; // Сдвиг первой строки относительно начала буфера - 0
    int curLineLength;
    for (int i = 0; i < appmodel->stormodel.numOfString; i++)
    {
        curLineLength = appmodel->stormodel.shift[i + 1] - appmodel->stormodel.shift[i] - 2;
        int required = ceil((float)curLineLength / (float)maxSymbInLine);
        for (int j = 1; j < required; j++)
        {
            appmodel->viewmodel.virtualShift[j + numOfVirtualStr] = appmodel->stormodel.shift[i] + maxSymbInLine * j;
        }
        numOfVirtualStr += required;
        if (curLineLength == 0) {
            numOfVirtualStr++;
        }
        appmodel->viewmodel.virtualShift[numOfVirtualStr] = appmodel->stormodel.shift[i] + curLineLength + 2;
    }
    appmodel->viewmodel.numOfVirtualStr = numOfVirtualStr;
}

/**
Changing view model

PARAM[IN] appmodel - pointer on model of application
PARAM[IN] oldPos - absolute position in Reader buffer

NO RETURN VALUE
**/
void FindNewPos(AppModel* appmodel, int oldPos)
{
    if (appmodel->viewmodel.withFormating){
       for (int i = 0; i < appmodel->viewmodel.numOfVirtualStr; i++)
        {
            if (oldPos >= appmodel->viewmodel.virtualShift[i] && oldPos < appmodel->viewmodel.virtualShift[i + 1])
            {
                appmodel->viewmodel.curPlace.y = i;
                break;
                //appmodel->viewmodel.scrollPos.y = i * appmodel->viewmodel.scrollRatio.y;
            }
        }
    }
    else{
       for (int i = 0; i < appmodel->stormodel.numOfString; i++)
        {
            if (oldPos >= appmodel->stormodel.shift[i] && oldPos < appmodel->stormodel.shift[i + 1])
            {
                appmodel->viewmodel.curPlace.y = i;
                //appmodel->viewmodel.scrollPos.y = i * appmodel->viewmodel.scrollRatio.y;
                break;
            }
        }
    }

}
