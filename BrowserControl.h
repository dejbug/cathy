#pragma once
#include "lib/Frame.h"
#include "lib/Surface.h"
#include "database/Database.h"
#include "Button.h"
#include "Editbox.h"
#include "Static.h"
#include "Chessboard.h"
#include "Worker.h"

#include "database/Database2.h"

#include "ProblemInfoBox.h"
#include "Clock.h"
#include "Trainer.h"

//NOTE: p00173 (#58) is an example for a pawn promotion which
// is not to a queen (but to a knight).

typedef struct _PROBLEM_DETAILS
{
	char index[16];
	char id[16];
	char rating[8];
	char rd[8];
	char moves[8];
	char color[8];

} PROBLEM_DETAILS, FAR * LPPROBLEM_DETAILS;


class BrowserControlListener
{
public:
	typedef std::vector <BrowserControlListener *> Vector;

public:
	virtual void OnMinimumSizeThreshold () = 0;
};

class BrowserControlListeners :
	public BrowserControlListener::Vector,
	public BrowserControlListener
{
public:
	virtual void OnMinimumSizeThreshold () {
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnMinimumSizeThreshold ();
	}
};

class BrowserControl :
	public lib::gui::Frame,
	public WorkerListener,
	public Editbox2Listener,
	public ChessboardListener,
	public ClockListener,
	public TrainerListener

{
private:
	Chessboard * chessboard;
	Database2 database;
	Database2::Entry entry;
	unsigned short currentProblemIndex;
	//unsigned short currentMoveIndex;

	BrowserControlListeners listeners;
	Button btnPrev, btnNext, btnLoad, btnFlip, btnPlay, btnBack, btnFrwd, btnStartSession;
	Editbox2 edtIndex, edtId;
	Static staRating, staRD, staMoves, staColor;

	Worker worker;

	ProblemInfoBox infobox;
	Clock clock;

	Trainer trainer;
	bool training;

public:
	BrowserControl () : chessboard(0), currentProblemIndex(0)
	{
		worker.AddListener (this);
		trainer.AddListener (this);
		training = false;
	}

public:
	bool Create (HWND parent, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "BROWSERCONTROL", id,
			WS_CHILD|style, exstyle, false);
	}
	
	void AddListener (BrowserControlListener * listener) {
		listeners.push_back (listener);
	}

	void SetChessboard (Chessboard * chessboard)
	{
		this->chessboard = chessboard;
	}

	void BrowseDatabase ()
	{
		if (!OpenDatabase ())
		{
			MessageBox (handle, "Please make sure that 'base.cts' is\nin the same folder as 'cathy.exe'\nbefore you start this program.", "Database not found!", MB_ICONEXCLAMATION);
			ExitProcess (-1);
		}

#ifndef _DEBUG
		LoadProblem (0);
#else
//		LoadProblem (208);	// Castling (kingside).
		LoadProblem (32);	// Pawn promotion (to Queen).
#endif

		trainer.SetDatabase (&database);
		trainer.SetChessboard (chessboard);
		trainer.SetClock (&clock);

	}

	bool LoadRandomProblem()
	{
		int total = database.Count();
		int index = rand () % total;
		return LoadProblem(index);
	}

	void GetProblemDetails(PROBLEM_DETAILS & info)
	{
		strncpy(info.index, edtIndex.GetText(), 16);
		strncpy(info.id, edtId.GetText(), 16);
		strncpy(info.rating, staRating.GetText(), 8);
		strncpy(info.rd, staRD.GetText(), 8);
		strncpy(info.moves, staMoves.GetText(), 8);
		strncpy(info.color, staColor.GetText(), 8);
	}

protected:
	/// Replays the currently displayed position's moves.
	bool PlayBackChessboardMovesForCurrentEntry (DWORD delay = 1000)
	{
		if (!chessboard) return false;

		chessboard->RewindHistory ();

		for (int i=0; i<entry.header.moves<<1; i++)
		{
			Sleep (delay);
			chessboard->MoveForward ();
		}
		
		return true;
	}

	/*
	/// Collects and displays information about the positions in the database.
	bool PrintDatabaseInfo ()
	{
		int maxIdDelta = 0;
		int maxPosDelta = 0;
		int maxMoves = 0;
		int minRating = MAXDWORD,
			maxRating = 0;
		int minRD = MAXDWORD,
			maxRD = 0;

		int lastId = 0;
		int lastPos = 0;

		database.Move (0);
		unsigned short count = database.Count ();

		for (int i=0; i<count; i++)
		{
			entry.Read (database);

			char buffer [64];
			sprintf (buffer, "%d", i + 1);
			edtIndex.SetText (buffer);

			sprintf (buffer, "%d", entry.header.id);
			edtId.SetText (buffer);

			maxIdDelta = max (maxIdDelta, entry.header.id-lastId);
			lastId = entry.header.id;

			maxPosDelta = max (maxPosDelta, entry.header.pos-lastPos);
			lastPos = entry.header.pos;

			maxMoves = max (maxMoves, entry.header.moves);
			minRating = min (minRating, entry.header.rating);
			maxRating = max (maxRating, entry.header.rating);
			minRD = min (minRD, entry.header.rd);
			maxRD = max (maxRD, entry.header.rd);
		}

		char buffer [128];
		sprintf (buffer,
			"maxIdDelta: %d\r\n"
			"maxPosDelta: %d\r\n"
			"maxMoves: %d\r\n"
			"rating: %d-%d\r\n"
			"deviation: %d-%d\r\n",
			maxIdDelta, maxPosDelta, maxMoves,
			minRating, maxRating, minRD, maxRD);
		OutputDebugString (buffer);
		MessageBox (handle, buffer, "Database Info", MB_OK|MB_ICONINFORMATION);

		return true;
	}
	*/

