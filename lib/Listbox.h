#pragma once
#include "Common.h"
#include "Child.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class Listbox : public Child
{
public:
	void AddItemText (LPCSTR text)
	{
		SendMessage (LB_ADDSTRING, 0, (LPARAM) text);
	}

	void GetItemText (int index, LPSTR buffer)
	{
		SendMessage (LB_GETTEXT, (WPARAM) index, (LPARAM) buffer);
	}
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
