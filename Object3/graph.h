#pragma once

#include "Resource.h"

#include "../SharedData.h"

#define CLIPBOARD_BUF_SIZE       16384
#define LABEL_BUF_SIZE           32

#define GRAPH_MARGIN_LEFT        65
#define GRAPH_MARGIN_RIGHT       35
#define GRAPH_MARGIN_TOP         35
#define GRAPH_MARGIN_BOTTOM      45
#define GRAPH_TICK_SIZE          5
#define GRAPH_NUM_TICKS          5
#define GRAPH_POINT_RADIUS       3

#define COLOR_AXIS               RGB(60, 60, 60)
#define COLOR_GRID               RGB(220, 220, 220)
#define COLOR_GRAPH_LINE         RGB(20, 100, 220)
#define COLOR_POINT_FILL         RGB(220, 40, 40)

class Object3
{
private:
    static Object3* p_instance;

    POINT* points;
    int pointCount;
    BOOL hasData;

    long minX, maxX;
    long minY, maxY;

    Object3();
    ~Object3();

    Object3(const Object3&) = delete;
    Object3& operator=(const Object3&) = delete;

    BOOL ReadFromClipboard(HWND hWnd);
    void SortPoints();
    void CalculateBounds();

    POINT MathToScreen(long x, long y, const RECT& plotRect);
public:
    static Object3* getInstance();

    void OnCreate(HWND hWnd);
    void OnUpdateGraph(HWND hWnd);
    void OnPaint(HWND hWnd);
    void OnCommand(HWND hWnd, WPARAM wParam);
    void OnDestroy(HWND hWnd);
};