#include "pch.h"
#include "framework.h"
#include "set.h"

HWND hWndDlg = NULL;
HWND hWndOther = NULL;

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void Populate(HWND hDlg);
bool ReadAndValidate(HWND hDlg, Object1& out);

Object1* Object1::p_instance = nullptr;

Object1* Object1::getInstance()
{
	if (!p_instance)
	{
		p_instance = new Object1();
	}
	return p_instance;
}

void Object1::Open(HWND hWnd)
{
	if (hWndDlg)
	{
		PostMessage(hWndDlg, WM_SYSCOMMAND, SC_CLOSE, 0);
		hWndDlg = NULL;
		return;
	}

	hWndDlg = CreateDialog((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);

	if (hWndDlg)
	{
		ShowWindow(hWndDlg, SW_SHOW);
		UpdateWindow(hWndDlg);
	}
}

void Object1::Close()
{
	if (hWndDlg)
	{
		DestroyWindow(hWndDlg);
		hWndDlg = NULL;
		return;
	}
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		hWndDlg = hDlg;
		Populate(hDlg);
		return (INT_PTR)TRUE;

	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
		{
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			Object1* obj = Object1::getInstance();

			if (ReadAndValidate(hDlg, *obj))
			{
				HWND hWndMain = GetParent(hDlg);

				PointParams params;
				params.nPoint = obj->nPoint;
				params.xMin = obj->xMin;
				params.xMax = obj->xMax;
				params.yMin = obj->yMin;
				params.yMax = obj->yMax;

				COPYDATASTRUCT cds;
				cds.dwData = CD_POINT_PARAMS;
				cds.cbData = sizeof(PointParams);
				cds.lpData = &params;

				hWndOther = FindWindowW(OBJECT2_CLASS, NULL);
				if (hWndOther)
				{
					SendMessage(hWndOther, WM_COPYDATA, (WPARAM)hWndMain, (LPARAM)&cds);
				}
				else
				{
					WinExec("Object2.exe", SW_SHOW);
				}
				DestroyWindow(hDlg);
				InvalidateRect(hWndMain, NULL, TRUE);
			}
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_DESTROY:
		InvalidateRect(GetParent(hDlg), NULL, TRUE);
		hWndDlg = NULL;
		return (INT_PTR)TRUE;
	}

	return (INT_PTR)FALSE;
}

void Populate(HWND hDlg) {
	Object1* obj = Object1::getInstance();

	SetDlgItemInt(hDlg, IDC_EDIT1, (UINT)obj->nPoint, FALSE);
	SetDlgItemInt(hDlg, IDC_EDIT2, (UINT)obj->xMin, TRUE);
	SetDlgItemInt(hDlg, IDC_EDIT3, (UINT)obj->xMax, TRUE);
	SetDlgItemInt(hDlg, IDC_EDIT4, (UINT)obj->yMin, TRUE);
	SetDlgItemInt(hDlg, IDC_EDIT5, (UINT)obj->yMax, TRUE);
};

static bool Fail(HWND hDlg, int ctrlId, const wchar_t* msg)
{
	MessageBox(hDlg, msg, L"Input Error", MB_OK | MB_ICONWARNING);

	HWND hCtrl = GetDlgItem(hDlg, ctrlId);
	SetFocus(hCtrl);

	SendMessage(hCtrl, EM_SETSEL, 0, -1); // виділити текст

	return false;
}

bool ReadAndValidate(HWND hDlg, Object1& out)
{
	BOOL ok;

	int nPoint = (int)GetDlgItemInt(hDlg, IDC_EDIT1, &ok, FALSE);
	if (!ok || nPoint < 2)
		return Fail(hDlg, IDC_EDIT1, L"Point count must be a number greater than 2");

	int xMin = (int)GetDlgItemInt(hDlg, IDC_EDIT2, &ok, TRUE);
	if (!ok) return Fail(hDlg, IDC_EDIT2, L"Invalid min x");

	int xMax = (int)GetDlgItemInt(hDlg, IDC_EDIT3, &ok, TRUE);
	if (!ok) return Fail(hDlg, IDC_EDIT3, L"Invalid max x");

	int yMin = (int)GetDlgItemInt(hDlg, IDC_EDIT4, &ok, TRUE);
	if (!ok) return Fail(hDlg, IDC_EDIT4, L"Invalid min y");

	int yMax = (int)GetDlgItemInt(hDlg, IDC_EDIT5, &ok, TRUE);
	if (!ok) return Fail(hDlg, IDC_EDIT5, L"Invalid max y");

	if (xMin >= xMax)
		return Fail(hDlg, IDC_EDIT3, L"Max x must be greater than min x");
	if (yMin >= yMax)
		return Fail(hDlg, IDC_EDIT5, L"Max y must be greater than min y");
	
	int xRange = xMax - xMin + 1;
	int yRange = yMax - yMin + 1;
	int possibleRange = xRange * yRange;

	if (possibleRange < nPoint)
		return Fail(hDlg, IDC_EDIT5, L"Point count should not be bigger than possible range");

	out.nPoint = nPoint;
	out.xMax = xMax;  out.xMin = xMin;
	out.yMax = yMax;  out.yMin = yMin;
	return true;
}