#pragma once
#include <vector>
#include <time.h>
#include "database/Database2.h"
#include "Chessboard.h"
#include "Clock.h"
#include "Timer.h"

#define TRAINER_NOTIFY(call) for(iterator it=begin(); it!=end(); it++) (*it)->call

class TrainerListener
{
public:
	typedef std::vector <TrainerListener *> Vector;

public:
	virtual void OnTrainerProblemLoaded (const Database2::Entry & entry, unsigned short index) = 0;
	virtual void OnTrainerStarted () = 0;
	virtual void OnTrainerStopped () = 0;
};

class TrainerListeners :
	public TrainerListener::Vector,
	public TrainerListener
{
public:
	virtual void OnTrainerProblemLoaded (const Database2::Entry & entry, unsigned short index)
	{
		TRAINER_NOTIFY (OnTrainerProblemLoaded (entry, index));
	}

	virtual void OnTrainerStarted ()
	{
		TRAINER_NOTIFY (OnTrainerStarted ());
	}

	virtual void OnTrainerStopped ()
	{
		TRAINER_NOTIFY (OnTrainerStopped ());
	}
};

class Trainer :
	public TimerListener,
	public ChessboardListener
{
public:
	typedef enum {None=0, Done, Playing, Stopping, Restarting} State;

private:
	TrainerListeners listeners;
	Timer timer;
	Database2 * database;
	Chessboard * chessboard;
	Clock * clock;

	class CURRENT { public:
		Database2::Entry entry;
		unsigned short index;
		unsigned short total;

		CURRENT () : index(0), total(0) {}
	} current;

	class TICKS { public:
		State state;
		bool turn;
		bool won;
		unsigned int opponent;
		unsigned int player;
		unsigned int waiting;

		TICKS () { Reset (); }
		
		void Reset ()
		{
			state = None;
			turn = won = false;
			opponent = player = waiting = 0;
		}
	} ticks;


public:
	Trainer () : database(0), chessboard(0), clock(0)
	{
		srand (time (0));
		timer.AddListener (this);
	}

	void AddListener (TrainerListener * listener)
	{
		listeners.push_back (listener);
	}

	void SetDatabase (Database2 * database)
	{
		this->database = database;
		if (database) current.total = database->Count ();
	}
	
	void SetChessboard (Chessboard * chessboard)
	{
		this->chessboard = chessboard;
		if (chessboard) chessboard->AddListener (this);
	}

	void SetClock (Clock * clock)
	{
		this->clock = clock;
	}

	bool Ready () const
	{
		return !!database && !!chessboard && !!clock;
	}

protected:
	unsigned short GetRandomIndex () const
	{
		return rand () % current.total;
	}

	bool GetRandomProblem (Database2::Entry & entry, unsigned short & index)
	{
		if (!Ready ()) return false;
		index = GetRandomIndex ();
		return database->Move (index) && entry.Read (*database);
	}

	bool UpdateChessboard (const Database2::Entry & entry)
	{
		if (!Ready ()) return false;
		return chessboard->SetupPosition (entry), true;
	}

	bool UpdateClock (const Database2::Entry & entry)
	{
		if (!Ready ()) return false;
		clock->Pause ();
		clock->Reset ();

		bool playerWhite = 1 == entry.header.color;

		if (!playerWhite) clock->Flip ();
//TODO...
		return true;
	}

	bool LoadRandomProblem ()
	{
		if (!Ready ()) return false;
		if (!GetRandomProblem (current.entry, current.index)) return false;
		if (!UpdateChessboard (current.entry)) return false;
		if (!UpdateClock (current.entry)) return false;
		listeners.OnTrainerProblemLoaded (current.entry, current.index);
		return true;
	}

public:	
	bool StartSession ()
	{
		ticks.Reset ();

		if (!LoadRandomProblem ()) return false;
		return timer.Start (1000);
	}

	bool StopSession ()
	{
		return timer.Stop ();
	}

protected:
	void OnProblemSolved ()
	{
		StopSession ();
		ticks.state = Restarting;
	}

	void OnProblemFailed ()
	{
		StopSession ();
	}

private:
	virtual void OnTimerStarted (Timer & timer)
	{
		clock->GetTopTime ().Set (3 * current.entry.header.moves);
		clock->GetTopTime ().SetCountdown (true);
		clock->Invalidate ();

		ticks.state = Playing;

		chessboard->SetDraggingEnabled (false);

		listeners.OnTrainerStarted ();
	}

	virtual void OnTimerStopped (Timer & timer)
	{
		if (Restarting == ticks.state) StartSession ();

		chessboard->SetDraggingEnabled (true);

		listeners.OnTrainerStopped ();
	}

	virtual void OnTimerSignalled (Timer & timer, __int64)
	{
		if (!Ready ()) StopSession ();

		if (Playing == ticks.state)
		{
			if (ticks.turn)
			{
				++ticks.player;
				clock->GetBottomTime ().Tick ();
				clock->Invalidate ();
			}
			else
			{
				++ticks.opponent;
				clock->GetTopTime ().Tick ();
				clock->Invalidate ();

				if (0 == ticks.opponent % 3)
				{
					ticks.turn = true;
					chessboard->MoveForward (false, true);
					//chessboard->MoveToLastCompletedMove ();
					//chessboard->MoveForward (false);
					chessboard->SetDraggingEnabled (true);
				}	
			}
		}
		else if (Stopping == ticks.state)
		{
			if (++ticks.waiting > 3)
			{
				if (ticks.won) OnProblemSolved ();
				else OnProblemFailed ();
			}
		}
		else if (Done == ticks.state)
		{
			if (ticks.won) OnProblemSolved ();
			else OnProblemFailed ();
		}

		//if (!chessboard->HasMoreMoves ()) StopSession ();
	}

private:
	virtual void OnChessboardWheelEvent (short delta)
	{
	}

	virtual void OnChessboardUserMove (unsigned char src, unsigned char dst, bool ok)
	{
		if (ticks.turn && src != dst && dst >= 0 && dst < 64)
		{
			if (ok)
			{
				if (chessboard->HasMoreMoves ())
					ticks.turn = false;
				else
				{
					ticks.won = true;
					ticks.state = Stopping;
				}

				chessboard->SetDraggingEnabled (false);
			}
			else
			{
				ticks.won = false;
				ticks.state = Done;
			}
		}
	}

	virtual void OnChessboardFlipped (bool inverted)
	{
	}

	virtual void OnChessboardLoaded (const Database2::Entry & entry)
	{
		
	}

	virtual void OnChessboardTurnChange (unsigned char color)
	{
	}

	virtual void OnChessboardUserInput (unsigned int key, unsigned int value)
	{
	}
};
