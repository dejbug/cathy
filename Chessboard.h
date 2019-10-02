#pragma once
#include "lib/Frame.h"
#include "lib/Surface.h"
#include "database/Database.h"
#include "Chessfont.h"

#include "database/Database2.h"
#include "database/Position.h"
#include "database/History.h"

#include "MoveHistory.h"
#include "Dragger.h"

/*
TODO: This class is too large! Clean this mess up!
	Create a ChessboardModel which holds the MoveHistory and board.
	Use the Chessboard class only for the painting. Delegate! Delegate!
*/

/// Square object. Holds the index and (token) contents of a square.
class SQUARE { public:
	unsigned char index, data;
	SQUARE () : index(0), data(0) {}
	void Reset () { index = data = 0; }
};

/// Piece object. Holds the type and color of a piece.
class PIECE { public:
	unsigned char type, color;
	PIECE () : type(0), color(0) {}
	void Reset () { type = color = 0; }
};

/// Helper object for dragging pieces.
class DRAGGER { public:
	HCURSOR cursor;	///< Handle of the currently dragged piece.
	bool dragging;	///< Is a piece currently being dragged?
	SQUARE square;	///< Square from which the piece was picked.
	PIECE piece;	///< Piece being dragged.
	
	DRAGGER () : cursor(0), dragging(false) { square.Reset (); }

	/// Set all attributes to default values.
	void Reset ()
	{
		dragging = false;
		square.Reset ();
		if (cursor) SetCursor (0), DestroyCursor (cursor), cursor = 0;
	}

	/// Store the cursor and activate it (i.e. set the system cursor to it).
	void Set (HCURSOR handle)
	{
		if (cursor) SetCursor (handle), DestroyCursor (cursor), cursor = handle;
		else SetCursor (cursor = handle);
	}
};


#define CHESSBOARD_LOAD_RANDOM_PROBLEM		0x00000001
#define CHESSBOARD_LOAD_PREVIOUS_PROBLEM	0x00000002
#define CHESSBOARD_LOAD_NEXT_PROBLEM		0x00000003


/// Interface to important chessboard events.
class ChessboardListener
{
public:
	typedef std::vector <ChessboardListener *> Vector;

public:
	/// Called when the mousewheel has been used over the chessboard.
	virtual void OnChessboardWheelEvent (short delta) = 0;
	/// Called when the user attempts to move a piece.
	virtual void OnChessboardUserMove (unsigned char src, unsigned char dst, bool ok) = 0;
	/// Called when the chessboard has been flipped.
	virtual void OnChessboardFlipped (bool inverted) = 0;
	/// Called when a position has been successfully set up.
	virtual void OnChessboardLoaded (const Database2::Entry & entry) = 0;
	/// Called after the current side has made its ply.
	virtual void OnChessboardTurnChange (unsigned char color) = 0;
	// Called when certain user input is caught over chessboard.
	virtual void OnChessboardUserInput (unsigned int key, unsigned int value) = 0;
};

#define CHESSBOARD_NOTIFY(call) for (iterator it=begin(); it!=end(); it++) (*it)->call

/// Helper class for managing chessboard listeners.
class ChessboardListeners :
	public ChessboardListener::Vector,
	public ChessboardListener
{
public:
	virtual void OnChessboardWheelEvent (short delta)
	{
		CHESSBOARD_NOTIFY (OnChessboardWheelEvent (delta));
	}

	virtual void OnChessboardUserMove (unsigned char src, unsigned char dst, bool ok)
	{
		CHESSBOARD_NOTIFY (OnChessboardUserMove (src, dst, ok));
	}

	virtual void OnChessboardFlipped (bool inverted)
	{
		CHESSBOARD_NOTIFY (OnChessboardFlipped (inverted));
	}

	virtual void OnChessboardLoaded (const Database2::Entry & entry)
	{
		CHESSBOARD_NOTIFY (OnChessboardLoaded (entry));
	}

	virtual void OnChessboardTurnChange (unsigned char color)
	{
		CHESSBOARD_NOTIFY (OnChessboardTurnChange (color));
	}

	virtual void OnChessboardUserInput (unsigned int key, unsigned int value)
	{
		CHESSBOARD_NOTIFY (OnChessboardUserInput (key, value));
	}
};

