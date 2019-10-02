#pragma once
#include <vector>

class Worker;

class WorkerListener
{
public:
	typedef std::vector <WorkerListener *> Vector;

public:
	virtual void OnWorkerStart (Worker & worker, void * param) = 0;
	virtual void OnWorkerTerminate (Worker & worker, void * param) = 0;
};

class WorkerListeners :
	public WorkerListener::Vector,
	public WorkerListener
{
public:
	virtual void OnWorkerStart (Worker & worker, void * param)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnWorkerStart (worker, param);
	}

	virtual void OnWorkerTerminate (Worker & worker, void * param)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnWorkerTerminate (worker, param);
	}
};

class Worker
{
private:
	WorkerListeners listeners;
	HANDLE handle;
	DWORD id;
	VOID * args;
	bool working;

public:
	Worker () : handle(0), id(0), args(0), working(false)
	{
	}

	~Worker ()
	{
		Delete ();
	}

protected:
	static DWORD CALLBACK Callback (void * param)
	{
		if (!param) return -1;
		Worker * worker = (Worker *) param;
		worker->working = true;
		worker->listeners.OnWorkerStart (*worker, worker->args);
		worker->listeners.OnWorkerTerminate (*worker, worker->args);
		worker->working = false;
		return 0;
	}

public:
	bool Create (void * args = 0, bool suspended = false)
	{
		this->args = args;
		handle = CreateThread (0, 0, Worker::Callback, this,
			suspended ? CREATE_SUSPENDED : 0, &id);
		return !!handle;
	}

	void Delete ()
	{
		if (handle) CloseHandle (handle), handle = 0, id = 0;
	}

	void AddListener (WorkerListener * listener)
	{
		listeners.push_back (listener);
	}

	bool Ready () const { return !!handle; }
	bool Busy () const { return working; }

	bool Resume ()
	{
		if (!Ready ()) return false;
		return MAXDWORD != ResumeThread (handle);
	}

	bool Suspend ()
	{
		if (!Ready ()) return false;
		return MAXDWORD != SuspendThread (handle);
	}
};
