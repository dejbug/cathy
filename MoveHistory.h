#pragma once
#include <vector>

#include "Stack.h"

class HistoryMove
{
public:
	typedef std::vector <HistoryMove> Vector;

public:
	bool complete;
	unsigned char src;
	unsigned char dst;
	unsigned char moved;
	unsigned char taken;
	bool enpassant;
	unsigned char pawn;
	unsigned char pawnsqr;
	bool castling;
	unsigned char rook;
	unsigned char rooksrc;
	unsigned char rookdst;
	bool promoted;
	unsigned char promotion;

public:
	void Print ()
	{
		char buffer [128];
		sprintf (buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d",
			complete, src, dst, moved, taken,
			enpassant, pawn, pawnsqr,
			castling, rooksrc, rookdst,
			promoted, promotion);
		OutputDebugString (buffer);
	}

public:
	HistoryMove (unsigned char src = 0, unsigned char dst = 0) :
		complete(false), src(src), dst(dst), moved(0), taken(0),
		enpassant(false), pawn(0), pawnsqr(0),
		castling(false), rook(0), rooksrc(0), rookdst(0),
		promoted(false), promotion(0)
		
	{
	}

public:
	bool HandleCastlingMove ()
	{
		// Black king AND on original square?
		if ('k' == moved && 4 == src)
		{
			rook = 'r';

			// Moves two squares to the right?
			if (6 == dst)
			{
				castling = true;

				rooksrc = 7;
				rookdst = 5;
			}
			// Moves two squares to the left?
			else if (2 == dst)
			{
				castling = true;

				rooksrc = 0;
				rookdst = 3;
			}
		}
		// White king AND on original square?
		else if ('K' == moved && 60 == src)
		{
			rook = 'R';

			// Moves two squares to the right?
			if (62 == dst)
			{
				castling = true;

				rooksrc = 63;
				rookdst = 61;
			}
			// Moves two squares to the left?
			else if (58 == dst)
			{
				castling = true;

				rooksrc = 56;
				rookdst = 59;
			}
		}
		return castling;
	}

	bool HandleEnPassant ()
	{
		if (0 != taken) return false;

		// White pawn move AND on fifth rank?
		if ('P' == moved && src >= 24 && src < 32)
		{
			pawn = 'P';
			
			// Take to the right.
			if (src-7 == dst)
			{
				enpassant = true;
				pawnsqr = src + 1;
			}
			// Take to the left.
			else if (src-9 == dst)
			{
				enpassant = true;
				pawnsqr = src - 1;
			}
		}
		// Black pawn move AND on fourth rank?
		else if ('p' == moved && src >= 32 && src < 48)
		{
			pawn = 'p';

			// Take to left.
			if (src+7 == dst)
			{
				enpassant = true;
				pawnsqr = src - 1;
			}
			// Take to right.
			else if (src+9 == dst)
			{
				enpassant = true;
				pawnsqr = src + 1;
			}
		}
		return enpassant;
	}

	bool HandlePromotion ()
	{
		if ('P' == moved && src >= 8 && src < 16)
		{
			promoted = true;
			promotion = 'Q';
		}

		else if ('p' == moved && src >= 48 && src < 56)
		{
			promoted = true;
			promotion = 'q';
		}

		return promoted;
	}
};

class MoveHistory : public Stack2 <HistoryMove>
{
};
