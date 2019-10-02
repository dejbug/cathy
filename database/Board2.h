#pragma once

/*
__________________________________________________________________
RECORD: HEADER
--------------------------
ID: 0-65535 (16 bit)
POS: 0-65535 (16 bit)
RATING: 1-3500 (12 bit) (should get extended to 14 bit)*
RD: 0-350 (9 bit) (should get extended to 12 bit)*
History: 1-16 (4 bit) (should get extended to 5 bit)
COLOR: 0-1 (1 bit)

[PADDING: 5 bits]
--------------------------
16 + 16 + 12 + 9 + 4 + 1 = 32 + 26 = 58 (< 64 bits = 8 bytes).
--------------------------
(*) the (more or less arbitrarily) marked fields are extended
so the padding bits are used up with something useful (allowing
the chosen fields to store values of a greater range) rather than
waiting for some other way of using them seperately (until when
the whole database module would most probably have already been
reworked a few times).
__________________________________________________________________

__________________________________________________________________
RECORD: POSITION
--------------------------
CELL[0-63]: 4 bit
--------------------------
64 * 4 = 256 bits = 32 bytes.
__________________________________________________________________

__________________________________________________________________
RECORD: History
--------------------------
MOVE[0-31]: 6 bit
--------------------------
32 * 6 = 192 bits = 24 bytes.
__________________________________________________________________

Total RECORD Size : 8 + 32 + 24 = 64 bytes.
*/

class Board2
{
public:
	typedef char Symbol;
	typedef unsigned char Square;
	typedef unsigned char Token;

	typedef Symbol Position [64];
	typedef unsigned char CompressedPosition [32];
	typedef Square History [32];
	typedef unsigned char CompressedHistory [24];

private:
	CompressedPosition position;
	CompressedHistory history;

public:
	static bool GetPieceToken (Symbol symbol, Token & token)
	{
		switch (symbol)
		{
			default: token = 0; return false;
			case 'K': token = 1; break;
			case 'Q': token = 2; break;
			case 'R': token = 3; break;
			case 'B': token = 4; break;
			case 'N': token = 5; break;
			case 'P': token = 6; break;
			case 'k': token = 7; break;
			case 'q': token = 8; break;
			case 'r': token = 9; break;
			case 'b': token = 10; break;
			case 'n': token = 11; break;
			case 'p': token = 12; break;
		}
		return true;
	}

	static bool GetPieceSymbol (Token token, Symbol & symbol)
	{
		switch (token)
		{
			default: symbol = 0; return false;
			case 1: symbol = 'K'; break;
			case 2: symbol = 'Q'; break;
			case 3: symbol = 'R'; break;
			case 4: symbol = 'B'; break;
			case 5: symbol = 'N'; break;
			case 6: symbol = 'P'; break;
			case 7: symbol = 'k'; break;
			case 8: symbol = 'q'; break;
			case 9: symbol = 'r'; break;
			case 10: symbol = 'b'; break;
			case 11: symbol = 'n'; break;
			case 12: symbol = 'p'; break;
		}
		return true;
	}

public:
	static bool CompressPosition (const Symbol in [64], CompressedPosition out)
	{
		for (int i=0, j=0; i<32; i++, j+=2)
		{
			unsigned char low, high;
			GetPieceToken (in [j], low);
			GetPieceToken (in [j+1], high);
			out [i] = (high << 4) | low;
		}
		return true;
	}

	static bool DecompressPosition (const CompressedPosition in, Symbol out [64])
	{
		for (int i=0, j=0; i<32; i++, j+=2)
		{
			unsigned char low, high;
			low = in [i] & 0xF;
			high = (in [i] >> 4) & 0xF;
			GetPieceSymbol (low, out [j]);
			GetPieceSymbol (high, out [j+1]);
		}
		return true;
	}

	static bool CompressHistory (const History in, CompressedHistory out)
	{
		//Four consecutive bytes are packed into three.

		const unsigned char SIX = 0x3F;
		const unsigned char TWO = 0x03;
		const unsigned char FOUR = 0x0F;

		for (int i=0, j=0; i<32 && j<24; i+=4, j+=3)
		{
			out [j+0] = ((in [i+1] & TWO) << 6) | (in [i+0] & SIX);
			out [j+1] = ((in [i+2] & FOUR) << 4) | ((in [i+1] >> 2) & FOUR);
			out [j+2] = ((in [i+3] & SIX) << 2) | ((in [i+2] >> 4) & TWO);
		}
		return true;
	}

	static bool DecompressHistory (const CompressedHistory in, History out)
	{
		const unsigned char SIX = 0x3F;
		const unsigned char TWO = 0x03;
		const unsigned char FOUR = 0x0F;

		for (int i=0, j=0; i<32 && j<24; i+=4, j+=3)
		{
			out [i+0] = in [j+0] & SIX;
			out [i+1] = ((in [j+1] & FOUR) << 2) | (in [j+0] >> 6) & TWO;
			out [i+2] = ((in [j+2] & TWO) << 4) | (in [j+1] >> 4) & FOUR;
			out [i+3] = (in [j+2] >> 2) & SIX;
		}
		return true;
	}

public:
	Board2 ()
	{
		Clear ();
	}

public:
	bool operator = (const Board2 & src)
	{
		memcpy (position, src.position, sizeof (CompressedPosition));
		memcpy (history, src.history, sizeof (CompressedHistory));
		return true;
	}

	bool operator = (const Position & src)
	{
		return CompressPosition (src);
	}

	bool operator = (const History & src)
	{
		return CompressHistory (src);
	}

	void Clear ()
	{
		memset (position, 0, sizeof (position));
		memset (history, 0, sizeof (history));
	}

public:
	bool CompressPosition (const Position in)
	{
		return CompressPosition (in, position);
	}

	bool DecompressPosition (Position out) const
	{
		return DecompressPosition (position, out);
	}

	bool CompressHistory (const History in)
	{
		return CompressHistory (in, history);
	}

	bool DecompressHistory (History out) const
	{
		return DecompressHistory (history, out);
	}

public:
	void Print () const
	{
		
	}

	bool Validate () const
	{
		int i;

		Position position;
		DecompressPosition (position);

		for (i=0; i<64; ++i)
			if (0 != position [i])
				break;
		if (i >= 64) return false;

		History history;
		DecompressHistory (history);

		for (i=0; i<32; ++i)
			if (history [(i<<1)+0] == history [(i<<1)+1])
				break;
		
		return i > 0 && 0 == i%2;
	}
};
