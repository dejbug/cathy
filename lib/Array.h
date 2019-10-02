#pragma once
#include "Defines.h"
#include <vector>

BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (mem)

template <class X> class Array
{
public:
	typedef std::vector <X *> Items;
	typedef std::vector <bool> Flags;
public:
	Items items;
	Flags flags;
public:
	~Array ()
	{
		Delete ();
	}
public:
	bool Delete ()
	{
		typename Items::iterator it = items.begin ();
		Flags::iterator ft = flags.begin ();
		for (; it != items.end () && ft != flags.end (); it++, ft++) {
			if (*ft) delete *it;
		}
		items.clear ();
		flags.clear ();
		return true;
	}

	bool Push (X * x, bool managed = true)
	{
		items.push_back (x);
		flags.push_back (managed);
		return items.size () == flags.size ();
	}

	bool Pop (X ** x, bool * managed = 0)
	{
		if (!x) return false;
		if (0 == items.size ()) {
			*x = 0;
			return false;
		}
		*x = items.back ();
		items.pop_back ();
		if (managed) *managed = flags.back ();
		flags.pop_back ();
		return !!*x;
	}

	int Size () const
	{
		return items.size ();
	}

	bool Empty () const
	{
		return items.empty ();
	}

	bool At (int index, X ** x)
	{
		if (!x) return false;
		if (index < 0 || index >= items.size ()) {
			*x = 0;
			return false;
		}

		*x = items.at (index);
		return true;
	}

	bool Top (X ** x)
	{
		if (!x) return false;
		if (items.empty ()) {
			*x = 0;
			return false;
		}

		*x = items.back ();
		return true;
	}
};

END_NAMESPACE (mem)
END_NAMESPACE (lib)
