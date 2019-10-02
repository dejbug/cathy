#include "Event.h"

INCLUDE_NAMESPACE (lib::sys)

Event::Event ()
{
	handle = 0;
}

Event::~Event ()
{
	if (handle) Delete ();
}

HANDLE Event::Handle () const
{
	return handle;
}

bool Event::Create (bool manual, bool reset)
{
	handle = CreateEvent (0, manual ? TRUE : FALSE, reset ? FALSE : TRUE, 0);
	return !!handle;
}

bool Event::Delete ()
{
	if (handle) {
		CloseHandle (handle);
		handle = 0;
	}
	return true;
}

bool Event::Set () const
{
	if (!handle) return false;
	return TRUE == SetEvent (handle);
}

bool Event::Reset () const
{
	if (!handle) return false;
	return TRUE == ResetEvent (handle);
}

bool Event::Pulse () const
{
	if (!handle) return false;
	return TRUE == PulseEvent (handle);
}

bool Event::Test () const
{
	if (!handle) return false;
	return WAIT_OBJECT_0 == WaitForSingleObject (handle, 0);
}

bool Event::Wait (DWORD msec) const
{
	if (!handle) return false;
	return WAIT_OBJECT_0 == WaitForSingleObject (handle, msec);
}
