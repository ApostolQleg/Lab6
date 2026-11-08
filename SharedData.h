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

// Користувацьке повідомлення про оновлення даних у Clipboard
constexpr UINT WM_UPDATE_GRAPH = WM_USER + 1;

// Для зручного пошуку через FindWindow
constexpr const wchar_t* OBJECT2_CLASS = L"Object2Class";
constexpr const wchar_t* OBJECT2_TITLE = L"Object2";

constexpr const wchar_t* OBJECT3_CLASS = L"Object3Class";
constexpr const wchar_t* OBJECT3_TITLE = L"Object3";