#ifndef TEXTOUT_H_INCLUDED
#define TEXTOUT_H_INCLUDED


#include "models.h"

/**
Text out in formating mode

PARAM[IN] hdc - descriptor of context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] ps - PAINTSTRUCT structure

RETURN - TRUE if successfully, FALSE if otherwise
**/

bool Formating(HDC hdc, AppModel* appmodel, PAINTSTRUCT ps);

/**
Text out without formating mode

PARAM[IN] hdc - descriptor of context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] ps - PAINTSTRUCT structure

RETURN - TRUE if successfully, FALSE if otherwise
**/

bool WithoutFormating(HDC hdc, AppModel* appmodel, PAINTSTRUCT ps);

/**
Text out function

PARAM[IN] hdc - descriptor of context device
PARAM[IN] appmodel - pointer on model of application
PARAM[IN] ps - PAINTSTRUCT structure

RETURN - TRUE if successfully, FALSE if otherwise
**/


bool MyTextOut(HDC hdc, AppModel* appmodel, PAINTSTRUCT ps);

#endif // TEXTOUT_H_INCLUDED
