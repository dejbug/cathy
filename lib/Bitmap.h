#pragma once
#include "Defines.h"
#include "Common.h"

BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gdi)

typedef struct PALETTE {
  RGBQUAD * colors;
  UINT count;

  PALETTE() { colors = NULL; count = 0; }
  ~PALETTE() { if( colors ) delete colors; colors = 0; count = 0; }
} PALETTE, FAR* LPPALETTE;


class Bitmap
{
private:
	HDC pdc, mdc;
	HBITMAP bmp;
	UINT width, height;

public:
	Bitmap ();
	Bitmap (HBITMAP bmp);
	Bitmap (const Bitmap & that);
	~Bitmap ();

	operator HBITMAP () const { return bmp; }
	operator HDC () const { return mdc; }

public:
	bool Load (HDC hdc, LPCSTR filepath, COLORREF bg=RGB(255,255,255), COLORREF fg=RGB(0,0,0));
	bool Create (HDC hdc, UINT w, UINT h);
	void Delete ();

	inline bool Ready () const { return 0 != bmp && 0 != mdc; }

	void SetTargetDC (HDC hdc);
	//bool Clear (int x, int y, float rx, float ry, int sx=0, int sy=0, int scx=-1, int scy=-1);

	bool Paint (HDC hdc);
	bool Paint (int x, int y, int dx=0, int dy=0, int cy=-1);
	bool Paint (int x, int y, float rx, float ry, int sx=0, int sy=0, int scx=-1, int scy=-1);
	bool Paint (HDC hdc, const LPRECT r, bool stretch);
	bool Paint (HDC hdc, const LPRECT r, int x, int y);
	bool Paint (HDC hdc, const LPRECT dst, const LPRECT src);
	bool Paint (HDC hdc, const LPRECT dst, const LPRECT src, COLORREF transparent=0);
	bool Flip ();
};

END_NAMESPACE (gdi)
END_NAMESPACE (lib)
