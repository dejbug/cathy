#pragma once
#include <ctype.h>
#include "Board2.h"

class Board
{
public:
	typedef unsigned char Index;
	typedef enum { king,queen,rook,bishop,knight,pawn } Type;
	typedef enum {	K,Q,R1,R2,B1,B2,N1,N2,P1,P2,P3,P4,P5,P6,P7,P8,
					k,q,r1,r2,b1,b2,n1,n2,p1,p2,p3,p4,p5,p6,p7,p8} Piece;
/*
	static Piece PIECES [32];
	static char TOKENS [32];
*/
public:
	unsigned char data [54];

public:
	Board ()
	{
		memset (data, 64, sizeof (data));
	}

public:
	bool IsEmptyDataByte (unsigned char index) const
	{
		return index < 54 && data [index] >= 64;
	}

	static bool TranslateToken (const char token, Piece & piece)
	{
		switch (token)
		{
			default: return false;
			case 'K': piece = K; break;
			case 'Q': piece = Q; break;
			case 'R': piece = R1; break;
			case 'B': piece = B1; break;
			case 'N': piece = N1; break;
			case 'P': piece = P1; break;
			case 'k': piece = k; break;
			case 'q': piece = q; break;
			case 'r': piece = r1; break;
			case 'b': piece = b1; break;
			case 'n': piece = n1; break;
			case 'p': piece = p1; break;
		}
		return true;
	}

	static bool TranslatePiece (const Piece piece, char & token)
	{
		switch (piece)
		{
			default: return false;
			case K: token = 'K'; break;
			case Q: token = 'Q'; break;
			case R1: case R2: token = 'R'; break;
			case B1: case B2: token = 'B'; break;
			case N1: case N2: token = 'N'; break;
			case P1: case P2: case P3: case P4:
			case P5: case P6: case P7: case P8: token = 'P'; break;
			case k: token = 'k'; break;
			case q: token = 'q'; break;
			case r1: case r2: token = 'r'; break;
			case b1: case b2: token = 'b'; break;
			case n1: case n2: token = 'n'; break;
			case p1: case p2: case p3: case p4: 
			case p5: case p6: case p7: case p8: token = 'p'; break;
		}
		return true;
	}

	static bool TranslateCell (Index cell, int & file, int & rank)
	{
		rank = 8 - cell / 8;
		file = cell % 8;
		return true;
	}

	static bool IsIndexedPiece (Piece piece)
	{
		return !(K==piece || Q==piece || k==piece || q==piece);
	}

	static bool IsWhite (Piece piece) { return piece >= K && piece <= P8; }
	static bool IsKing (Piece piece) { return K==piece || k==piece; }
	static bool IsQueen (Piece piece) { return Q==piece || q==piece; }
	static bool IsRook (Piece piece) {
		return R1==piece || R2==piece || r1==piece || r2==piece;
	}
	static bool IsBishop (Piece piece) {
		return B1==piece || B2==piece || b1==piece || b2==piece;
	}
	static bool IsKnight (Piece piece) {
		return N1==piece || N2==piece || n1==piece || n2==piece;
	}
	static bool IsPawn (Piece piece) {
		return piece >= P1 && piece <= P8 || piece >= p1 && piece <= p8;
	}

	static bool GetPieceType (Piece piece, Type & type)
	{
		if (IsKing (piece)) return type = king, true;
		if (IsQueen (piece)) return type = queen, true;
		if (IsRook (piece)) return type = rook, true;
		if (IsBishop (piece)) return type = bishop, true;
		if (IsKnight (piece)) return type = knight, true;
		if (IsPawn (piece)) return type = pawn, true;
		return false;
	}

	static bool GetPieceBase (Piece piece, Piece & base)
	{
		if (IsKing (piece)) base = K;
		else if (IsQueen (piece)) base = Q;
		else if (IsRook (piece)) base = R1;
		else if (IsBishop (piece)) base = B1;
		else if (IsKnight (piece)) base = N1;
		else if (IsPawn (piece)) base = P1;
		else return false;
		if (!IsWhite (piece)) base = (Piece) ((int)base + 16);
		return true;
	}

	static int GetPieceIndexRange (Piece piece)
	{
		if (IsKing (piece) || IsQueen (piece)) return 1;
		if (IsRook (piece) || IsBishop (piece) || IsKnight (piece)) return 2;
		if (IsPawn (piece)) return 8;
		return 0;
	}

	static unsigned char GetPieceColor (Piece piece)
	{
		return IsWhite (piece) ? 0 : 1;
	}

public:
	bool Copy (const Board & board)
	{
		memcpy (data, board.data, 54);
		return true;
	}

	bool Flip ()
	{
		for (int i=0; i<54; i++)
			if (data [i] < 64)
				data [i] = 63-data[i];
		return true;
	}

public:
	bool Set (Piece piece, Index cell, int offset = 0)
	{
		return data [piece + offset] = cell, cell < 64;
	}

	bool Set (Index move, Index src, Index dst)
	{
		if (move >= 11) return false;
		if (src >= 64 || dst >= 64) return false;
		data [32 + (move<<1)] = src;
		data [32 + (move<<1) + 1] = dst;
		return true;
	}

