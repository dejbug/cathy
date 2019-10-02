#pragma once

class Timer
{
private:
	HANDLE handle;

public:
	Timer () : handle(0)
	{
	}

public:
	bool Create (bool manualReset)
	{
		return handle = CreateWaitableTimer (0, manualReset, 0), !!handle;
	}

	bool CreateNotificationTimer ()
	{
		return Create (true);
	}

	bool CreateSynchronisationTimer ()
	{
		return Create (false);
	}

	void Delete ()
	{
		if (handle) CloseHandle (handle), handle = 0;
	}

	bool Signal ()
	{
		return handle && TRUE == SetEvent (handle);
	}

	bool Reset ()
	{
		return handle && TRUE == ResetEvent (handle);
	}

	bool SetPeriodic (DWORD msec)
	{
		return handle && TRUE == SetWaitableTimer (handle, 0, msec, 0, 0, FALSE);
	}

	bool Set (DWORD msec)
	{
		if (!handle) return false;
		LARGE_INTEGER t = msec * 10000;
		return TRUE == SetWaitableTimer (handle, &t, 0, 0, 0, FALSE);
	}

	bool Cancel ()
	{
		return handle && TRUE == CancelWaitableTimer (handle);
	}
};

class Operator
{
private:
	bool running;

public:
	Operator () : running(false)
	{
	}

private:
	static DWORD CALLBACK Main (VOID * param)
	{
		Operator * op = (Operator *) param;
		return op->Run ();
	}

protected:
	virtual void Run ()
	{
		while (Continue ()) Execute (), Delay ();
	}

	virtual bool Continue ()
	{
		return running;
	}

	virtual void Delay ()
	{
		
	}

	virtual void Execute ()
	{
		
	}
};
