#pragma once
#include "lib/Frame.h"

class Button : public lib::gui::Frame
{
public:
	bool Create (HWND parent, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "BUTTON",id, WS_CHILD|style, exstyle);
	}
};