private:
	virtual void OnWorkerStart (Worker & worker, void * param)
	{
		if (!param) return;
		switch (*(int*)param)
		{
			case 0: PlayBackChessboardMovesForCurrentEntry (); break;
			//case 1: PrintDatabaseInfo (); break;
		}
	}

	virtual void OnWorkerTerminate (Worker & worker, void * param)
	{
	}

	/// Handles the input for the Editbox2 objects. Loads the positions
	/// by index or id, provided the entered index or id is correct.
	virtual void OnUserInputDone (Editbox2 & box, const char * text)
	{
		if (text && *text)
		{
			if (box == edtIndex)
			{
				int index = atoi (text) - 1;
				if (index >= 0 && index < database.Count () &&
					index != currentProblemIndex)
					LoadProblem (index);
			}
			else if (box == edtId)
			{
				int id = atoi (text);
				//LoadProblemById (id);
				unsigned short index;
				if (database.FindEntryIndexById (id, index) &&
					index != currentProblemIndex)
					LoadProblem (index);
			}
		}
	}

	/// Called when the user uses the mousewheel over the chessboard
	/// control. Used to cycle through the current position's move history.
	virtual void OnChessboardWheelEvent (short delta)
	{
		if (chessboard)
		{
			if (delta > 0) chessboard->MoveBackward ();
			else chessboard->MoveForward (training);
		}
	}

	virtual void OnChessboardUserMove (unsigned char src, unsigned char dst, bool ok)
	{
	}

	virtual void OnChessboardFlipped (bool inverted)
	{
	}

	virtual void OnChessboardLoaded (const Database2::Entry & entry)
	{
		clock.SetFlipped (0 == entry.header.color);
	}

	virtual void OnChessboardTurnChange (unsigned char color)
	{
	}

	virtual void OnChessboardUserInput (unsigned int key, unsigned int value)
	{
		switch(key)
		{
			case CHESSBOARD_LOAD_RANDOM_PROBLEM:
				LoadRandomProblem();
				break;

			case CHESSBOARD_LOAD_PREVIOUS_PROBLEM:
				LoadPreviousProblem();
				break;
			
			case CHESSBOARD_LOAD_NEXT_PROBLEM:
				LoadNextProblem();
				break;
		}
	}

	virtual void OnClockTick ()
	{
	}

	virtual void OnTrainerProblemLoaded (const Database2::Entry & entry, unsigned short index)
	{
		currentProblemIndex = index;
		this->entry = entry;

		UpdateIndices (index + 1, entry.header.id);
		UpdateInformation (entry.header);
	}

	virtual void OnTrainerStarted ()
	{
		training = true;
		EnableControls (false);
	}

	virtual void OnTrainerStopped ()
	{
		training = false;
		EnableControls (true);
	}

