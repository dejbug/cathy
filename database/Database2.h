#pragma once

class Database2 : public File
{
public:
	class Header
	{
	public:
		typedef unsigned int Bitfield;

	public:
		unsigned short id;
		unsigned short pos;
		Bitfield rating : 12;
		Bitfield rd : 9;
		Bitfield moves : 4;
		Bitfield color : 1;
		Bitfield extra : 6;

	public:
		Header () : id(0), pos(0), rating(0), rd(0), moves(0), color(0), extra(0)
		{
		}

	public:
		bool operator = (const Header & src)
		{
			id = src.id;
			pos = src.pos;
			rating = src.rating;
			rd = src.rd;
			moves = src.moves;
			color = src.color;
			extra = src.extra;
			return true;
		}

	public:
		void Set (unsigned short id, unsigned short pos, unsigned short rating, unsigned short rd, unsigned char moves, unsigned char color)
		{
			this->id = id;
			this->pos = pos;
			this->rating = rating;
			this->rd = rd;
			this->moves = moves;
			this->color = color;
		}

	public:
		void Print () const
		{
			printf (
				"ID:     %d\n"
				"POS:    %d\n"
				"Rating: %d\n"
				"RD:     %d\n"
				"Moves:  %d\n"
				"Color:  %d\n",
				id, pos, rating, rd, moves, color);
		}
	};

	class Entry
	{
	public:
		Header header;
		Board2 board;

	public:
		bool operator = (const Entry & src)
		{
			header = src.header;

			return true;
		}

	public:
		bool Write (const Database2 & base) const
		{
			return base.Write ((const char *) this, sizeof (Database2::Entry));
		}

		bool Read (const Database2 & base)
		{
			return base.ReadExactly ((char *) this, sizeof (Database2::Entry));
		}

	public:
		void Print () const
		{
			header.Print ();

			printf ("\n");
			
			Position position (board);
			History history (board);

			position.Print ();
			history.Print ();

			printf ("\n\n");

			Memory::Dump ((void *) this, 64);

			printf ("\n");
		}

		bool Validate () const
		{
			return header.id > 0 && header.moves > 0 && board.Validate ();
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

		Entry entry;
		size_t count = Count ();

		Move (0);

		for (size_t i=0; i<count; i++)
		{
			if (!entry.Read (*this)) break;
			if (id <= entry.header.id)
				return index = i, true;
		}

		return false;
	}

	bool FindEntryById (unsigned short id, Database2::Entry & entry)
	{
		if (!Ready ()) return false;
		
		unsigned short index;
		
		if (FindEntryIndexById (id, index))
			return Move (index) && entry.Read (*this);

		return false;
	}
};
