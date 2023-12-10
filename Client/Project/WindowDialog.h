#pragma once
#include "resource1.h"

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern map<string, HWND> handleDictionary;