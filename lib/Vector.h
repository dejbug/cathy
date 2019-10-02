#pragma once

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (mem)

template <class X> class Vector
{
public:
	typedef std::vector <X *> VECTOR;
	typedef std::vector <X *>::iterator ITERATOR;

private:
	VECTOR * v;
	bool managed;

public:
	Vector ()
	{
		v = new VECTOR;
		managed = true;
	}

	Vector (VECTOR & rv, bool managed=true)
	{
		v = &rv;
		this->managed = managed;
	}

	Vector (VECTOR * pv, bool managed=true)
	{
		v = pv;
		this->managed = managed;
	}

	~Vector ()
	{
		Purge ();
		if (managed) delete v;
	}

public:
	bool Push (X * x)
	{
		if (!x) return false;
		v->push_back (x);
		return true;
	}
	
	bool Pop (X ** x)
	{
		if (v->empty ()) return false;
		if (!!x) *x = v->back ();
		v->pop_back ();
		return true;
	}

	bool Purge ()
	{
		ITERATOR it = v->begin ();
		for ( ; it != v->end (); it++)
			delete (*it);
		v->clear ();
		return true;
	}
};

END_NAMESPACE (mem)
END_NAMESPACE (lib)
