#ifndef SCROLL_H_INCLUDED
#define SCROLL_H_INCLUDED

#include "models.h"

/**
Macros for maximum scroll value
**/
#define VSCROLL_MAX 65534

/**
Scroll range and position Initialization

PARAM[IN] hwnd - context device
PARAM[IN] appmodel - pointer on model of application

NO RETURN VALUE
**/

void InitScroll(HWND hwnd, AppModel* appmodel);

/**
Scroll ratio initialization

PARAM[IN] appmodel - pointer on model of application

NO RETURN VALUE
**/

void InitScrollRatio(AppModel* appmodel);
#endif // SCROLL_H_INCLUDED
