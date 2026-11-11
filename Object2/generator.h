#pragma once

#include "Resource.h"

#include "../SharedData.h"

#define BUF_HEADER_CHARS         128
#define BUF_POINT_CHARS          64
#define BYTES_PER_POINT_LINE     32

#define UI_HEADER_X              20
#define UI_HEADER_Y              15
#define UI_LIST_START_X          20
#define UI_LIST_START_Y          45
#define UI_LIST_LINE_HEIGHT      20
#define UI_LIST_COL_WIDTH        140
#define UI_MARGIN_BOTTOM         25
#define UI_WAIT_MSG_X            20
#define UI_WAIT_MSG_Y            20

class Object2
{
private:
	static Object2* p_instance;

	PointParams params;
	POINT* points;
	int pointCount;
	bool hasData;

	Object2();
	~Object2();

	Object2(const Object2&) = delete;
	Object2& operator=(const Object2&) = delete;

	void GeneratePoints();
	int PutTextToClipboard(HWND hWnd, const char* src);
public:
	static Object2* getInstance();

	void OnCreate(HWND hWnd);
	BOOL OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnPaint(HWND hWnd);
	void OnCommand(HWND hWnd, WPARAM wParam);
	void OnDestroy(HWND hWnd);
};