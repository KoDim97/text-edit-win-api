#ifndef MODELS_H_INCLUDED
#define MODELS_H_INCLUDED

#include <windows.h>
#include <stdio.h>
/**
Definition of boolean type
**/
typedef enum { false, true } bool;


/**
Structure for holding current placement in documents

curString - current string in file
curSymb - current symbol in string

**/
struct Place{
    int curString;
    int curSymb;
}Place;

/**
Structure for storage model in application

buff - array of text
fileSize - number of symbols in file
maxStringLength - most length string in file
numOfString - number of strings in file
shift - array of string's offset

**/
struct StorageModel {
	char* buff;
	int fileSize;
	int maxStringLength;
	int numOfString;
	int* shift;
}StorageModel;

/**
Structure for view model in application

WindowWidth - window width in px
WindowHeight - window height in px
LetterWidth - letter width in px
LetterHeight - letter height in px
withFormating - flag for formating mode
numOfVirtualStr - number of wrapped strings
virtualShift - array of wrapped string's offsets
curPlace - current position in text
scrollPos - current scroll position structure
scrollRatio - scroll ration structure

**/
struct ViewModel {
	int WindowWidth;
	int WindowHeight;
	int LetterWidth;
	int LetterHeight;
	bool withFormating;
	int numOfVirtualStr;
	int* virtualShift;
	POINT curPlace;
	POINT scrollPos;
	POINTFLOAT scrollRatio;
};

/**
Addition structure, which combine view and storage model

stormodel - pointer of storage model structure
viewmodel - pointer of view model structure

**/
typedef struct AppModel {
	struct StorageModel stormodel;
	struct ViewModel viewmodel;
}AppModel;

/**
Set zero value for point structure

PARAM[IN] p - point on point structure

NO RETURN VALUE
**/

void InitPoint(POINT* p);

/**
Reading chosen file

PARAM[IN] hwnd - context device
PARAM[IN] filename - name of new file
PARAM[IN] appmodel - pointer on model of application

RETURN - TRUE if successfully, FALSE if otherwise
**/

bool FileReading(HWND hwnd, char* filename, AppModel* appmodel);

/**
Creating storage structure

PARAM[IN] appmodel - pointer on model of application

RETURN - TRUE if successfully, FALSE if otherwise
**/

bool StorageStructBuilding(AppModel* appmodel);

/**
Changing view model

PARAM[IN] hwnd - context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] rect - structure for window rectangle sizes

NO RETURN VALUE
**/

void RebuildViewModel(HWND hwnd, AppModel *appmodel, RECT rect);

/**
Changing view model

PARAM[IN] appmodel - pointer on model of application
PARAM[IN] oldPos - absolute position in Reader buffer

NO RETURN VALUE
**/

void FindNewPos(AppModel* appmodel, int oldPos);

/**
Creating virtual strings

PARAM[IN] appmodel - pointer on model of application

NO RETURN VALUE
**/

void CreateVirtualStrings(AppModel* appmodel);

#endif // MODELS_H_INCLUDED
