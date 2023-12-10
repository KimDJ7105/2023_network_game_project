#include "stdafx.h"
#include "WindowDialog.h"

map<string, HWND> handleDictionary;

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		handleDictionary.emplace("Exit Button", GetDlgItem(hDlg, IDC_BUTTON1));
		handleDictionary.emplace("Stay Button", GetDlgItem(hDlg, IDC_BUTTON2));
		handleDictionary.emplace("Text", GetDlgItem(hDlg, IDC_STATIC1));
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON1:
			EndDialog(hDlg, IDCANCEL);
			Define::GameRunnig = false;
			return TRUE;
		case IDC_BUTTON2:
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}