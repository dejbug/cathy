#pragma once
#include "../webparser/File.h"
#include "Board.h"
#include "Board2.h"
#include <stddef.h>

#include "Position.h"
#include "History.h"

#include "Memory.h"

/*
FIXME: The current record layout is broken! See: p00057.

It doesn't take into account positions with more than the regular number
of a certain kind of piece (which occurs whenever a pawn passes).

As it turns out, 32 bytes are sufficient to hold the necessary data for
all 64 squares, since each square only needs 4 bits (> 12 different pieces,
6 for each side), therefore: 64 * 4 = 256 bits = 256 / 8 = 32 bytes.

Hence, the file size of the complete database will stay the same as it
is now: around 1 MB.

We could compress the header info (ratings will probably never be higher
than 3500 (< 12 bits), rds will be less than 350 (< 9 bits); also more
than 8 (= 3 bits) user moves would be unusual.  Also the color needs
only one bit instead of the whole byte.

With the above changes the 80-bit header would come down to less than
64 bits (= 8 bytes).

[ We could get rid of the 16 bits for the id and reduce them to (maybe)
6, if we would use the field as an offset to the previous problem's id
(the observation being that the difference between consecutive problem
ids is always < 32). This would however slow down the I/O noticably,
since an item's id is calulated by knowledge of each previous id! ]

We will run through all the data records and determine the upper bounds
for the current snapshot of the database. Future additions to the data
set might blow our layout, however. An update to the application would
be in order in that case and to ease the process, a highly modular
(plug-in) architecture should be designed around the database I/0.
*/

class Database : public File
{
public:
	class Entry
	{
	public:
		unsigned short id;
		unsigned short pos;
		unsigned short rating;
		unsigned short rd;
		unsigned char moves;
		unsigned char color;
		Board board;

	public:
		Entry () : id(0), pos(0), rating(0), rd(0), moves(0), color(0)
		{
		}

	public:
		bool Write (const Database & base) const
		{
			return base.Write ((const char *) this, sizeof (Database::Entry));
		}

		bool Read (const Database & base)
		{
			return base.ReadExactly ((char *) this, sizeof (Database::Entry));
		}

		void Print () const
		{			
			printf ("ID:     %d\n", id);
			printf ("POS:    %d\n", pos);
			printf ("Rating: %d\n", rating);
			printf ("RD:     %d\n", rd);
			printf ("Moves:  %d\n", moves);
			printf ("Color:  %d (%s)\n", color, color ? "black":"white");
			board.Print ();
		}
	};


public:
	bool Open (const char * path, bool writable = true, bool existing = false)
	{
		if (!File::Exists (path) && !existing && !File::Create (path)) return false;
		return File::Open (path, writable ? "r+b" : "rb");
	}

	bool Add (const Entry & entry) const
	{
		return entry.Write (*this);
	}

	bool Move (unsigned short index) const
	{
		return File::Seek (index * sizeof (Entry));
	}

	bool Seek (short index) const
	{
		return File::Seek (index * sizeof (Entry));
	}

	bool Skip (short index) const
	{
		return File::Skip (index * sizeof (Entry));
	}

	bool Insert (unsigned short index, const Entry & entry) const
	{
		long old; File::Tell (old);

		Move (index);
		bool result = entry.Write (*this);
		
		File::Seek (old);
		return result;
	}

public:
	size_t Count () const
	{
		return Size () / sizeof (Entry);
	}

public:
	bool FindEntryIndexById (unsigned short id, unsigned short & index)
	{
		if (!Ready ()) return false;

		Database::Entry entry;
		size_t count = Count ();

		Move (0);

		for (size_t i=0; i<count; i++)
		{
			if (!entry.Read (*this)) break;
			if (id <= entry.id)
				return index = i, true;
		}

		return false;
	}

	bool FindEntryById (unsigned short id, Database::Entry & entry)
	{
		if (!Ready ()) return false;
		
		unsigned short index;
		
		if (FindEntryIndexById (id, index))
			return Move (index) && entry.Read (*this);

		return false;
	}
};
