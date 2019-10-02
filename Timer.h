#pragma once
#include <vector>
#include "lib/Event.h"
#include "Worker.h"

#define TIMER_NOTIFY(call) for(iterator it=begin(); it!=end(); it++) (*it)->call

class Timer;

class TimerListener
{
public:
	typedef std::vector <TimerListener *> Vector;

public:
	virtual void OnTimerStarted (Timer & timer) = 0;
	virtual void OnTimerStopped (Timer & timer) = 0;
	virtual void OnTimerSignalled (Timer & timer, __int64 ticks) = 0;
};

class TimerListeners :
	public TimerListener::Vector,
	public TimerListener
{
public:
	virtual void OnTimerStarted (Timer & timer)
	{
		TIMER_NOTIFY (OnTimerStarted (timer));
	}

	virtual void OnTimerStopped (Timer & timer)
	{
		TIMER_NOTIFY (OnTimerStopped (timer));
	}
	
	virtual void OnTimerSignalled (Timer & timer, __int64 ticks)
	{
		TIMER_NOTIFY (OnTimerSignalled (timer, ticks));
	}
};

class Timer :
	public WorkerListener
{
private:
	TimerListeners listeners;
	Worker worker;
	lib::sys::Event killer, terminator;
	__int64 ticks;

public:
	Timer () : ticks(0L)
	{
		worker.AddListener (this);
	}

	~Timer ()
	{
	}

	void AddListener (TimerListener * listener)
	{
		listeners.push_back (listener);
	}

private:
	virtual void OnWorkerStart (Worker & worker, void * param)
	{
		listeners.OnTimerStarted (*this);

		while (!killer.Wait (1000))
		{
			listeners.OnTimerSignalled (*this, ++ticks);
		}
	}

	virtual void OnWorkerTerminate (Worker & worker, void * param)
	{
		terminator.Set ();
		listeners.OnTimerStopped (*this);
	}

public:
	bool Ready () const
	{
		return worker.Ready ();
	}

	bool Busy () const
	{
		return worker.Busy ();
	}

	bool Start (long period = 1000)
	{
		Stop ();

		ticks = 0L;
		
		killer.Delete ();
		killer.Create (false, true);

		terminator.Delete ();
		terminator.Create (false, true);
		
		return worker.Create (this);
	}

	bool Stop ()
	{
		return killer.Set ();
	}


};
