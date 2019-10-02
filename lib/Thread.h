#pragma once
#include "Common.h"
#include "Event.h"
#include "Array.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (sys)

class Thread
{
public:
	typedef std::vector <Thread *> Vector;
	typedef lib::mem::Array <Thread> Array;
	class Context
	{
	public:
		LPTHREAD_START_ROUTINE callback;
		VOID * argument;
	public:
		Context ();
		Context (LPTHREAD_START_ROUTINE callback, LPVOID argument = 0);
	};
protected:
	HANDLE handle;
	DWORD id;
	DWORD result;
	Event killer;
	Context context;
public:
	Thread ();
	Thread (LPTHREAD_START_ROUTINE callback, LPVOID argument = 0);
	~Thread ();
	HANDLE Handle () const;
	const Event * Killer () const;
protected:
	static DWORD CALLBACK Dispatcher (VOID * param);
	virtual DWORD Callback (VOID * param);
public:
	bool Create (LPVOID argument = 0, bool suspended = false);
	bool Delete ();
	bool Resume ();
	bool Suspend ();
	bool Kill ();
	bool Wait (DWORD msec = INFINITE) const;
	DWORD Result () const;
};

END_NAMESPACE (sys)
END_NAMESPACE (lib)
