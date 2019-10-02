#pragma once
#include <windows.h>

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gdi)

class Font {
private:
	HFONT handle;
public:
	Font(LPCSTR face, int size);
	~Font();
	operator WPARAM () { return (WPARAM) handle; }
	operator HFONT () { return handle; }
	HFONT GetHandle () { return handle; }
};

END_NAMESPACE (gdi)
END_NAMESPACE (lib)
