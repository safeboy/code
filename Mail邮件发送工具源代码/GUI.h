#ifndef GUI_H
#define GUI_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include "resource.h"
#include "SendEmail.h"

#define NDEBUG

LRESULT CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int RunDialog(HINSTANCE hInstance);


#endif // GUI_H
