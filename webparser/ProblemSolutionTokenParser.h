#pragma once
#include <vector>

class ProblemSolutionTokenParserListener
{
public:
	typedef std::vector <ProblemSolutionTokenParserListener *> Vector;

public:
	virtual void OnToken (int index, const char * text) = 0;
};

class ProblemSolutionTokenParserListeners :
	public ProblemSolutionTokenParserListener::Vector,
	public ProblemSolutionTokenParserListener
{
public:
	virtual void OnToken (int index, const char * text)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnToken (index, text);
	}
};

class ProblemSolutionTokenParser
{
private:
	ProblemSolutionTokenParserListeners listeners;

public:
	void Add (ProblemSolutionTokenParserListener * listener) {
		listeners.push_back (listener);
	}

protected:
	bool ExplodeBoard (char * text, char & color)
	{
		int textlen = text ? strlen (text) : 0;
		if (66!=textlen) return false;
		if (' '!=text [64]) return false;
		text [64] = 0;
		color = text [65];
		//if ('w'==color) NormaliseBoard (text);
		return true;
	}
/*
	bool NormaliseBoard (char * text)
	{
		int textlen = text ? strlen (text) : 0;
		if (64!=textlen) return false;
		for (int i=0; i<32; i++)
		{
			char & src = text [i];
			char & dst = text [textlen-1-i];
			char tmp = src;
			src = dst;
			dst = tmp;
		}
		return true;
	}
*/
public:
	bool Parse (const char * path)
	{
		if (!path) return false;

		FileParser parser;
//		parser.Open (path, "rt");
		parser.Open (path, "rb");

		char buffer [1024];
		parser.GetNextToken (buffer, sizeof (buffer), "<script type='text/javascript'>\r\nPlyUBound= ",";");
		//printf ("%s\n", buffer);

		int count = atoi (buffer);
		char temp [16];
		int i;

		for (i=0; i<count; i++)
		{
			sprintf (temp, "Boards[%d]=\"", i);
			parser.GetNextToken (buffer, sizeof (buffer), temp, "\";");
			//printf ("%s\n", buffer);
			if (0 == i) {
				char color [2] = {0, 0};
				if (! ExplodeBoard (buffer, color[0])) return false;
				listeners.OnToken (0, color);
				listeners.OnToken (1, buffer);
			}
		}

		for (i=0; i<count-1; i++)
		{
			sprintf (temp, "ActFrom[%d]=\"", i);
			parser.GetNextToken (buffer, sizeof (buffer), temp, "\";");
			//printf ("%s -> ", buffer);
			listeners.OnToken (2|(i<<8), buffer);

			sprintf (temp, "ActTo[%d]=\"", i);
			parser.GetNextToken (buffer, sizeof (buffer), temp, "\";");
			//printf ("%s\n", buffer);
			listeners.OnToken (3|(i<<8), buffer);
		}

		listeners.OnToken (4, 0);

		return true;
	}
};