/**
	The chessboard control.
*/
class Chessboard : public lib::gui::Frame
{
private:
	ChessboardListeners listeners;
	lib::gdi::Surface surface;
	Chessfont font;
	int cellsize, offx, offy;
	unsigned char board [64];
	bool flipped;
	MoveHistory history;
	DRAGGER dragger;

	/// The player's color in the currently loaded position.
	unsigned char playerColor;
	/// Should listeners be notified about board flips?
	bool notifyFlips;
	/// Should board be automatically flipped so that the player's
	/// color is always at the bottom?
	bool autoFlip;
	bool noDragAfterLast;
	bool noDragging;
	bool noOpponentDrag;

	char currentlyCompletedCount;
	char currentlyShownIndex;

public:
	Chessboard () : font(),
		cellsize(0), offx(0), offy(0), flipped(false),
		playerColor(0),
		notifyFlips(true),
		autoFlip(true),
		noDragAfterLast(true), noDragging(false), noOpponentDrag(true),
		currentlyCompletedCount(0), currentlyShownIndex(0)
	{
		memset (board, 0, 64);
		font.Install ("Alpha.ttf");
	}

	~Chessboard ()
	{
		font.Uninstall ();
	}

	bool Create (HWND parent, UINT id, DWORD style = WS_VISIBLE, DWORD exstyle = 0)
	{
		return Frame::Create (parent, "CHESSBOARD", id,
			WS_CHILD|style, exstyle, false);
	}

	void AddListener (ChessboardListener * listener)
	{
		listeners.push_back (listener);
	}

protected:
	bool GetPieceFromPoint (short x, short y, SQUARE & square, PIECE & piece)
	{
		GetCellIndexFromPoint (square.index, x, y);
		square.data = GetCellData (square.index);
		if (0 == square.data) return false;
		TranslateToken (square.data, piece.type, piece.color);
		return true;
	}

	bool TakePieceFromPoint (short x, short y, SQUARE & square, PIECE & piece)
	{
		if (!GetPieceFromPoint (x, y, square, piece)) return false;

		// May the piece be taken by the player?
		if (noOpponentDrag && piece.color == playerColor) return false;

		SetCellData (square.index, 0);
		Invalidate ();
		return true;
	}

	bool DragPiece (short x, short y)
	{
		return TakePieceFromPoint (x, y, dragger.square, dragger.piece);
	}

	bool DropPiece ()
	{
		if (!dragger.dragging) return false;
		ReleaseCapture ();
		SetCellData (dragger.square.index, dragger.square.data);
		dragger.Reset ();
		Invalidate ();
		return true;
	}

protected:
	static bool TranslateToken (char token, unsigned char & piece, unsigned char & color)
	{
		switch (token)
		{
			default: piece=0; color=0; return false;
			case 'K': piece=0; color=0; break;
			case 'Q': piece=1; color=0; break;
			case 'R': piece=2; color=0; break;
			case 'B': piece=3; color=0; break;
			case 'N': piece=4; color=0; break;
			case 'P': piece=5; color=0; break;

			case 'k': piece=6; color=1; break;
			case 'q': piece=7; color=1; break;
			case 'r': piece=8; color=1; break;
			case 'b': piece=9; color=1; break;
			case 'n': piece=10; color=1; break;
			case 'p': piece=11; color=1; break;
		}
		return true;
	}

	static void PaintBoard (HDC hdc, int x, int y, int size)
	{
		COLORREF dark = RGB(119,162,109);
		COLORREF bright = RGB(200,195,101);//RGB(180,175,101);
		COLORREF darkhatch = RGB(119,162,250);
		COLORREF brighthatch = RGB(200,195,0);

		HBRUSH brush [4];
		brush [0] = CreateSolidBrush (dark);
		brush [1] = CreateSolidBrush (bright);
		brush [2] = CreateHatchBrush (HS_BDIAGONAL, darkhatch);
		brush [3] = CreateHatchBrush (HS_FDIAGONAL, brighthatch);

		SelectStockObject (hdc, BLACK_PEN);
		SetROP2 (hdc, R2_COPYPEN);

		for (int index=0; index<64; index++)
		{
			int i = index % 8;
			int j = index / 8;
			int c = (i+j+1)%2;

			SelectObject (hdc, brush [c]);

			Rectangle (hdc, x+i*size, y+j*size, x+i*size+size, y+j*size+size);
			
			//SelectObject (hdc, brush [c+2]);
			//Rectangle (hdc, x*size, y*size, x*size+size, y*size+size);
		}

		for (int i=0; i<sizeof (brush)/sizeof (*brush); i++)
			DeleteObject (brush [i]);
	}

