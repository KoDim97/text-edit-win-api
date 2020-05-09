#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

/**
Open window to choose new file

PARAM[IN] hwnd - context device
PARAM[IN] sfile - name of new file

RETURN - TRUE if successfully, FALSE if otherwise
**/

BOOL ShowDialog(HWND hwnd, TCHAR* sfile);
#endif // MAIN_H_INCLUDED
