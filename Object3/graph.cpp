#include "pch.h"
#include "framework.h"
#include "graph.h"

#include <stdio.h>

Object3* Object3::p_instance = nullptr;

Object3::Object3() :
	points(nullptr),
	pointCount(0),
	hasData(FALSE),
	minX(0), maxX(0),
	minY(0), maxY(0)
{
}

Object3::~Object3()
{
	if (points)
	{
		free(points);
		points = nullptr;
	}
}

Object3* Object3::getInstance()
{
	if (!p_instance)
	{
		p_instance = new Object3();
	}
	return p_instance;
}

static int ComparePointsX(const void* a, const void* b)
{
	const POINT* ptA = (const POINT*)a;
	const POINT* ptB = (const POINT*)b;
	if (ptA->x < ptB->x) return -1;
	if (ptA->x > ptB->x) return 1;
	return 0;
}

void Object3::SortPoints()
{
	if (points && pointCount > 1)
	{
		qsort(points, pointCount, sizeof(POINT), ComparePointsX);
	}
}

void Object3::CalculateBounds()
{
	if (!points || pointCount <= 0) return;

	minX = maxX = points[0].x;
	minY = maxY = points[0].y;

	for (int i = 1; i < pointCount; ++i)
	{
		if (points[i].x < minX) minX = points[i].x;
		if (points[i].x > maxX) maxX = points[i].x;
		if (points[i].y < minY) minY = points[i].y;
		if (points[i].y > maxY) maxY = points[i].y;
	}

	if (minX == maxX) maxX = minX + 1;
	if (minY == maxY) maxY = minY + 1;
}

BOOL Object3::ReadFromClipboard(HWND hWnd)
{
	if (!IsClipboardFormatAvailable(CF_TEXT)) return FALSE;
	if (!OpenClipboard(hWnd)) return FALSE;

	HGLOBAL hglb = GetClipboardData(CF_TEXT);
	if (!hglb)
	{
		CloseClipboard();
		return FALSE;
	}

	char* lptstr = (char*)GlobalLock(hglb);
	if (!lptstr)
	{
		CloseClipboard();
		return FALSE;
	}

	if (points)
	{
		free(points);
		points = nullptr;
		pointCount = 0;
	}

	int estimatedPoints = 0;
	const char* scan = lptstr;
	while (*scan)
	{
		if (*scan == '\n') estimatedPoints++;
		scan++;
	}

	if (estimatedPoints <= 0)
	{
		GlobalUnlock(hglb);
		CloseClipboard();
		return FALSE;
	}

	points = (POINT*)malloc(estimatedPoints * sizeof(POINT) + sizeof(POINT));
	if (!points)
	{
		GlobalUnlock(hglb);
		CloseClipboard();
		return FALSE;
	}

	const char* line = lptstr;
	pointCount = 0;

	while (line && *line)
	{
		long xVal = 0, yVal = 0;
		if (sscanf_s(line, "%ld\t%ld", &xVal, &yVal) == 2)
		{
			points[pointCount].x = xVal;
			points[pointCount].y = yVal;
			pointCount++;
		}

		line = strchr(line, '\n');
		if (line) line++;
	}

	GlobalUnlock(hglb);
	CloseClipboard();

	if (pointCount > 0)
	{
		SortPoints();
		CalculateBounds();
		hasData = TRUE;
		return TRUE;
	}

	return FALSE;
}

POINT Object3::MathToScreen(long x, long y, const RECT& plotRect)
{
	POINT pt;
	double scaleX = (double)(plotRect.right - plotRect.left) / (double)(maxX - minX);
	double scaleY = (double)(plotRect.bottom - plotRect.top) / (double)(maxY - minY);

	pt.x = plotRect.left + (LONG)((x - minX) * scaleX);
	pt.y = plotRect.bottom - (LONG)((y - minY) * scaleY);
	return pt;
}

void Object3::OnCreate(HWND hWnd)
{
	if (ReadFromClipboard(hWnd))
	{
		InvalidateRect(hWnd, NULL, TRUE);
	}
}

void Object3::OnUpdateGraph(HWND hWnd)
{
	if (ReadFromClipboard(hWnd))
	{
		InvalidateRect(hWnd, NULL, TRUE);
	}
}

