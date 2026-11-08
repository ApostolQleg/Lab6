#pragma once

#include "Resource.h"

class Object1
{
private:
	static Object1* p_instance;

	Object1() {};
	~Object1() {};

	Object1(const Object1&) = delete;
	Object1& operator=(const Object1&) = delete;
public:
	int nPoint = 0;

	int xMin = 0;
	int xMax = 0;

	int yMin = 0;
	int yMax = 0;

	static Object1* getInstance();

	void Open(HWND hWnd);
	void Close();
};