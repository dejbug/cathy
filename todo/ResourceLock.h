#pragma once
#include <memory>
#include <windows.h>

struct ResourceLock
{
	char const * data = nullptr;
	size_t size = 0;

	ResourceLock(WORD id, HINSTANCE hi = GetModuleHandle(nullptr));
	virtual ~ResourceLock();
};
