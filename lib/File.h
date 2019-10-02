#pragma once
#include "Common.h"
#include <vector>

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (sys)

class File
{
public:
	typedef std::vector <DWORD> Marks;
protected:
	HANDLE handle;
	Marks marks;

public:
	File ();

public:
	bool Open (LPCSTR path, DWORD access = GENERIC_READ, DWORD share = 0, DWORD disposition = OPEN_EXISTING, DWORD flags = 0);
	bool Close ();
	bool Ready ();
	bool Read (VOID * buffer, DWORD size, DWORD * done);
	bool Write (VOID * buffer, DWORD size, DWORD * done);
	bool Size (DWORD * low, DWORD * high = 0);
	bool Tell (DWORD * low, LONG * high = 0);
	bool Seek (LONG offset, DWORD method = FILE_BEGIN);
	bool More ();

	bool Rewind ();
	bool Mark ();
	bool Back ();
};

END_NAMESPACE (sys)
END_NAMESPACE (lib)
