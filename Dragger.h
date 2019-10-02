#pragma once
#include "lib/Cursor.h"

template <class Chessboard> class Checker
{
private:
	Chessboard * chessboard;

public:
	Checker () : chessboard(0) {}

public:
	void Create (Chessboard * chessboard)
	{
		this->chessboard = chessboard;
	}

	bool CheckUserMove (unsigned char dstCellIndex)
	{
		return chessboard->CheckUserMove (dstCellIndex);
	}
};

/*

class ChessboardModel
{
public:
	typedef struct {
		HWND handle;
		Chessfont * font;
		int cellsize;
	} CHESSBOARDVIEWINFO;

public:
	virtual bool GetCellIndexFromPoint (unsigned char & index, int x, int y) = 0;
	virtual unsigned char GetCellData (unsigned char index) = 0;
	virtual void SetCellData (unsigned char index, unsigned char data) = 0;
	virtual bool TranslateToken (char token, unsigned char & piece, unsigned char & color) = 0;

	virtual void GetPiece (unsigned index, unsigned char & piece, unsigned char & color) = 0;
	virtual void SetPiece (unsigned index, unsigned char & piece, unsigned char & color) = 0;
	virtual void GetWindowInfo (CHESSBOARDVIEWINFO & info) = 0;
	virtual void Invalidate () = 0;
};

class Dragger
{
private:
	ChessboardModel * model;
	bool dragging;
	unsigned char draggedCellIndex;
	unsigned char draggedCellData;

public:
	Dragger () : model(0), dragging(false),
		draggedCellIndex(0), draggedCellData(0)
	{
	}

	void Attach (ChessboardModel * model)
	{
		this->model = model;
	}

public:
	bool DragPieceAtPoint (short x, short y)
	{
		unsigned char piece, color;

		model->GetCellIndexFromPoint (draggedCellIndex, x, y);
		draggedCellData = model->GetCellData (draggedCellIndex);

		if (0 == draggedCellData) return false;

		model->TranslateToken (draggedCellData, piece, color);
		
		model->SetCellData (draggedCellIndex, 0);
		model->Invalidate ();

		COLORREF transparentcolor = RGB(192,192,192);
		Surface surface;
		surface.Create (handle);
		surface.Resize (cellsize, cellsize);
		surface.Clear (transparentcolor);
		surface.Select (font);

		PaintPiece (surface, piece, color, 0, 0, cellsize);

		HICON temp = Cursor::CreateCursorFromBitmap (surface, surface, transparentcolor, cellsize);
		if (cursor) SetCursor (temp), DestroyCursor (cursor), cursor = temp;
		else SetCursor (cursor = temp);

		SetCapture (handle);
		dragging = true;

		return dragging;
	}

	bool DropPiece ()
	{
		if (dragging)
		{
			dragging = false;

			ReleaseCapture ();

			if (cursor) DestroyCursor (cursor), cursor = 0;

			model->SetCellData (draggedCellIndex, draggedCellData);
		}

		model->Invalidate ();

		return true;
	}
};

*/