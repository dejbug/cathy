#include "Font.h"

INCLUDE_NAMESPACE (lib::gdi)

Font::Font(LPCSTR face, int size) : handle(0) {
	handle = CreateFont (size,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FF_DONTCARE, face);
}

Font::~Font() {
	if (0 != handle) DeleteObject (handle);
}
