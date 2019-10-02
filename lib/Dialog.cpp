#include "Dialog.h"

INCLUDE_NAMESPACE (lib::gui)

bool Dialog::Destroy (int result)
{
	return TRUE == EndDialog (handle, result);
}

bool Dialog::SetTitle (LPCSTR text)
{
	return TRUE == SetWindowText (handle, text);
}

bool Dialog::SetIcon (UINT resid)
{
	HINSTANCE hinstance = GetModuleHandle (0);
	HICON icon = LoadIcon (hinstance, MAKEINTRESOURCE(resid));
	::SendMessage (handle, WM_SETICON, ICON_SMALL, (LPARAM) icon);
	::SendMessage (handle, WM_SETICON, ICON_BIG, (LPARAM) icon);
	return true;
}

LRESULT Dialog::SendChildMessage (UINT childid, UINT m, WPARAM w, LPARAM l, bool blocking)
{
	HWND hchild = 0;
	hchild = GetDlgItem (handle, childid);
	WNDPROC method = TRUE == blocking ? ::SendMessage : (WNDPROC) PostMessage;
	return method (hchild, m, w, l);
}
