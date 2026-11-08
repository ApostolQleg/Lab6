#pragma once

#include "Resource.h"

#include "../SharedData.h"

// Константи буферів форматування тексту
#define BUF_HEADER_CHARS         128  // Довжина тексту заголовка вікна
#define BUF_POINT_CHARS          64   // Довжина рядка однієї пари координат у UI
#define BYTES_PER_POINT_LINE     32   // Запас байтів на один рядок "x\ty\r\n" для буфера обміну

// Константи геометрії та візуалізації списку
#define UI_HEADER_X              20   // Відступ заголовка зліва
#define UI_HEADER_Y              15   // Відступ заголовка зверху
#define UI_LIST_START_X          20   // Початкова координата X для списку точок
#define UI_LIST_START_Y          45   // Початкова координата Y для списку точок
#define UI_LIST_LINE_HEIGHT      20   // Висота одного рядка точок
#define UI_LIST_COL_WIDTH        140  // Ширина одного стовпчика
#define UI_MARGIN_BOTTOM         25   // Нижній відступ від краю вікна
#define UI_WAIT_MSG_X            20   // Відступ повідомлення очікування
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