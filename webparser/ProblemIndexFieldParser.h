#pragma once
#include <vector>
#include "ProblemIndexTokenParser.h"

class ProblemIndexFieldParserListener
{
public:
	typedef std::vector <ProblemIndexFieldParserListener *> Vector;

public:
	virtual void OnField (int index, unsigned short value) = 0;
};

class ProblemIndexFieldParserListeners :
	public ProblemIndexFieldParserListener::Vector,
	public ProblemIndexFieldParserListener
{
public:
	virtual void OnField (int index, unsigned short value)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnField (index, value);
	}
};

class ProblemIndexFieldParser : public ProblemIndexTokenParserListener
{
private:
	ProblemIndexFieldParserListeners listeners;
	ProblemIndexTokenParser problemIndexParser;

public:
	ProblemIndexFieldParser ()
	{
		problemIndexParser.Add (this);
	}

	void Add (ProblemIndexFieldParserListener * listener) {
		listeners.push_back (listener);
	}

	bool Open (const char * path) {
		return problemIndexParser.Open (path);
	}

	bool Parse () {
		return problemIndexParser.Parse ();
	}

protected:
	bool ParseUrlToken (const char * text, unsigned short & id, unsigned short & pos)
	{
		if (!text) return false;
		int textlen = strlen (text);
		if (textlen < 5) return false;
		
		char idbuf [16];
		memcpy (idbuf, text + textlen - 9, 5);
		idbuf [5] = 0;
		id = atoi (idbuf);

		char posbuf [16];
		memcpy (posbuf, text + textlen - 17, 5);
		posbuf [5] = 0;
		char * posbufptr = posbuf;
		for (; *posbufptr && (*posbufptr<'0' || *posbufptr>'9'); posbufptr++);
		pos = atoi (posbufptr);
		return true;
	}

	bool ParseColorToken (const char * text, unsigned char & color)
	{
		color = 'b'==*text ? 1 : 0;
		return true;
	}

	bool ParseNumericToken (const char * text, unsigned short & value)
	{
		value = text ? atoi (text) : 0;
		return true;
	}

private:
	virtual void OnToken (int index, const char * text)
	{
		switch (index)
		{
			default: break;
			
			case 0:
			{
				unsigned short id, pos;
				ParseUrlToken (text, id, pos);
				listeners.OnField (0, id);
				listeners.OnField (1, pos);
			}	break;

			case 1: case 2: case 3:
			{
				unsigned short value;
				ParseNumericToken (text, value);
				listeners.OnField (index+1, value);
			}	break;

			case 4:
			{
				unsigned char color;
				ParseColorToken (text, color);
				listeners.OnField (index+1, color);
			}	break;
		}
		
	}
};