	static void PaintPiece (HDC hdc, int index, int color, int x, int y, int size)
	{
		static char fg [7] = "kqrbhp";
		static char bg [7] = "lwtnjo";

		index %= 6;
		color %= 2;

		SetBkMode (hdc, TRANSPARENT);

		if (color) {
			SetTextColor (hdc, RGB(0,0,0));
			TextOut (hdc, x, y, &bg[index], 1);
			SetTextColor (hdc, RGB(225,225,255));
			TextOut (hdc, x, y, &fg[index], 1);
		}
		else {
			SetTextColor (hdc, RGB(255,255,255));
			TextOut (hdc, x, y, &bg[index], 1);
			SetTextColor (hdc, RGB(0,0,0));
			TextOut (hdc, x, y, &fg[index], 1);
		}
	}

	static void PaintPieces (HDC hdc, const unsigned char board [64], int x, int y, int size, bool flipped = false)
	{
		for (int index=0; index<64; index++)
		{
			int realindex = flipped ? 63-index : index;

			int i = index % 8;
			int j = index / 8;

			unsigned char piece, color;
			if (TranslateToken (board [index], piece, color))
				PaintPiece (hdc, piece, color, x+i*size, y+j*size, size);
		}
	}

public:
	bool GetCellIndexFromPoint (unsigned char & index, int x, int y)
	{
		if (x < 0 || y < 0 || x > (cellsize<<3) || y > (cellsize<<3)) return false;
		index = ((y/cellsize)<<3) + x/cellsize;
		index = flipped ? 63-index : index;
		return index >= 0 && index < 64;
	}

	void SetCellData (unsigned char index, unsigned char data)
	{
		board [flipped ? 63-index : index] = data;
	}

	unsigned char GetCellData (unsigned char index) const
	{
		return board [flipped ? 63-index : index];
	}

protected:
	void CompleteHistoryMove (HistoryMove & move)
	{
		move.moved = GetCellData (move.src);
		move.taken = GetCellData (move.dst);

		move.HandleCastlingMove ();
		move.HandleEnPassant ();
		move.HandlePromotion ();

		move.complete = true;
	}

	void CommitHistoryMove (const HistoryMove & move)
	{
		SetCellData (move.dst, move.moved);
		SetCellData (move.src, 0);

		if (move.castling)
		{
			SetCellData (move.rookdst, move.rook);
			SetCellData (move.rooksrc, 0);
		}

		else if (move.enpassant)
		{
			SetCellData (move.pawnsqr, 0);
		}

		else if (move.promoted)
		{
			SetCellData (move.dst, move.promotion);
		}
	}

	void RevertHistoryMove (const HistoryMove & move)
	{
		SetCellData (move.dst, move.taken);
		SetCellData (move.src, move.moved);

		if (move.castling)
		{
			SetCellData (move.rooksrc, move.rook);
			SetCellData (move.rookdst, 0);
		}

		else if (move.enpassant)
		{
			SetCellData (move.pawnsqr, move.pawn);
		}

		else if (move.promoted)
		{
			// Nothing special needs to be done.
		}
	}

public:
	bool HasMoreMoves () const
	{
		return history.More ();
	}

	size_t GetPlyCount () const
	{
		return history.Count ();
	}

