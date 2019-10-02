#include "Thread.h"

INCLUDE_NAMESPACE (lib::sys)

Thread::Context::Context ()
{
	callback = 0;
	argument = 0;
}

Thread::Context::Context (LPTHREAD_START_ROUTINE callback, LPVOID argument)
{
	this->callback = callback;
	this->argument = argument;
}

Thread::Thread ()
{
	handle = 0;
	id = 0;
}

Thread::Thread (LPTHREAD_START_ROUTINE callback, LPVOID argument)
{
	handle = 0;
	id = 0;
	context.callback = callback;
	context.argument = argument;
}

Thread::~Thread ()
{
	if (handle) Delete ();
}

HANDLE Thread::Handle () const
{
	return handle;
}

const Event * Thread::Killer () const
{
	return &killer;
}

DWORD CALLBACK Thread::Dispatcher (VOID * param)
{
	if (!param) return 0xFFFFFFFF;
	Thread * thread = (Thread *) param;
	return thread->result = thread->Callback (thread->context.argument);
}

DWORD Thread::Callback (VOID * param)
{
	return 0;
}

bool Thread::Create (LPVOID argument, bool suspended)
{
	if (!killer.Create ()) return false;
	
	LPTHREAD_START_ROUTINE callback;

	if (!context.callback)
	{
		context.argument = argument;
		callback = Dispatcher;
		argument = (VOID *) this;
	}
	else
	{
		callback = context.callback;
		argument = context.argument;
	}

	handle = CreateThread (0, 0, callback, argument, 
		suspended ? CREATE_SUSPENDED : 0, &id);
	return !!handle;
}

bool Thread::Delete ()
{
	if (handle) {
		CloseHandle (handle);
		handle = 0;
	}
	killer.Delete ();
	return true;
}

bool Thread::Resume ()
{
	if (!handle) return false;
	return 0xffffffff != ResumeThread (handle);
}

bool Thread::Suspend ()
{
	if (!handle) return false;
	return 0xffffffff != SuspendThread (handle);
}

bool Thread::Kill ()
{
	return killer.Set ();
}

bool Thread::Wait (DWORD msec) const
{
	if (!handle) return false;
	return WAIT_OBJECT_0 == WaitForSingleObject (handle, msec);
}

DWORD Thread::Result () const
{
	return result;
}
