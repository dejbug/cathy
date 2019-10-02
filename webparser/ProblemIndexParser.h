#pragma once
#include "ProblemIndexFieldParser.h"
#include "../database/Database.h"

class ProblemIndexParserListener
{
public:
	typedef std::vector <ProblemIndexParserListener *> Vector;

public:
	virtual void OnEntry (Database::Entry & entry) = 0;
};

class ProblemIndexParserListeners :
	public ProblemIndexParserListener::Vector,
	public ProblemIndexParserListener
{
public:
	virtual void OnEntry (Database::Entry & entry)
	{
		for (iterator it=begin(); it!=end(); it++)
			(*it)->OnEntry (entry);
	}
};

class ProblemIndexParser :
	public ProblemIndexFieldParser,
	public ProblemIndexFieldParserListener
{
private:
	ProblemIndexParserListeners listeners;
	Database::Entry entry;

public:
	ProblemIndexParser ()
	{
		ProblemIndexFieldParser::Add (this);
	}
	
	void Add (ProblemIndexParserListener * listener) {
		listeners.push_back (listener);
	}

private:
	virtual void OnField (int index, unsigned short value)
	{
		switch (index)
		{
			case 0: entry.id = value; break;
			case 1: entry.pos = value; break;
			case 2: entry.rating = value; break;
			case 3: entry.rd = value; break;
			case 4: entry.moves = value; break;
			case 5: entry.color = value;
				listeners.OnEntry (entry);
				break;
		}
	}
};