	bool GetCurrentTurn (unsigned char & color) const
	{
		HistoryMove move;
		if (!history.Current (move)) return false;
		unsigned char data;
		if (move.complete) data = move.moved;
		else data = GetCellData (move.src);
		color = islower (data) ? 1 : 0;
		return true;
	}

public:
/*
	void SetupPosition (unsigned char (& board) [64])
	{
		flipped = false;
		memcpy (this->board, board, 64);
		history.Clear ();
		Invalidate ();

		//unsigned char currentTurn; GetCurrentTurn (currentTurn);
		//listeners.OnChessboardLoaded (currentTurn);
	}
*/
	void SetupPosition (const Database2::Entry & entry)
	{
		currentlyCompletedCount = 0;
		currentlyShownIndex = 0;

		playerColor = entry.header.color;

		Position position = entry.board;
		memcpy (board, position.GetDataPointer (), 64);

		History history = entry.board;

		this->history.Clear ();
		flipped = false;

		for (int i=0; i<entry.header.moves<<1; i++)
		{
			unsigned char src, dst;
			history.Get (i, src, dst);

			HistoryMove move (src, dst);			
			this->history.Push (move);
		}

		notifyFlips = false;
		SetFlipped (false);
		if (autoFlip && 0 == playerColor) Flip ();
		notifyFlips = true;

		Invalidate ();

		//unsigned char currentTurn; GetCurrentTurn (currentTurn);
		//listeners.OnChessboardLoaded (currentTurn);
		listeners.OnChessboardLoaded (entry);
	}

	bool MoveToFirstIncompleteMove (HistoryMove & move)
	{
		while (move.complete)
			if (!history.Next (move))
				return false;
		return true;
	}

	/*
	bool MoveToFirstIncompleteMove ()
	{
		MoveHistory::iterator it = history.begin ();
		for (; it != history.end (); it++)
			if (!it->complete)
				return true;
		return false;
	}
	*/

	bool MoveToLastCompletedMove ()
	{
		for (char i=currentlyShownIndex; i<currentlyCompletedCount; ++i)
			if (!MoveForward ()) return false;
		return true;
	}

	bool MoveForward (bool completed = false, bool appending = false, bool update = true)
	{
		DropPiece ();

		HistoryMove move;
		if (!history.Next (move)) return false;

		if (appending && !MoveToFirstIncompleteMove (move)) return false;

		if (!move.complete)
		{
			if (completed) return history.Previous (move), false;

			currentlyCompletedCount = history.GetCurrentIndex ();
			currentlyShownIndex = currentlyCompletedCount;

			CompleteHistoryMove (move);
			history.Replace (history.GetCurrentIndex (), move);
		}

		CommitHistoryMove (move);

		if (update) Invalidate ();

		unsigned char turnColor = islower (move.moved) ? 1 : 0;
		listeners.OnChessboardTurnChange (turnColor);

		return true;
	}

	bool MoveBackward (bool update = true)
	{
		// Just in case the user is dragging. Or we'd mess up the board.
		DropPiece ();

		HistoryMove move;
		if (!history.Previous (move)) return false;

		currentlyShownIndex = __max (0, currentlyShownIndex);

		RevertHistoryMove (move);

		if (update) Invalidate ();

		unsigned char turnColor = isupper (move.moved) ? 1 : 0;
		listeners.OnChessboardTurnChange (turnColor);

		return true;
	}

	void RewindHistory ()
	{
		for (int i=0; i<history.Count (); i++)
			MoveBackward (false);
		Invalidate ();
	}


	void SetFlipped (bool flipped)
	{
		this->flipped = flipped;
		if (notifyFlips) listeners.OnChessboardFlipped (flipped);
	}

	void Flip ()
	{
		unsigned char temp;

		for (int i=0; i<32; i++)
		{
			temp = board [i];
			board [i] = board [63-i];
			board [63-i] = temp;
		}
		
		SetFlipped (!flipped);

		Invalidate ();
	}

	void SetDraggingEnabled (bool enabled)
	{
		noDragging = !enabled;
	}

	void RequestRandomProblem()
	{
		if (notifyFlips) listeners.OnChessboardUserInput (CHESSBOARD_LOAD_RANDOM_PROBLEM, 0);
	}

	void RequestPrevProblem()
	{
		if (notifyFlips) listeners.OnChessboardUserInput (CHESSBOARD_LOAD_PREVIOUS_PROBLEM, 0);
	}

	void RequestNextProblem()
	{
		if (notifyFlips) listeners.OnChessboardUserInput (CHESSBOARD_LOAD_NEXT_PROBLEM, 0);
	}

private:
	virtual void OnCreate (HWND handle)
	{
		surface.Create (handle);
		/*
		unsigned char initialPosition [64] = {
			'r','n','b','q','k','b','n','r',
			'p','p','p','p','p','p','p','p',
			  0,  0,  0,  0,  0,  0,  0,  0,
			  0,  0,  0,  0,  0,  0,  0,  0,
			  0,  0,  0,  0,  0,  0,  0,  0,
			  0,  0,  0,  0,  0,  0,  0,  0,
			'P','P','P','P','P','P','P','P',
			'R','N','B','Q','K','B','N','R'
		};

		SetupPosition (initialPosition);
		*/
	}

