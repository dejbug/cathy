#pragma once
#include "Common.h"
#include "Window.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class Dialog : public Window
{
public:
	bool Destroy (int result = 0);
	bool SetTitle (LPCSTR text);
	bool SetIcon (UINT resid);
	LRESULT SendChildMessage (UINT childid, UINT msg, WPARAM w = 0, LPARAM l = 0, bool blocking = true);
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
