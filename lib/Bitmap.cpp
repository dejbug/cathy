#include "Bitmap.h"
// #pragma comment (lib,"msimg32.lib")

INCLUDE_NAMESPACE (lib::gdi)

bool error (bool code, const char * msg) { OutputDebugString (msg); return code; }

Bitmap::Bitmap ()
{
	bmp = 0;
	width = 0;
	height = 0;
}

Bitmap::Bitmap (HBITMAP bmp)
{
	this->bmp = bmp;
}

Bitmap::Bitmap (const Bitmap & that)
{
	this->bmp = that.bmp;
}

Bitmap::~Bitmap ()
{
}

typedef struct tagBITMAPINFOX {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[2];
} BITMAPINFOX;

void RGBtoQUAD (RGBQUAD & q, COLORREF rgb) {
	/*
	q.rgbBlue = (rgb >> 16) & 0xff;
	q.rgbGreen = (rgb >> 8) & 0xff;
	q.rgbRed = rgb & 0xff;
	q.rgbReserved = 0;
	*/
	memcpy (&q, &rgb, 4);
}

bool Bitmap::Load (HDC hdc, LPCSTR filepath, COLORREF bg, COLORREF fg)
{
	HANDLE file;
	unsigned long alread;
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	PALETTE palette;
	BYTE * map;
	BITMAPINFOX bi;

	if (!filepath) return error (false, "Invalid path specified.");

	file = CreateFile (filepath, GENERIC_READ, 0, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);

	if (INVALID_HANDLE_VALUE == file) return error (false, "Couldn't open bitmap file.");

	if (FALSE == ReadFile (file, &fileHeader, sizeof(BITMAPFILEHEADER), &alread, 0)) {
		CloseHandle (file);
		return error (false, "Couldn't read the file header.");
	}

	if (FALSE == ReadFile (file, &infoHeader, sizeof(BITMAPINFOHEADER), &alread, 0)) {
		CloseHandle (file);
		return error (false, "Couldn't read the info header.");
	}

	//int imgSize = infoHeader.biSizeImage / (infoHeader.biBitCount>>3);
	int imgSize = infoHeader.biSizeImage;

	/*
	//TODO: TEST!!!!
	if( infoHeader.biBitCount <= 8 )
	{
		palette.count  = 1 << (infoHeader.biBitCount - 1);
		palette.colors = new RGBQUAD [palette.count];
    
		if (FALSE == ReadFile (file, palette.colors, palette.count, &alread, 0)) {
			CloseHandle (file);
			return error(false, "Couldn't read the palette.");
		}
	}
	*/

	map = new BYTE[infoHeader.biSizeImage];

	if (FALSE == ReadFile (file, map, infoHeader.biSizeImage, &alread, 0)) {
		CloseHandle (file);
		delete map;
		return error (false, "Couldn't read the image data.");
	}

	CloseHandle (file);  //Done with file.

	//File was loaded here, create the bitmap.

	bi.bmiHeader = infoHeader;
	RGBtoQUAD (bi.bmiColors [0], fg);
	RGBtoQUAD (bi.bmiColors [1], bg);

	/*
	// Correct the bitmap size.
	if (bmp) DeleteObject (bmp);
	bmp = CreateCompatibleBitmap (pdc,infoHeader.biWidth,infoHeader.biHeight);
	SelectObject (mdc, bmp);
	*/
	Create (hdc, infoHeader.biWidth, infoHeader.biHeight);

	//CreateCompatibleBitmap (pdc, infoHeader.biWidth, infoHeader.biHeight);
	SetDIBits (mdc, bmp, 0, infoHeader.biHeight, map, (CONST BITMAPINFO *) &bi, DIB_RGB_COLORS);

	delete map;

	return true;
}

