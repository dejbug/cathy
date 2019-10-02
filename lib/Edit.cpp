#include "Edit.h"

INCLUDE_NAMESPACE (lib::gui)

bool Edit::Attach (HWND parent, UINT cid)
{
	Window window (parent);
	return window.GetChildControl (*this, cid);
}

bool Edit::AppendText (LPCSTR text)
{
	SendMessage (EM_SETSEL, MAXDWORD-1, MAXDWORD-1);
	SendMessage (EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) text);
	return true;
}
