#pragma once
#include "Common.h"

#include "Defines.h"
BEGIN_NAMESPACE (lib)
BEGIN_NAMESPACE (mem)

/*
	Convenience class which manages copies of zero-terminated strings.
	On destruction all the string copies will be deleted individually.
*/

class Strings : public std::vector <char *>
{
public:
	Strings ();
	~Strings ();

public:
	bool Push (const char * x);
	bool Pop (char ** x);
	bool Purge ();
};

END_NAMESPACE (mem)
END_NAMESPACE (lib)