bool Bitmap::Create (HDC hdc, UINT w, UINT h)
{
	if (bmp) Delete ();
	
	pdc = hdc;
	width = w;
	height = h;
	
	mdc = CreateCompatibleDC (hdc);
	if (!mdc) return false;
	bmp = CreateCompatibleBitmap (hdc,w,h);
	if (!bmp) { DeleteDC (mdc); return false; }
	SelectObject (mdc, bmp);
	return true;
}

void Bitmap::Delete ()
{
	if (bmp) DeleteObject (bmp); bmp = 0;
	if (mdc) DeleteDC (mdc); mdc = 0;
}

bool Bitmap::Paint (HDC hdc)
{
	if (!bmp) return false;
	if (!mdc) return false;
	BitBlt (hdc,0,0,width,height,mdc,0,0,SRCCOPY);
	return true;
}

bool Bitmap::Paint (int x, int y, int dx, int dy, int cy)
{
	if (!bmp) return false;
	if (!mdc) return false;
	if (cy < 0) cy = height;
	//return TRUE == BitBlt (pdc,0,0,width,height,mdc,x,y,SRCCOPY);
	//return TRUE == StretchBlt (pdc,0,0,width+dx,height+dy,mdc,x,y,width,height,SRCCOPY);
	return TRUE == StretchBlt (pdc,0,0,width+dx,cy+dy,mdc,x,y,width,cy,SRCCOPY);
}

bool Bitmap::Paint (int x, int y, float rx, float ry, int sx, int sy, int scx, int scy)
{
	if (!bmp) return false;
	if (!mdc) return false;
	if (scx < 0) scx = width;
	if (scy < 0) scy = height;
	float rscx = scx > 0 && scx < width ? scx / (float)width: 1.0f;
	float rscy = scy > 0 && scy < height ? scy / (float)height : 1.0f;

	return TRUE == StretchBlt (pdc, x, y, width*rx*rscx, height*ry*rscy, mdc, sx, sy, width*rscx, height*rscy, SRCCOPY);
}
/*
bool Bitmap::Clear (int x, int y, float rx, float ry, int sx, int sy, int scx, int scy)
{
	if (!bmp) return false;
	if (!mdc) return false;
	if (scx < 0) scx = width;
	if (scy < 0) scy = height;
	float rscx = scx > 0 && scx < width ? scx / (float)width: 1.0f;
	float rscy = scy > 0 && scy < height ? scy / (float)height : 1.0f;
	RECT r = {x, y, x+width*rx*rscx, y+height*ry*rscy};
	InflateRect (&r, 0, 30);
	//SelectObject (pdc, GetStockObject (BLACK_PEN));
	return TRUE == Rectangle (pdc, r.left, r.top, r.right, r.bottom);
}
*/
void Bitmap::SetTargetDC (HDC hdc)
{
	pdc = hdc;
}

bool Bitmap::Paint (HDC hdc, const LPRECT r, bool stretch)
{
	if (!bmp) return false;
	if (!mdc) return false;
	if (!r) return Paint (hdc);
	if (!stretch) return Paint (hdc, r, 0, 0);
	return TRUE == StretchBlt
		(hdc, r->left, r->top, r->right, r->bottom,
		mdc, 0, 0, width, height, SRCCOPY);
}

bool Bitmap::Paint (HDC hdc, const LPRECT r, int x, int y)
{
	if (!bmp) return false;
	if (!mdc) return false;
	if (!r) return Paint (hdc);
	return TRUE == BitBlt
		(hdc,r->left,r->top,r->right,r->bottom,mdc,x,y,SRCCOPY);
}

bool Bitmap::Paint (HDC hdc, const LPRECT dst, const LPRECT src, COLORREF transparent)
{
	if (!bmp) return false;
	if (!mdc) return false;
	return TRUE == TransparentBlt
		(hdc, dst->left, dst->top, dst->right, dst->bottom,
		mdc, src->left, src->top, src->right, src->bottom, transparent);
}

bool Bitmap::Flip ()
{
	return Paint (pdc);
}
