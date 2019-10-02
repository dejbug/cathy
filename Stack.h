#pragma once
#include <vector>

template <class T> class Stack : public std::vector <T>
{
public:
	bool Push (T & x)
	{
		int oldSize = size ();
		push_back (x);
		return size () == oldSize+1;
	}

	bool Pop (T & t)
	{
		if (empty ()) return false;
		t = back ();
		pop_back ();
		return true;
	}

	void Print (const char * text = 0)
	{
		if (text) OutputDebugString (text);
		for (size_t i=0; i<size (); i++)
			at (i).Print ();
	}

	bool Replace (int index, T & t)
	{
		if (index < 0 || index > size ()) return false;
		insert (begin() + index, t);
		erase (begin() + index - 1);
		return true;
	}

	void Clear ()
	{
		clear ();
	}

	bool Empty () const { return empty (); }

	int Count () const { return size (); }
};

template <class T> class Stack2 : public Stack <T>
{
private:
	int current;

public:
	Stack2 () : current (0) {}

public:

	int GetCurrentIndex () const { return current; }

	void Rewind ()
	{
		current = 0;
	}

	bool Next (T & t)
	{
		if (current < 0 || current >= size ()) return false;
		t = at (current++);
		return true;
	}

	bool Previous (T & t)
	{
		if (current < 1 || current > size ()) return false;
		t = at (--current);
		return true;
	}

	bool Current (T & t) const
	{
		if (current < 0 || current >= size ()) return false;
		t = at (current);
		return true;
	}

	void Clear ()
	{
		Stack<T>::Clear ();
		current = 0;
	}

	bool More () const
	{
		return current >= 0 && current < size ();
	}
};
