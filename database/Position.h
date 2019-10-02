#pragma once
#include "Board2.h"

class Position
{
private:
	Board2::Position data;

public:
	Position ()
	{
		Clear ();
	}

	Position (const Board2 & src)
	{
		operator = (src);
	}

	Position (const unsigned char (& src) [64])
	{
		operator = (src);
	}

public:
	bool operator = (const Board2 & src)
	{
		return src.DecompressPosition (data);
	}

	bool operator = (const unsigned char (& data) [64])
	{
		memcpy (this->data, data, sizeof (this->data));
		return true;
	}

	Board2::Symbol & operator [] (Board2::Square index)
	{
		return data [index];
	}

	operator const Board2::Position & () const { return data; }

	const Board2::Position & GetDataPointer () const { return data; }

public:
	bool IsIndexInRange (int index) const { return index >= 0 && index < 64; }

public:
	void Clear () { memset (data, 0, sizeof (data)); }

	bool Set (Board2::Square square, Board2::Symbol symbol)
	{
		if (!IsIndexInRange (square)) return false;
		return data [square] = symbol, true;
	}

	bool Get (Board2::Square square, Board2::Symbol & symbol)
	{
		if (!IsIndexInRange (square)) return false;
		return symbol = data [square], true;
	}

public:
	void Print () const
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
};
