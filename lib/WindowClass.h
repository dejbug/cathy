#pragma once

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class WindowClass : public WNDCLASSEX
{
private:
	ATOM atom;

public:
	WindowClass () : atom(0)
	{
		cbSize = sizeof (WNDCLASSEX);
	}

	bool Create (HINSTANCE instance, LPCSTR name, WNDPROC callback = DefWindowProc)
	{
		style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
		lpfnWndProc = callback;
		cbClsExtra = 0;
		cbWndExtra = 0;
		hInstance = instance;
		hIcon = LoadIcon (0, MAKEINTRESOURCE(IDI_APPLICATION));
		hCursor = LoadCursor (0, MAKEINTRESOURCE(IDC_ARROW));
		hbrBackground = (HBRUSH) COLOR_WINDOW;
		lpszMenuName = 0;
		lpszClassName = name; // Add some random string.
		hIconSm = hIcon;
		return true;
	}

	bool Register ()
	{
		atom = RegisterClassEx ((WNDCLASSEX *) this);
		return 0 != atom;
	}

	bool Unregister ()
	{
		return TRUE == UnregisterClass (lpszClassName, hInstance);
	}
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
