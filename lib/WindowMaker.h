#pragma once
#include "Common.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class WindowMaker : public CREATESTRUCT
{
public:
	bool Create (HINSTANCE instance, LPCSTR type, LPCSTR title, DWORD style, DWORD exstyle)
	{
		lpCreateParams = 0;
		hInstance = instance;
		hMenu = 0;
		hwndParent = 0;
		cx = CW_USEDEFAULT;
		cy = CW_USEDEFAULT;
		x = CW_USEDEFAULT;
		y = CW_USEDEFAULT;
		this->style = style;
		lpszName = title;
		lpszClass = type;
		dwExStyle = exstyle;
		return true;
	}

	bool Create (HWND parent, LPCSTR type, UINT id, DWORD style, DWORD exstyle)
	{
		lpCreateParams = 0;
		hInstance = (HINSTANCE) GetWindowLong (parent, GWL_HINSTANCE);
		hMenu = (HMENU) id;
		hwndParent = parent;
		cx = CW_USEDEFAULT;
		cy = CW_USEDEFAULT;
		x = CW_USEDEFAULT;
		y = CW_USEDEFAULT;
		this->style = WS_CHILD | style;
		lpszName = 0;
		lpszClass = type;
		dwExStyle = exstyle;
		return true;
	}

	bool Make (HWND & out)
	{
		out = CreateWindowEx (dwExStyle, lpszClass, lpszName, style,
			x, y, cx, cy, hwndParent, hMenu, hInstance, lpCreateParams);
		return 0 != out;
	}
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
