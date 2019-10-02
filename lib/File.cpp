#include "File.h"

INCLUDE_NAMESPACE (lib::sys)

File::File () : handle (INVALID_HANDLE_VALUE)
{
}

bool File::Open (LPCSTR path, DWORD access, DWORD share, DWORD disposition, DWORD flags)
{
	handle = CreateFile (path, access, share, 0, disposition, 0, 0);
	return INVALID_HANDLE_VALUE != handle;
}

bool File::Close ()
{
	if (INVALID_HANDLE_VALUE != handle) {
		CloseHandle (handle);
		handle = INVALID_HANDLE_VALUE;
	}
	return true;
}

bool File::Ready ()
{
	return INVALID_HANDLE_VALUE != handle;
}

bool File::Read (VOID * buffer, DWORD size, DWORD * done)
{
	if (INVALID_HANDLE_VALUE == handle) return false;
	return TRUE == ReadFile (handle, buffer, size, done, 0);
}

bool File::Write (VOID * buffer, DWORD size, DWORD * done)
{
	if (INVALID_HANDLE_VALUE == handle) return false;
	return TRUE == WriteFile (handle, buffer, size, done, 0);
}

bool File::Seek (LONG offset, DWORD method)
{
	if (INVALID_HANDLE_VALUE == handle) return false;
	LONG high = 0;
	DWORD low = SetFilePointer (handle, offset, &high, method);
	return 0xFFFFFFFF != low || NO_ERROR == GetLastError ();
}

bool File::Tell (DWORD * low, LONG * high)
{
	if (INVALID_HANDLE_VALUE == handle) return false;
	if (!low) return false;
	if (high) *high = 0;
	*low = SetFilePointer (handle, 0, high, FILE_CURRENT);
	return !(0xFFFFFFFF == *low && (!high || NO_ERROR != GetLastError ()));
}

bool File::Size (DWORD * low, DWORD * high)
{
	if (INVALID_HANDLE_VALUE == handle) return false;
	if (!low) return false;
	*low = GetFileSize (handle, high);
	return !(0xFFFFFFFF == *low && (!high || NO_ERROR != GetLastError ()));
}

bool File::More ()
{
	if (INVALID_HANDLE_VALUE == handle) return false;
	DWORD sizeLow, sizeHigh;
	DWORD posLow; LONG posHigh;
	if (!Size (&sizeLow, &sizeHigh)) return false;
	if (!Tell (&posLow, &posHigh)) return false;
	return sizeHigh > posHigh || (sizeHigh == posHigh && sizeLow > posLow);
}

bool File::Rewind ()
{
	return Seek (0, SEEK_SET);
}

bool File::Mark ()
{
	DWORD mark;
	if (!Tell (&mark)) return false;
	marks.push_back (mark);
	return true;
}

bool File::Back ()
{
	if (marks.empty ()) return false;
	DWORD mark = marks.back ();
	marks.pop_back ();
	return Seek (mark, SEEK_SET);
}
