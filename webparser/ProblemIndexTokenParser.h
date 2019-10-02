#pragma once
#include <vector>
#include "FileParser.h"
#include "../Common.h"

class ProblemIndexTokenParserListener
{
public:
	typedef std::vector <ProblemIndexTokenParserListener *> Vector;

public:
	virtual void OnToken (int index, const char * text) = 0;
};

class ProblemIndexTokenParserListeners :
	public ProblemIndexTokenParserListener::Vector,
	public ProblemIndexTokenParserListener
{
public:
	virtual void OnToken (int index, const char * text)
	{
		char * temp;
		if (Trim (text, &temp)) text = temp;

		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnToken (index, text);

		delete temp;
	}
};

class ProblemIndexTokenParser
{
private:
	ProblemIndexTokenParserListeners listeners;
	FileParser parser;

public:
	void Add (ProblemIndexTokenParserListener * listener) {
		listeners.push_back (listener);
	}

	bool Open (const char * path)
	{
		return parser.Open (path, "rt");
	}

	bool ParseNextProblem ()
	{
		char buffer [1024];
		int i;

		for (i=0; i<4; i++)
		{
			if (!parser.GetNextToken (buffer, sizeof (buffer), "<td align=right class='normal'>","</td>")) return false;
			listeners.OnToken (i, buffer);
		}

		if (!parser.GetNextToken (buffer, sizeof (buffer), "<td class='normal' align=right>","</td>")) return false;
		listeners.OnToken (4, buffer);

		for (i=0; i<2; i++)
		{
			if (!parser.GetNextToken (buffer, sizeof (buffer), "<td align=right class='normal'>","</td>")) return false;
			listeners.OnToken (5+i, buffer);
		}

		return true;
	}

	bool Parse ()
	{
		while (ParseNextProblem ());
		return true;
	}
};
