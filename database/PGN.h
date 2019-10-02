#pragma once

/*
	To print a move in algebraic notation:

	1. Pawn moves.

	1.1. Is the destination square on the same file?

	1.1.N. NO -> PRINT "x" and goto 1.1.Y.

	1.1.Y. YES -> PRINT destination square coordinates.
		Is the rank the eighth or first?

	1.1.Y.N. NO -> goto DONE.

	1.1.Y.Y. YES -> PRINT "=" and append promoted piece's symbol.
		Then goto 2.3.

	2. Piece moves. Start with printing the piece symbol.

	2.1. Can any other piece of the same color and type
		move to the same square?

	2.1.N. NO -> goto 2.2.
	2.1.Y. YES

	2.1.Y.1 Of these that can, are either all not
		on the same file or all not on the same rank?

	2.1.Y.1.Y. YES -> Use the modality (file or rank)
		which is different and append it to the piece symbol.

	2.1.Y.1.N. NO -> If both modalities are shared, append both.

	2.1.Y.1.E. Examples

	2.1.Y.1.E.1. Knights on a1, c1 and d4 can take b3, two share
		the same rank, but all have different files, use files, i.e.:
		"Naxb3" or "Ncxb3" or "Ndxb3".

	2.1.Y.1.E.2. Knights on a1, c1, and a5 can take b3. Two share
		ranks, two share files, so both have to be appended in order
		to identify the piece uniquely, i.e.: "Na1xb3" or "Nc1xb3" or
		"Na5xb3".

	2.2. Was a piece on the destination square?

	2.2.N. NO -> goto 2.2.1
	2.2.Y. YES -> Append an "x" to the string so far, then goto 2.2.1.

	2.2.1. Append the destination square's coordinates (e.g. "b3").

	2.3. Is the enemy king in check after the move?

	2.3.N. NO -> goto DONE.

	2.3.Y. YES -> Can he move out of the check?

	2.3.Y.Y. YES -> Append a "+" and goto DONE.

	2.3.Y.N. NO -> Can a piece block the check?

	2.3.Y.N.Y. YES -> goto 2.3.Y.Y.

	2.3.Y.N.N. NO -> Can the checking piece be taken?

	2.3.Y.N.N.Y. YES -> goto 2.3.Y.Y.

	2.3.Y.N.N.N. NO -> Append a "#" and goto DONE.

	DONE.
*/


bool GetProblem (unsigned short index, Database2::Entry & entry)
{
	Database2 database;
	if (!database.Open ("base.cts"))
		return false;

	if (!database.Move (index)) return false;
	if (!entry.Read (database)) return false;

	return true;
}

bool GetPosition (unsigned short index, Position & position)
{
	Database2::Entry entry;
	if (!GetProblem (index, entry)) return false;
	return position = entry.board;
}

bool GetHistory (unsigned short index, History & history)
{
	Database2::Entry entry;
	if (!GetProblem (index, entry)) return false;
	return history = entry.board;
}

class FileRank
{
public:
	char file;
	char rank;

public:
	FileRank (Board2::Square square)
	{
		file = 'a' + (square % 8);
		rank = 8 - (square >> 3);
	}

	void Print () const { printf ("%c%d", file, rank); }
};

bool PrintAlgebraic (Position & position, Board2::Square src, Board2::Square dst)
{
	char srcPiece, dstPiece;

	srcPiece = position [src];
	dstPiece = position [dst];

	position [dst] = position [src];
	position [src] = 0;

	FileRank a (src), b (dst);

	if (srcPiece) printf ("%c", toupper(srcPiece));
	a.Print ();

	printf (" -> ");
	
	if (dstPiece) printf ("%c", toupper(dstPiece));
	b.Print ();

	printf ("\n");
	return true;
}

void PrintAlgebraic (Position & position, const History & history)
{
	Board2::Square src, dst;
	for (int i=0; history.Get (i, src, dst); i++)
	{
		PrintAlgebraic (position, src, dst);
	}
}
