#include "Strings.h"

INCLUDE_NAMESPACE (lib::mem)

Strings::Strings ()
{
}

Strings::~Strings ()
{
	Purge ();
}

bool Strings::Push (const char * x)
{
	char * y = x ? strdup (x) : strdup ("");
	push_back (y);
	return true;
}

bool Strings::Pop (char ** x)
{
	if (empty ()) return false;
	if (!!x) *x = back ();
	pop_back ();
	return true;
}

bool Strings::Purge ()
{
	iterator it = begin ();
	for ( ; it != end (); it++)
		delete (*it);
	clear ();
	return true;
}
