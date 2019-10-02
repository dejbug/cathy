#include "Window.h"

INCLUDE_NAMESPACE (lib::gui)

Window::Window (HWND handle)
{
	this->handle = handle;
}

bool Window::OutputLastErrorMessage ()
{
	LPTSTR lpMsgBuf;
	FormatMessage (
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);
	OutputDebugString (lpMsgBuf);
	LocalFree (lpMsgBuf);
	return true;
}

bool Window::Attach (HWND handle)
{
	this->handle = handle;
	return true;
}

bool Window::SetLocation (int x, int y)
{
	return TRUE == SetWindowPos (handle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

bool Window::SetSize (int cx, int cy)
{
	return TRUE == SetWindowPos (handle, 0, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
}

bool Window::GetSize (SIZE & size)
{
	RECT bounds;
	if (FALSE == GetClientRect (handle, &bounds)) return false;
	size.cx = bounds.right - bounds.left;
	size.cy = bounds.bottom - bounds.top;
	return true;
}

bool Window::GetBounds (RECT & rect)
{
	return TRUE == GetWindowRect (handle, &rect);
}

bool Window::SetBounds (RECT & rect)
{
	return TRUE == SetWindowPos (
		handle, 0, rect.left, rect.top,
		rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER);
}

bool Window::CenterToScreen ()
{
	int cx = GetSystemMetrics (SM_CXSCREEN);
	int cy = GetSystemMetrics (SM_CYSCREEN);
	RECT r;
	GetWindowRect (handle, &r);
	r.left = (cx - (r.right - r.left)) >> 1;
	r.top = (cy - (r.bottom - r.top)) >> 1;
	return SetLocation (r.left, r.top);
}

LRESULT Window::SendMessage (UINT m, WPARAM w, LPARAM l, BOOL blocking) const
{
	WNDPROC method = TRUE == blocking ? ::SendMessage : (WNDPROC) PostMessage;
	return method (handle, m, w, l);
}

bool Window::GetText (LPSTR buffer, DWORD size)
{
	return 0 < ::SendMessage (handle, WM_GETTEXT, (WPARAM) size, (LPARAM) buffer);
}

bool Window::SetText (LPCSTR text)
{
	return TRUE == ::SendMessage (handle, WM_SETTEXT, 0, (LPARAM) text);
}

bool Window::GetChildControl (Window & child, DWORD id)
{
	HWND childHandle = GetDlgItem (handle, id);
	return child.Attach (childHandle);
}

bool Window::SetFont (HFONT font)
{
	SendMessage (WM_SETFONT, (WPARAM) font);
	return true;
}

bool Window::Destroy ()
{
	return handle && TRUE == DestroyWindow (handle);
}

bool Window::Show (int cmd)
{
	return TRUE == ShowWindow (handle, cmd);
}

bool Window::Hide ()
{
	return TRUE == ShowWindow (handle, SW_HIDE);
}

bool Window::Enable ()
{
	return TRUE == EnableWindow (handle, TRUE);
}

bool Window::Disable ()
{
	return TRUE == EnableWindow (handle, FALSE);
}
