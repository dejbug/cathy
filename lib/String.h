#pragma once
#include "Common.h"
#include "Buffer.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (mem)

class String : public Buffer <CHAR>
{
public:
	typedef std::vector <String *> Vector;
public:
	String ();
	String (DWORD size);
	String (LPCSTR text);
	~String ();
public:
	operator LPSTR () const;
	operator DWORD () const;
public:
	LPSTR Text () const;
	DWORD Size () const;
	DWORD Length () const;
	DWORD Free () const;
	bool Empty () const;

	bool Set (LPCSTR text);
	bool Copy (LPCSTR text);
	bool Append (LPCSTR text);
	bool Terminate ();
};

END_NAMESPACE (mem)
END_NAMESPACE (lib)
