#include "String.h"

INCLUDE_NAMESPACE (lib::mem)

String::String () : Buffer <CHAR> ()
{
}

String::String (DWORD size) : Buffer <CHAR>  ()
{
	Create (size);
}

String::String (LPCSTR text) : Buffer <CHAR>  ()
{
	Set (text);
}

String::~String ()
{
	//Delete (); //if (data) delete data; data = 0;
}

String::operator DWORD () const
{
	return size;
}

String::operator LPSTR () const
{
	return data;
}

LPSTR String::Text () const
{
	return data;
}

DWORD String::Size () const
{
	return size;
}

DWORD String::Length () const
{
	return data ? strlen (data) : 0;
}

DWORD String::Free () const
{
	if (!data) return 0;
	return Size () - Length ();
}

bool String::Empty () const
{
	return !data || !data[0];
}

bool String::Set (LPCSTR text)
{
	if (!text) return false;
	if (text == data) return true;
	Delete ();
	data = strdup (text);
	size = strlen (data) + 1;
	return !!data;
}

bool String::Copy (LPCSTR text)
{
	if (!text) return false;
	if (!data || !size) return false;
	strncpy (data, text, size-1);
	data [size-1] = 0;
	return true;
}

bool String::Append (LPCSTR text)
{
	if (!data || !size) return false;
	//DWORD size = _msize (data);
	strcat (data, text);
	data [size-1] = 0;
	return true;
}

bool String::Terminate ()
{
	if (!data || !size) return false;
	//DWORD size = _msize (data);
	//if (!size) return false;
	data [size-1] = 0;
	return true;
}