protected:
	/// Sets the index and id edit boxes to reflect the specified
	/// index and id values. This is called whenever a new position
	/// is loaded.
	bool UpdateIndices (unsigned short index, unsigned short id)
	{
		char buffer [64];
		sprintf (buffer, "%d", index);
		edtIndex.SetText (buffer);

		sprintf (buffer, "%d", id);
		edtId.SetText (buffer);
		return true;
	}

	/// Updates the information controls for a given entry header.
	/// This is called whenever a new position is loaded. The updated
	/// controls reflect the position's rating, rd, ply count and
	/// the color of the first (i.e. the opponent's) move.
	bool UpdateInformation (const Database2::Header & header)
	{
		staRating.SetText ("%d", header.rating);
		staRD.SetText ("%d", header.rd);
		staMoves.SetText ("%d", header.moves << 1);
		staColor.SetText ("%s", header.color ? "black" : "white");
		return true;
	}

	/// Loads a position into the chessboard control.
	bool UpdateChessboard (const Database2::Entry & entry)
	{
		return !!chessboard ? chessboard->SetupPosition (entry), true : false;
	}

	/// Enables or disables all the user input controls (namely all the
	/// buttons and edit boxes). All output-only controls (i.e. the
	/// static information controls) are unaffected.
	void EnableControls (bool enable)
	{
		edtIndex.SetEditable (enable);
		edtId.SetEditable (enable);

		btnPrev.SetEnabled (enable);
		btnNext.SetEnabled (enable);
		btnFlip.SetEnabled (enable);
		btnPlay.SetEnabled (enable);
		btnBack.SetEnabled (enable);
		btnFrwd.SetEnabled (enable);
	}

	/// Attempts to open the database.
	/// The database is currently assumed to be found in a file called
	/// "base.cts" which is located in the same folder as the executable.
	bool OpenDatabase ()
	{
		if (!database.Open ("base.cts", true, true)) return false;

		char buffer [64];
		sprintf (buffer, "%s of %d", "%s", database.Count ());
		edtIndex.SetFormat (buffer);
		
		return true;
	}

	/// Finds a position in the database with the given id, then loads
	/// it. If no position is with the exact id was found, the next
	/// highest id will be loaded instead.
	bool LoadProblemById (unsigned short id)
	{
		if (training) return false;
		if (!database.Ready ()) return false;

//TODO: Implement binary tree search. First entry to check for id is
// at database.Count () >> 1. Next is the middle of one half.

		database.Move (0);

		for (int i=0; i<database.Count (); i++)
		{
			entry.Read (database);
			if (id <= entry.header.id)
				return LoadProblem (i);
		}

		return false;
	}

	/// Loads a position by its index in the database.
	bool LoadProblem (unsigned short index)
	{
		if (training) return false;
		if (!database.Ready ()) return false;

		database.Move (index);
		entry.Read (database);

		UpdateIndices (index + 1, entry.header.id);
		UpdateInformation (entry.header);

		currentProblemIndex = index;

		UpdateChessboard (entry);

		return true;
	}

	bool LoadPreviousProblem ()
	{
		if (!database.Ready ()) return false;
		if (0 == currentProblemIndex) return false;
		return LoadProblem (currentProblemIndex - 1);
	}

	bool LoadNextProblem ()
	{
		if (!database.Ready ()) return false;
		if (database.Count ()-1 == currentProblemIndex) return false;
		return LoadProblem (currentProblemIndex + 1);
	}

	/// Starts a thread to playback the current position's move
	/// history.
	bool ReplayMoves ()
	{
		if (training) return false;
		if (worker.Busy ()) return false;
		static int code = 0;
		worker.Create (&code);
		return true;
	}

