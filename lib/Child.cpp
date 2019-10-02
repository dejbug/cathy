#include "Child.h"

INCLUDE_NAMESPACE (lib::gui)

Child::Child ()
{
	handle = 0;
	dispatcher = 0;
}

Child::Child (HWND handle) : Window (handle)
{
}

Child::~Child ()
{
}

bool Child::Attach (HWND parent, UINT cid)
{
	Window window (parent);
	return window.GetChildControl (*this, cid);
}

bool Child::Create (HWND parent, LPCSTR type, UINT id, DWORD style, DWORD exstyle)
{
	if (handle) DestroyWindow (handle);
	HINSTANCE instance = (HINSTANCE) GetWindowLong (parent,GWL_HINSTANCE);
	style |= WS_CHILD;
	handle = CreateWindowEx (exstyle,type,"",style,
		10,10,100,100,parent,(HMENU)id, instance,0);
	return !!handle;
}

bool Child::Subclass (WNDPROC callback)
{
	if (!dispatcher) dispatcher = (WNDPROC) GetWindowLong (handle, GWL_WNDPROC);
	SetWindowLong (handle, GWL_WNDPROC, (LONG) callback);
	SendMessage (WM_SUBCLASS, (WPARAM) handle, (LPARAM) this);
	return true;
}

LRESULT Child::Dispatch (UINT m, WPARAM w, LPARAM l) const
{
	if (dispatcher) return CallWindowProc (dispatcher, handle, m, w, l);
	return DefWindowProc (handle,m,w,l);
}
