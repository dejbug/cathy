#pragma once
#include "Common.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class Window
{
protected:
	HWND handle;

public:
	Window (HWND handle = 0);
	operator HWND () { return handle; }
	HWND GetHandle () const { return handle; }

public:
	static bool OutputLastErrorMessage ();

public:
	bool Attach (HWND handle);
	bool SetLocation (int x, int y);
	bool SetSize (int cx, int cy);
	bool GetSize (SIZE & size);
	bool GetBounds (RECT & rect);
	bool SetBounds (RECT & rect);
	bool CenterToScreen ();
	LRESULT SendMessage (UINT m, WPARAM w = 0, LPARAM l = 0, BOOL blocking = TRUE) const;

	bool GetText (LPSTR buffer, DWORD size);
	bool SetText (LPCSTR text);
	bool GetChildControl (Window & child, DWORD id);
	bool SetFont (HFONT);

	bool Destroy ();
	bool Show (int cmd = SW_SHOW);
	bool Hide ();
	bool Enable ();
	bool Disable ();

	bool Move (int x, int y, int cx, int cy)
	{
		return TRUE == SetWindowPos (handle, 0, x, y, cx, cy, SWP_NOZORDER);
	}

	bool MoveUnder (HWND h)
	{
		return TRUE == SetWindowPos (handle, h, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	bool MoveOver (HWND h)
	{
		return TRUE == SetWindowPos (h, handle, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
