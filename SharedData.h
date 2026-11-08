#pragma once
#include <windows.h>

// Потрібні параметри згідно мого варіанту
struct PointParams {
    int nPoint;
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};

// Ідентифікатор типу даних для COPYDATASTRUCT::dwData
// Для того щоб переконатися, що отримані саме параметри точок
constexpr ULONG_PTR CD_POINT_PARAMS = 0x1001;

// Системні користувацькі повідомлення
constexpr UINT WM_UPDATE_GRAPH = WM_USER + 1; // Object1 -> Object3 (оновлення графіка)
constexpr UINT WM_DATA_READY = WM_USER + 2; // Object2 -> Object1 (точки в буфері обміну)
constexpr UINT WM_OBJECT2_READY = WM_USER + 3; // Object2 -> Object1 (вікно Object2 створено)

// Для зручного пошуку через FindWindow
constexpr const wchar_t* OBJECT1_CLASS = L"IDC_OBJECT1";
constexpr const wchar_t* OBJECT2_CLASS = L"IDC_OBJECT2";
constexpr const wchar_t* OBJECT3_CLASS = L"IDC_OBJECT3";