#pragma once
#include "Common.h"
#include "Child.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (gui)

class Edit : public Child
{
public:
	bool Attach (HWND parent, UINT cid);

public:
	bool AppendText (LPCSTR text);
};

END_NAMESPACE (gui)
END_NAMESPACE (lib)