private:
	virtual void OnCreate (HWND handle)
	{
		btnPrev.Create (handle, 1010);
		btnPrev.SetText ("Prev");

		btnNext.Create (handle, 1011);
		btnNext.SetText ("Next");

		/*
		HICON icon1 = LoadIcon (GetInstance (handle), MAKEINTRESOURCE (IDI_ICON1));

		btnLoad.Create (handle, 1012, WS_VISIBLE|BS_ICON);
		btnLoad.SetText ("Load");
		btnLoad.Message (BM_SETIMAGE, IMAGE_ICON, (LONG) icon1);
		*/

		btnFlip.Create (handle, 1013, WS_VISIBLE);
		btnFlip.SetText ("Flip");

		btnPlay.Create (handle, 1014, WS_VISIBLE);
		btnPlay.SetText ("Play");

		btnBack.Create (handle, 1015, WS_VISIBLE);
		btnBack.SetText ("<");

		btnFrwd.Create (handle, 1016);
		btnFrwd.SetText (">");

		btnStartSession.Create (handle, 1017);
		btnStartSession.SetText ("Start Session");

		edtIndex.Create (handle, 1020, WS_VISIBLE|WS_BORDER|
			ES_CENTER|ES_NUMBER);
		edtIndex.AddListener (this);
		edtIndex.Message (EM_SETLIMITTEXT, 5);
		edtIndex.SetFormat ("%s of 0");

		edtId.Create (handle, 1021, WS_VISIBLE|WS_BORDER|
			ES_CENTER|ES_NUMBER);
		edtId.AddListener (this);
		edtId.Message (EM_SETLIMITTEXT, 5);
		edtId.SetFormat ("p%05s");

		staRating.Create (handle, 1031);
		staRD.Create (handle, 1032);
		staMoves.Create (handle, 1033);
		staColor.Create (handle, 1034);

		staRating.SetFormat ("Rating: %s");
		staRating.SetAlternateFormat ("Rating: ?");
		staRD.SetFormat ("RD: %s");
		staRD.SetAlternateFormat ("RD: ?");
		staMoves.SetFormat ("Plys: %s");
		staMoves.SetAlternateFormat ("Moves: ?");
		staColor.SetFormat ("Turn: %s");
		staColor.SetAlternateFormat ("Color: ?");

		//infobox.Create (handle, 1041);
		clock.Create (handle, 1042, 0, WS_EX_STATICEDGE);
	}

	virtual void OnSize (DWORD flags, WORD cx, WORD cy)
	{
		int xmid = cx>>1;
		int xgap = 0;

		btnPrev.Move (0,0,xmid-xgap,20);
		btnNext.Move (xmid+xgap,0,xmid-xgap,20);
		btnFlip.Move (0,160,xmid-xgap,20);
		btnPlay.Move (xmid+xgap,160,xmid-xgap,20);
		btnBack.Move (0,180,xmid-xgap,20);
		btnFrwd.Move (xmid+xgap,180,xmid-xgap,20);
		btnStartSession.Move (10, 290, cx-20, 20);

		//btnLoad.Move (0,cy-40,40,40);

		edtIndex.Move (0,20,cx,20);
		edtId.Move (0,40,cx,20);

		staRating.Move (10,70,cx-20,20);
		staRD.Move (10,90,cx-20,20);
		staMoves.Move (10,110,cx-20,20);
		staColor.Move (10,130,cx-20,20);

		//infobox.Move (0,160,cx,120);

		clock.Move (10, 220, cx-20, 60);
		//clock.SetBlackTime (55,59,23);

		if (cx < 32) listeners.OnMinimumSizeThreshold ();
	}

	virtual void OnCommand (WORD code, WORD id, HWND h)
	{
		switch (code)
		{
			case BN_CLICKED:
				if (0 == h) break;

				else if (btnPrev == h)
				{
					if (!training) LoadPreviousProblem ();
				}
				else if (btnNext == h)
				{
					if (!training) LoadNextProblem ();
				}
				else if (btnFlip == h)
				{
					if (chessboard) chessboard->Flip ();
				}
				else if (btnPlay == h)
				{
					if (!training) ReplayMoves ();
				}
				else if (btnBack == h)
				{
					if (!training)
						if (chessboard && !worker.Busy ())
							chessboard->MoveBackward ();
				}
				else if (btnFrwd == h)
				{
					if (!training)
						if (chessboard && !worker.Busy ())
							chessboard->MoveForward ();
				}
				else if (btnStartSession == h)
				{
					training = trainer.StartSession ();
				}
				break;
		}
	}

	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
			case WM_MOUSEMOVE:
			{
				HWND focused = GetFocus ();
				if (focused != h && focused != edtIndex && focused != edtId) SetFocus (h);
			}	break;

			case WM_MOUSEWHEEL:
			{
				short delta = (short) HIWORD(w);
				if (delta > 0) LoadPreviousProblem ();
				else LoadNextProblem ();

			}	break;

			case WM_LBUTTONDOWN:
				//SetFocus (GetParent (handle));
				SetFocus (handle);
				break;

			case WM_MBUTTONDOWN:
			{
				chessboard->Flip();

				//database.Move (index);
				//Database2::Entry entry;
				//entry.Read (database);
			}	break;

			case WM_KEYDOWN:
			{
				switch((int)w)
				{
					case VK_F2:
					{
						chessboard->Flip();
					}	break;

					case VK_F3:
					{
						chessboard->RequestRandomProblem();
					}	break;

					case VK_UP:
					{
						OnChessboardWheelEvent ((short) +WHEEL_DELTA);
					}	break;

					case VK_DOWN:
					{
						OnChessboardWheelEvent ((short) -WHEEL_DELTA);
					}	break;

					case VK_LEFT:
					{
						chessboard->RequestPrevProblem();
					}	break;

					case VK_RIGHT:
					{
						chessboard->RequestNextProblem();
					}	break;
				}
			}	break;

			/*
			case WM_MBUTTONDOWN:
				//SetCapture (handle);
				SendMessage (GetParent (handle), WM_USER + 4711, 0, l);
				break;

			case WM_MBUTTONUP:
				//ReleaseCapture ();
				SendMessage (GetParent (handle), WM_USER + 4711, 1, l);
				break;
			*/

			case WM_CTLCOLORSTATIC:
				//return (LRESULT) GetStockObject (WHITE_BRUSH);
				//SetBkMode ((HDC) w, TRANSPARENT);
				//return (LRESULT) GetSysColorBrush (COLOR_WINDOW);
				return 0;
		}
		return Frame::Callback (h,m,w,l);
	}
};
