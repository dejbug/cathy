#pragma once

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gdi)

class Surface
{
	HWND parent;
	HDC mdc;
	HBITMAP bmp;
	int cx, cy;

public:
	Surface () : parent(0), mdc(0), bmp(0), cx(400), cy(300)
	{
	}

	operator HDC () const { return mdc; }
	operator HBITMAP () const { return bmp; }
	
	HWND GetParent () const { return parent; }
	HDC GetHandle () const { return mdc; }
	HBITMAP GetBitmap () const { return bmp; }
	int GetWidth () const { return cx; }
	int GetHeight () const { return cy; }

public:
	bool Ready () { return !!mdc && !!bmp; }

	void Free ()
	{
		if (bmp) bmp = (DeleteObject (bmp), (HBITMAP)0);
		if (mdc) mdc = (DeleteDC (mdc), (HDC)0);
		parent = 0;
	}

	bool Resize (int cx, int cy)
	{
		this->cx = cx;
		this->cy = cy;
		return Create (parent);
	}

	bool Create (HWND handle)
	{
		Free ();
		parent = handle;
		HDC pdc = GetDC (handle);
		mdc = CreateCompatibleDC (pdc);
		bmp = CreateCompatibleBitmap (pdc, cx, cy);
		ReleaseDC (parent, pdc);
		SelectObject (mdc, bmp);		
		return true;
	}

	bool Flip ()
	{
		if (!Ready ()) return false;
		HDC pdc = GetDC (parent);
		bool result = TRUE == BitBlt (pdc, 0,0,cx,cy, mdc, 0,0, SRCCOPY);
		ReleaseDC (parent, pdc);
		return result;
	}

	bool Clear ()
	{
		if (!Ready ()) return false;
		SelectObject (mdc, GetStockObject (WHITE_BRUSH));
		SelectObject (mdc, GetStockObject (WHITE_PEN));
		return TRUE == Rectangle (mdc, 0, 0, cx, cy);
	}

	bool Clear (COLORREF color)
	{
		if (!Ready ()) return false;
		HBRUSH brush = CreateSolidBrush (color);
		HPEN pen = CreatePen (PS_SOLID, 1, color);
		SelectObject (mdc, brush);
		SelectObject (mdc, pen);
		bool result = TRUE == Rectangle (mdc, 0, 0, cx, cy);
		DeleteObject (pen);
		DeleteObject (brush);
		return result;
	}

public:
	HGDIOBJ Select (HGDIOBJ object)
	{
		return Ready () ? SelectObject (mdc, object) : 0;
	}

	HGDIOBJ Select (int index)
	{
		return Ready () ? SelectObject (mdc, GetStockObject (index)) : 0;
	}
};

END_NAMESPACE (gdi)
END_NAMESPACE (lib)
