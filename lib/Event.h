#pragma once
#include "Common.h"
#include "Array.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (sys)

class Event
{
public:
	typedef std::vector <Event *> Vector;
	typedef lib::mem::Array <Event> Array;
protected:
	HANDLE handle;
public:
	Event ();
	~Event ();
	HANDLE Handle () const;
public:
	bool Create (bool manual = true, bool reset = true);
	bool Delete ();
	bool Set () const;
	bool Reset () const;
	bool Pulse () const;
	bool Test () const;
	bool Wait (DWORD msec = INFINITE) const;
};

END_NAMESPACE (sys)
END_NAMESPACE (lib)
