#include "pch.h"
#include "framework.h"
#include "generator.h"

#include <stdio.h>

Object2* Object2::p_instance = nullptr;

Object2::Object2()
	: points(nullptr), pointCount(0), hasData(FALSE)
{
	memset(&params, 0, sizeof(params));
	srand((unsigned int)GetTickCount64());
}

Object2::~Object2()
{
	if (points)
	{
		free(points);
		points = nullptr;
	}
}

Object2* Object2::getInstance()
{
	if (!p_instance)
	{
		p_instance = new Object2();
	}
	return p_instance;
}

void Object2::OnCreate(HWND hWnd)
{
	HWND hWndObj1 = FindWindowW(OBJECT1_CLASS, NULL);
	if (hWndObj1 && IsWindow(hWndObj1))
	{
		PostMessage(hWndObj1, WM_OBJECT2_READY, (WPARAM)hWnd, 0);
	}
}

void Object2::GeneratePoints()
{
	if (points)
	{
		free(points);
		points = nullptr;
	}

	pointCount = params.nPoint;
	if (pointCount <= 0) return;

	points = (POINT*)malloc(pointCount * sizeof(POINT));
	if (!points) return;

	int xRange = (params.xMax - params.xMin) + 1;
	int yRange = (params.yMax - params.yMin) + 1;

	if (xRange <= 0) xRange = 1;
	if (yRange <= 0) yRange = 1;

	for (int i = 0; i < pointCount; ++i)
	{
		points[i].x = params.xMin + (rand() % xRange);
		points[i].y = params.yMin + (rand() % yRange);
	}
}

int Object2::PutTextToClipboard(HWND hWnd, const char* src)
{
	HGLOBAL hglbCopy;
	BYTE* pTmp;
	long len;

	if (src == NULL) return 0;
	if (src[0] == 0) return 0;

	len = (long)strlen(src);
	hglbCopy = GlobalAlloc(GHND, len + 1);
	if (hglbCopy == NULL) return 0;

	pTmp = (BYTE*)GlobalLock(hglbCopy);
	if (pTmp == NULL)
	{
		GlobalFree(hglbCopy);
		return 0;
	}

	memcpy(pTmp, src, len + 1);
	GlobalUnlock(hglbCopy);

	if (!OpenClipboard(hWnd))
	{
		GlobalFree(hglbCopy);
		return 0;
	}

	EmptyClipboard();
	SetClipboardData(CF_TEXT, hglbCopy);
	CloseClipboard();

	return 1;
}

BOOL Object2::OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;
	if (!cds || cds->dwData != CD_POINT_PARAMS || cds->cbData < sizeof(PointParams))
	{
		return FALSE;
	}

	params = *(PointParams*)cds->lpData;
	hasData = TRUE;

	GeneratePoints();

	size_t bufSize = (size_t)pointCount * BYTES_PER_POINT_LINE + BYTES_PER_POINT_LINE;
	char* textBuffer = (char*)malloc(bufSize);
	if (textBuffer)
	{
		int offset = sprintf_s(textBuffer, bufSize, "x\ty\r\n");
		for (int i = 0; i < pointCount; ++i)
		{
			offset += sprintf_s(textBuffer + offset, bufSize - offset, "%ld\t%ld\r\n", points[i].x, points[i].y);
		}

		PutTextToClipboard(hWnd, textBuffer);
		free(textBuffer);
	}

	InvalidateRect(hWnd, NULL, TRUE);

	HWND hWndCaller = (HWND)wParam;
	if (hWndCaller && IsWindow(hWndCaller))
	{
		PostMessage(hWndCaller, WM_DATA_READY, 0, 0);
	}

	return TRUE;
}

void Object2::OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	if (!hasData || !points)
	{
		const wchar_t* msg = L"Waiting for Object1 coordinates...";
		TextOutW(hdc, UI_WAIT_MSG_X, UI_WAIT_MSG_Y, msg, lstrlenW(msg));
	}
	else
	{
		wchar_t szHeader[BUF_HEADER_CHARS];
		wsprintfW(szHeader, L"Points: %d     X: [%d; %d]     Y: [%d; %d]",
			params.nPoint, params.xMin, params.xMax, params.yMin, params.yMax);
		TextOutW(hdc, UI_HEADER_X, UI_HEADER_Y, szHeader, lstrlenW(szHeader));

		RECT rc;
		GetClientRect(hWnd, &rc);

		int availableHeight = (rc.bottom - rc.top) - UI_LIST_START_Y - UI_MARGIN_BOTTOM;
		int maxRowsPerCol = availableHeight / UI_LIST_LINE_HEIGHT;
		if (maxRowsPerCol < 1)
		{
			maxRowsPerCol = 1;
		}

		for (int i = 0; i < pointCount; ++i)
		{
			int col = i / maxRowsPerCol;
			int row = i % maxRowsPerCol;

			wchar_t szPoint[BUF_POINT_CHARS];
			wsprintfW(szPoint, L"%d) (%ld, %ld)", i + 1, points[i].x, points[i].y);
			TextOutW(hdc, UI_LIST_START_X + col * UI_LIST_COL_WIDTH,
				UI_LIST_START_Y + row * UI_LIST_LINE_HEIGHT,
				szPoint, lstrlenW(szPoint));
		}
	}

	EndPaint(hWnd, &ps);
}

void Object2::OnCommand(HWND hWnd, WPARAM wParam)
{
	int wmId = LOWORD(wParam);
	if (wmId == IDM_EXIT)
	{
		DestroyWindow(hWnd);
	}
}

void Object2::OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}