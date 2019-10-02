#pragma once
#include "Common.h"
#include "Window.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class Child : public Window
{
protected:
	WNDPROC dispatcher;

public:
	Child ();
	Child (HWND handle);
	~Child ();
	inline operator HWND () const { return handle; }

public:
	using Window::Attach;
	bool Attach (HWND parent, UINT cid);
	bool Create (HWND parent, LPCSTR type, UINT id, DWORD style=0, DWORD exstyle=0);
	bool Subclass (WNDPROC callback);
	LRESULT Dispatch (UINT m, WPARAM w=0, LPARAM l=0) const;
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
