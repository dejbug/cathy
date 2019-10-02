#pragma once
#include "FileParser.h"
#include "../database/Database.h"
#include "ProblemSolutionFieldParser.h"

#include "../database/Database2.h"
#include "../database/Board2.h"
#include "../database/Position.h"
#include "../database/History.h"

class ProblemSolutionParserListener
{
public:
	typedef std::vector <ProblemSolutionParserListener *> Vector;

public:
	virtual void OnEntry (Database::Entry & entry) = 0;
};

class ProblemSolutionParserListener2 :
	public ProblemSolutionParserListener
{
public:
	typedef std::vector <ProblemSolutionParserListener2 *> Vector;

public:
	virtual void OnEntry2 (Database2::Entry & entry) = 0;
};

class ProblemSolutionParserListeners :
	public ProblemSolutionParserListener2::Vector,
	public ProblemSolutionParserListener2
{
public:
	virtual void OnEntry (Database::Entry & entry)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnEntry (entry);
	}

	virtual void OnEntry2 (Database2::Entry & entry)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnEntry2 (entry);
	}
};

class ProblemSolutionParser :
	public ProblemSolutionFieldParserListener2
{
private:
	ProblemSolutionParserListeners listeners;
	Database::Entry entry;

	Database2::Entry entry2;
	Position position;
	History history;

public:
	void Add (ProblemSolutionParserListener2 * listener) {
		listeners.push_back (listener);
	}

private:
	virtual void OnPiece (Board::Piece piece, Board::Index square)
	{
		entry.board.Add (piece, square);
	}

	virtual void OnMove (int index, Board::Index src, Board::Index dst)
	{
		entry.board.Set (index, src, dst);
	}

	virtual void OnColor (unsigned char color)
	{
		entry.color = color;

		entry2.header.color = color;
	}

	virtual void OnDone ()
	{
		//entry.Print ();
		listeners.OnEntry (entry);

		entry2.board = position;
		entry2.board = history;
		listeners.OnEntry2 (entry2);
	}

	virtual void OnPiece2 (Board2::Square square, Board2::Symbol symbol)
	{
		position.Set (square, symbol);
	}

	virtual void OnMove2 (int index, Board2::Square src, Board2::Square dst)
	{
		history.Set (index, src, dst);
	}
};