	virtual void OnPaint (HDC hdc)
	{
		surface.Select (font);
		PaintBoard (surface, offx, offy, cellsize);
		PaintPieces (surface, board, offx, offy, cellsize, flipped);
		surface.Flip ();
	}

	virtual void OnSize (DWORD flags, WORD cx, WORD cy)
	{
		cellsize = (cx < cy ? cx : cy) >> 3;
		
		offx = (cx - (cellsize<<3)) >> 1;
		offy = (cy - (cellsize<<3)) >> 1;

		font.Resize (cellsize);
		surface.Resize (cx, cy);

		surface.Select (font);
	}

	virtual LRESULT CALLBACK Callback (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
			case WM_ERASEBKGND: return 1;

			case WM_MOUSEMOVE: if (GetFocus () != h) SetFocus (h); break;

#ifdef _DEBUG
			case WM_RBUTTONDOWN:
			{
				unsigned char index;
				GetCellIndexFromPoint (index, LOWORD(l), HIWORD(l));
				lib::gui::Frame parent = GetParentFrame ();
				parent.SetFormattedText ("Square: %d", index);
			}	break;
#endif

			case WM_MOUSEWHEEL:
			{
				listeners.OnChessboardWheelEvent ((short) HIWORD (w));
			}	break;

			case WM_KEYDOWN:
			{
				switch((int)w)
				{
					case VK_F2:
					{
						Flip();
					}	break;

					case VK_F3:
					{
						RequestRandomProblem();
					}	break;

					case VK_UP:
					{
						listeners.OnChessboardWheelEvent ((short) +WHEEL_DELTA);
					}	break;

					case VK_DOWN:
					{
						listeners.OnChessboardWheelEvent ((short) -WHEEL_DELTA);
					}	break;

					case VK_LEFT:
					{
						RequestPrevProblem();
					}	break;

					case VK_RIGHT:
					{
						RequestNextProblem();
					}	break;
				}
			}	break;

			case WM_MBUTTONDOWN:
			{
				RequestRandomProblem();
			}	break;

			case WM_LBUTTONDOWN:
			{
				if (noDragging) break;

				if (noDragAfterLast && !history.More ()) break;

				if (!DragPiece (LOWORD(l), HIWORD(l))) break;

				COLORREF transparentColor = RGB(192,192,192);
				lib::gdi::Surface surface;
				PrepareCursorSurface (surface, transparentColor);
				PaintPiece (surface, dragger.piece.type, dragger.piece.color, 0, 0, cellsize);

				SetCapture (handle);

				HICON cursor = lib::gui::Cursor::CreateCursorFromBitmap (surface, surface, transparentColor, cellsize);
				dragger.Set (cursor);
				dragger.dragging = true;
			}	break;

			case WM_LBUTTONUP:
			{
				if (dragger.dragging)
				{
					unsigned char dstCellIndex;
					GetCellIndexFromPoint (dstCellIndex, LOWORD(l), HIWORD(l));

					bool ok;
					CheckUserMove (dstCellIndex, ok);

					listeners.OnChessboardUserMove (dragger.square.index, dstCellIndex, ok);
				}
				DropPiece ();
			}	break;

		}
		return Frame::Callback (h,m,w,l);
	}

protected:
	void PrepareCursorSurface (lib::gdi::Surface & surface, COLORREF transparentColor)
	{
		surface.Create (handle);
		surface.Resize (cellsize, cellsize);
		surface.Clear (transparentColor);
		surface.Select (font);
	}

	bool CheckUserMove (unsigned char dstCellIndex, bool & ok)
	{
		ok = false;

		if (!dragger.dragging) return false;

		HistoryMove move;
		if (!history.Current (move)) return false;

		if (move.src == dragger.square.index &&
			move.dst == dstCellIndex)
		{
			ok = true;
			MoveForward ();
		}

		return true;
	}
};
