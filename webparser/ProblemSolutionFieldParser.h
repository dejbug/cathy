#pragma once
#include <vector>
#include "../database/Board.h"
#include "ProblemSolutionTokenParser.h"

class ProblemSolutionFieldParserListener
{
public:
	typedef std::vector <ProblemSolutionFieldParserListener *> Vector;

public:
	virtual void OnPiece (Board::Piece piece, Board::Index square) = 0;
	virtual void OnMove (int index, Board::Index src, Board::Index dst) = 0;
	virtual void OnColor (unsigned char color) = 0;
	virtual void OnDone () = 0;
};

class ProblemSolutionFieldParserListener2 :
	public ProblemSolutionFieldParserListener
{
public:
	typedef std::vector <ProblemSolutionFieldParserListener2 *> Vector;

public:
	virtual void OnPiece2 (Board2::Square square, Board2::Symbol symbol) = 0;
	virtual void OnMove2 (int index, Board2::Square src, Board2::Square dst) = 0;
};

class ProblemSolutionFieldParserListeners :
	public ProblemSolutionFieldParserListener2::Vector,
	public ProblemSolutionFieldParserListener2
{
public:
	virtual void OnPiece (Board::Piece piece, Board::Index square)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnPiece (piece, square);
	}

	virtual void OnMove (int index, Board::Index src, Board::Index dst)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnMove (index, src, dst);
	}

	virtual void OnColor (unsigned char color)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnColor (color);
	}

	virtual void OnDone ()
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnDone ();
	}

	virtual void OnPiece2 (Board2::Square square, Board2::Symbol symbol)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnPiece2 (square, symbol);
	}

	virtual void OnMove2 (int index, Board2::Square src, Board2::Square dst)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnMove2 (index, src, dst);
	}
};

class ProblemSolutionFieldParser :
	public ProblemSolutionTokenParserListener
{
private:
	ProblemSolutionFieldParserListeners listeners;

public:
	void Add (ProblemSolutionFieldParserListener2 * listener) {
		listeners.push_back (listener);
	}

protected:
	bool AnalyseBoard (const char * text, bool flipped)
	{
		if (!text || 64!=strlen(text)) return false;

		Board::Piece piece;

		for (int i=0; i<64; i++)
		{
			int c = flipped ? text[64-1-i] : text[i];
			if (Board::TranslateToken (c, piece))
				listeners.OnPiece (piece, i);

			listeners.OnPiece2 (i, text [i]);
		}
		return true;
	}

	bool CorrectMove (Board::Index & move, bool flipped)
	{
		if (flipped) move = 63-move;
		return true;
	}

	bool IsFlipped (unsigned char color)
	{
		return 'b'==color || 1==color;
	}

private:
/*
FIXME [done]: The flipping of board positions is wrong, since the
positions are already in the proper orientation (white player at the
bottom) and are flipped on the webpage as neccessary (computer on the
top) at run-time.

What i've thought is that the board positions on the webpage are stored
as dispayed. But as it turns out, the corrections are unneccessary and,
moreover, falsifications.

To fix it, just remove all conditional move corrections. I.e. the
AnalyseBoard () function shouldn't bother with "flipped" positions.
*/
	virtual void OnToken (int index, const char * text)
	{
		static unsigned char color = 0;
		static bool flipped = false;
		static Board::Index src = 0, dst = 0;

		switch (0xF & index)
		{
			case 0:
				color = 'b'==*text ? 1 : 0;

//FIX BEGIN _______________________________________________________
//INFO: 'flipped' is always false! The position as it is found on
// the webpage is always correctly orientated (i.e. white on the
// bottom).
//
//TODO: Clean up all the 'flipping' mechanism. It's a mess! :)
//				flipped = IsFlipped (color);
				flipped = false;
//FIX END __________________________________________________________
				listeners.OnColor (color);
				break;
			case 1: AnalyseBoard (text, flipped); break;
			case 2:
				src= atoi (text);
				CorrectMove (src, flipped);
				break;
			case 3:
				dst = atoi (text);
				CorrectMove (dst, flipped);
				listeners.OnMove (index>>8, src, dst);
				listeners.OnMove2 (index>>8, src, dst);
				break;
			case 4: listeners.OnDone (); break;
		}
		//printf ("%d. %s\n", index, text);
	}
};
