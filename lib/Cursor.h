#pragma once

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class Cursor
{
public:
	static void CreateMaskBitmaps (HDC hdc, HBITMAP bmp, COLORREF clear, HBITMAP & andbm, HBITMAP & xorbm, int size)
	{
		HDC anddc = CreateCompatibleDC (hdc);
		HDC xordc = CreateCompatibleDC (hdc);

		COLORREF white = RGB(255,255,255);
		COLORREF black = RGB(0,0,0);

		andbm = CreateCompatibleBitmap (hdc, size, size);
		xorbm = CreateCompatibleBitmap (hdc, size, size);

		HGDIOBJ oldand = SelectObject (anddc, andbm);
		HGDIOBJ oldxor = SelectObject (xordc, xorbm);

		COLORREF color = 0;
		for (int x=0; x < size; x++)
		{
			for (int y=0; y < size; y++)
			{
				color = GetPixel (hdc, x, y);
				if (color == clear)
				{
					SetPixel (anddc, x, y, white);
					SetPixel (xordc, x, y, black);
				}
				else
				{
					SetPixel (anddc, x, y, black);
					SetPixel (xordc, x, y, color);
				}
			}
		}

		SelectObject (xordc, oldxor);
		SelectObject (anddc, oldand);

		DeleteDC (xordc);
		DeleteDC (anddc);
	}

	static HCURSOR CreateCursorFromBitmap (HDC hdc, HBITMAP bmp, COLORREF clear, int size)
	{
		HBITMAP andbm, xorbm;
		CreateMaskBitmaps (hdc, bmp, clear, andbm, xorbm, size);

		ICONINFO ii;
		ii.fIcon = FALSE;
		ii.xHotspot = size >> 1;
		ii.yHotspot = size >> 1;
		ii.hbmMask = andbm;
		ii.hbmColor = xorbm;

		HCURSOR cursor = (HCURSOR) CreateIconIndirect (&ii);

		DeleteObject (xorbm);
		DeleteObject (andbm);

		return cursor;
	}

};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
