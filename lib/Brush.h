#pragma once
#include "Defines.h"

BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gdi)

class Brush { public:
	HBRUSH handle;
	Brush () : handle (0) {}
	Brush (COLORREF color) { handle = CreateSolidBrush (color); }
	Brush (int r, int g, int b) { handle = CreateSolidBrush (RGB(r,g,b)); }
	~Brush () { if (handle) DeleteObject (handle); }
	operator HBRUSH () { return handle; }
	bool Free () { return handle ? DeleteObject (handle), handle = 0, true : false; }
	bool SetColor (COLORREF color)
	{
		return Free (), handle = CreateSolidBrush (color), 0 != handle;
	}
	bool Select (HDC hdc, HGDIOBJ * old = 0) const
	{
		if (!handle) return old ? *old = 0, false : false;
		HGDIOBJ temp = SelectObject (hdc, handle);
		if (old) *old = temp;
		return true;
	}
};

END_NAMESPACE (gdi)
END_NAMESPACE (lib)
