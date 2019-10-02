#pragma once
#include "Board2.h"

class History
{
private:
	Board2::History data;

public:
	History ()
	{
		Clear ();
	}

	History (const Board2 & src)
	{
		operator = (src);
	}

	bool operator = (const Board2 & src)
	{
		return src.DecompressHistory (data);
	}

	Board2::Square & operator [] (int index)
	{
		return data [index];
	}

	operator const Board2::History & () const { return data; }

	const Board2::History & GetDataPointer () const { return data; }

public:
	bool IsIndexInRange (int index) const { return index >= 0 && index < 32-1; }

public:
	void Clear () { memset (data, 0, sizeof (data)); }

	bool Set (int index, Board2::Square src, Board2::Square dst)
	{
		if (!IsIndexInRange (index)) return false;
		if (dst == src) return false;
		return data [(index<<1)+0] = src, data [(index<<1)+1] = dst, true;
	}

	bool Get (int index, Board2::Square & src, Board2::Square & dst) const
	{
		if (!IsIndexInRange (index)) return false;
		return src = data [(index<<1)+0], dst = data [(index<<1)+1], dst != src;
	}

public:
	void Print () const
	{
		Board2::Square src, dst;

		int i=0;
		for (; i<16; i++)
		{
			if (!Get (i, src, dst)) break;
			if (i > 0) printf (",   ");
			if (i > 0 && 0==i%4) printf ("\n");
			printf ("(%x). %2d -> %2d", i, src, dst);
		}
		if (i > 1) printf (".\n");
	}
};