	bool Add (Piece piece, Index cell)
	{
		int range = GetPieceIndexRange (piece);
		if (!range) return false;

		if (1==range) return Set (piece, cell);

		Piece base;
		GetPieceBase (piece, base);

		for (int i=base; i<base+range; i++)
			if (IsEmptyDataByte (i)) return data [i] = cell, true;
		
		return false;
	}
	
	bool Add (Index src, Index dst)
	{
		if (src >= 64 || dst >= 64) return false;
		for (int i=0; i<11; i++)
		{
			int srcx = 32 + (i<<1);
			int dstx = srcx + 1;
			//if (0==data [srcx] && 0==data [dstx])
			if (IsEmptyDataByte (srcx) && IsEmptyDataByte (dstx))
				return data [srcx] = src, data [dstx] = dst, true;
		}
		return false;
	}

	bool GetMove (Index move, Index & src, Index & dst) const
	{
		if (move > 11) return false;
		int index = 32 + (move<<1);
		src = data [index];
		dst = data [index+1];
		return src != dst && src >= 0 && src < 64 && dst >= 0 && dst < 64;
	}

public:
	void Unpack (unsigned char (& board) [64]) const
	{
		for (int i=0, c=*data; i<32; i++, c=data [i])
		{
			if (c<64)TranslatePiece ((Board::Piece) i, (char &) board [c]);
		}
	}

	void Print () const
	{
		unsigned char temp [64];
		memset (temp, 0, 64);

		/*
		memcpy (temp, data, 54);
		qsort (temp, 32, 1, sorter);
		for (int i=0; i<32; i++)
			if (temp[i]) printf ("%02d ", temp[i]);
		*/

		
		Unpack (temp);

		unsigned char color = -1;
		InferFirstMoveColor (color);

		Board::PrintFEN (temp, color);
		printf ("\n");
		Board::PrintSetup (temp);

		int correction = color ? 2 : 1;

		for (int i=0; i<11; i++)
		{
			if (!PrintMove (temp, i)) break;
			printf ("\n");
			Board::PrintFEN (temp, (++color)%2, i+correction);
			printf ("\n");
			Board::PrintSetup (temp);
		}

		printf ("\n");
	}

	static void PrintSetup (const unsigned char data [64])
	{
		printf ("{--------------");
		for (int i=0; i<64; i++)
		{
			if (i%8==0) printf ("\n");
			if (data[i]) printf ("%c ", data[i]);
			else printf (". ");
		}
		printf ("\n--------------}\n");
	}

	static void PrintFEN (const unsigned char data [64], unsigned char color, Index move = 0)
	{
		bool lastCellWasEmpty = false;
		int emptyCount = 0;

		for (int i=0, c=*data; i<64; i++, c=data[i])
		{
			if (i%8==0)
			{
				if (lastCellWasEmpty) printf ("%d", emptyCount);
				lastCellWasEmpty = false;
				emptyCount = 0;

				if (i>0 && i<63) printf ("/");
			}

			if (0 == c)
			{
				lastCellWasEmpty = true;
				++emptyCount;

				if (63==i) printf ("%d", emptyCount);
			}
			else
			{
				if (lastCellWasEmpty)
				{
					printf ("%d", emptyCount);
					lastCellWasEmpty = false;
					emptyCount = 0;
				}
				printf ("%c", c);
			}
		}

		int index = (move >> 1) + 1;
		printf (" %c - - 0 %d", color < 0 ? '?' : color ? 'b' : 'w', index);
	}

	bool InferFirstMoveColor (unsigned char & color) const
	{
		return InferMoveColor (0, color);
	}

	bool InferMoveColor (Index move, unsigned char & color) const
	{
		Index src, dst;
		if (!GetMove (move, src, dst)) return false;
		for (int i=0; i<32; i++)
			if (data [i] == src)
				return color = IsWhite ((Piece) i) ? 0 : 1, true;
		return false;
	}

	//bool PrintMove (const unsigned char data [64], Index move) const
	bool PrintMove (unsigned char data [64], Index move) const
	{
		Index src, dst;
		if (!GetMove (move, src, dst)) return false;
		if (0 == data [src]) return false;

		// Test with black moving first.
//		data [src] = 'b';

		int index = (move >> 1) + 1;

		printf ("%d.", index); 

		// Infer the first mover's color.
		Piece piece;
		TranslateToken (data [src], piece);
		if (!IsWhite (piece)) printf ("..");

		// Go on printing...
		 printf (" %c", toupper (data [src]));
		if (0 != data [dst]) printf ("x");
		
		int rank, file;
		TranslateCell (dst, file, rank);
		printf ("%c%d ", file+'a', rank);

		data [dst] = data [src];
		data [src] = 0;
		return true;
	}
/*
	static int sorter (const void * a, const void * b)
	{
		return *(unsigned char *)a - *(unsigned char *)b;
	}
*/

public:
	bool HasMoves () const
	{
		return !IsEmptyDataByte (32);
	}
};
/*
Board::Piece Board::PIECES [32] = {
	K,Q,R1,R2,B1,B2,N1,N2,P1,P2,P3,P4,P5,P6,P7,P8,
	k,q,r1,r2,b1,b2,n1,n2,p1,p2,p3,p4,p5,p6,p7,p8};
char Board::TOKENS [32] = {
	'K','Q','R','R','B','B','N','N','P','P','P','P','P','P','P','P',
	'k','q','r','r','b','b','n','n','p','p','p','p','p','p','p','p'};
*/