void Object3::OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	if (!hasData || pointCount < 2)
	{
		const wchar_t* msg = L"Waiting for buffer update";
		TextOutW(hdc, 20, 20, msg, lstrlenW(msg));
		EndPaint(hWnd, &ps);
		return;
	}

	RECT rc;
	GetClientRect(hWnd, &rc);

	RECT plotRect;
	plotRect.left = rc.left + GRAPH_MARGIN_LEFT;
	plotRect.top = rc.top + GRAPH_MARGIN_TOP;
	plotRect.right = rc.right - GRAPH_MARGIN_RIGHT;
	plotRect.bottom = rc.bottom - GRAPH_MARGIN_BOTTOM;

	if (plotRect.right <= plotRect.left || plotRect.bottom <= plotRect.top)
	{
		EndPaint(hWnd, &ps);
		return;
	}

	HPEN hGridPen = CreatePen(PS_DOT, 1, COLOR_GRID);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hGridPen);

	for (int i = 0; i <= GRAPH_NUM_TICKS; ++i)
	{
		int x = plotRect.left + i * (plotRect.right - plotRect.left) / GRAPH_NUM_TICKS;
		int y = plotRect.top + i * (plotRect.bottom - plotRect.top) / GRAPH_NUM_TICKS;

		MoveToEx(hdc, x, plotRect.top, NULL);
		LineTo(hdc, x, plotRect.bottom);

		MoveToEx(hdc, plotRect.left, y, NULL);
		LineTo(hdc, plotRect.right, y);
	}

	HPEN hAxisPen = CreatePen(PS_SOLID, 2, COLOR_AXIS);
	SelectObject(hdc, hAxisPen);
	DeleteObject(hGridPen);

	MoveToEx(hdc, plotRect.left, plotRect.top, NULL);
	LineTo(hdc, plotRect.left, plotRect.bottom);
	LineTo(hdc, plotRect.right, plotRect.bottom);

	SetBkMode(hdc, TRANSPARENT);

	for (int i = 0; i <= GRAPH_NUM_TICKS; ++i)
	{
		int xPos = plotRect.left + i * (plotRect.right - plotRect.left) / GRAPH_NUM_TICKS;
		long mathX = minX + i * (maxX - minX) / GRAPH_NUM_TICKS;

		MoveToEx(hdc, xPos, plotRect.bottom, NULL);
		LineTo(hdc, xPos, plotRect.bottom + GRAPH_TICK_SIZE);

		wchar_t xText[LABEL_BUF_SIZE];
		wsprintfW(xText, L"%ld", mathX);
		TextOutW(hdc, xPos - 10, plotRect.bottom + 8, xText, lstrlenW(xText));

		int yPos = plotRect.bottom - i * (plotRect.bottom - plotRect.top) / GRAPH_NUM_TICKS;
		long mathY = minY + i * (maxY - minY) / GRAPH_NUM_TICKS;

		MoveToEx(hdc, plotRect.left - GRAPH_TICK_SIZE, yPos, NULL);
		LineTo(hdc, plotRect.left, yPos);

		wchar_t yText[LABEL_BUF_SIZE];
		wsprintfW(yText, L"%ld", mathY);
		TextOutW(hdc, plotRect.left - GRAPH_MARGIN_LEFT + 15, yPos - 8, yText, lstrlenW(yText));
	}

	HPEN hGraphPen = CreatePen(PS_SOLID, 2, COLOR_GRAPH_LINE);
	SelectObject(hdc, hGraphPen);

	POINT ptStart = MathToScreen(points[0].x, points[0].y, plotRect);
	MoveToEx(hdc, ptStart.x, ptStart.y, NULL);

	for (int i = 1; i < pointCount; ++i)
	{
		POINT pt = MathToScreen(points[i].x, points[i].y, plotRect);
		LineTo(hdc, pt.x, pt.y);
	}

	HBRUSH hPointBrush = CreateSolidBrush(COLOR_POINT_FILL);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPointBrush);

	for (int i = 0; i < pointCount; ++i)
	{
		POINT pt = MathToScreen(points[i].x, points[i].y, plotRect);
		Ellipse(hdc, pt.x - GRAPH_POINT_RADIUS, pt.y - GRAPH_POINT_RADIUS,
			pt.x + GRAPH_POINT_RADIUS, pt.y + GRAPH_POINT_RADIUS);
	}

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hAxisPen);
	DeleteObject(hGraphPen);
	DeleteObject(hPointBrush);

	EndPaint(hWnd, &ps);
}

void Object3::OnCommand(HWND hWnd, WPARAM wParam)
{
	int wmId = LOWORD(wParam);
	if (wmId == IDM_EXIT)
	{
		DestroyWindow(hWnd);
	}
}

void Object3::OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}