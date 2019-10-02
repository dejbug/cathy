#include <stdio.h>
#include "Parser.h"

const char * html = "<html><body>bla<br /><p style=\"color: red;\" align=\"left\">hello</p></body></html>";

class Listener : public Parser::Listener
{
	virtual void OnTagOpen (const char * text)
	{
		printf ("OPEN: %s.\n", text);
	}
	virtual void OnTagClose (const char * text)
	{
		printf ("CLOSE: %s.\n", text);
	}
	virtual void OnTagInline (const char * text)
	{
		printf ("INLINE: %s.\n", text);
	}
	virtual void OnAttribute (const char * key, const char * value)
	{
		printf ("ATTRIBUTE: %s = %s\n", key, value);
	}
	virtual void OnText (const char * text)
	{
		printf ("TEXT: %s\n", text);
	}
};

int test (int argc, char ** argv)
{
	Listener listener;
	Parser parser;
	parser.Add (&listener);
	parser.Write (html);
	parser.Parse ();
	return 0;
}

#include "FileParser.h"

void ParseIndexBatch ()
{
	FileParser parser;
	parser.Open ("c:\\Projects\\Game\\CTSI\\Documentation\\index.txt", "rt");

	char buffer [1024];
	for (int i=0; i<3; i++)
	{
		int j;
		for (j=0; j<3; j++)
		{
			parser.GetNextToken (buffer, sizeof (buffer), "<td align=right class='normal'>","</td>");
			printf ("%02d. %s\n", i, buffer);
		}

		parser.GetNextToken (buffer, sizeof (buffer), "<td class='normal' align=right>","</td>");
		printf ("%02d. %s\n", i, buffer);

		for (j=0; j<2; j++)
		{
			parser.GetNextToken (buffer, sizeof (buffer), "<td align=right class='normal'>","</td>");
			printf ("%02d. %s\n", i, buffer);
		}
	}
}

void ParseSolution ()
{
	FileParser parser;
	parser.Open ("c:\\Projects\\Game\\CTSI\\Documentation\\solution.txt", "rt");

	char buffer [1024];
	parser.GetNextToken (buffer, sizeof (buffer), "<script type='text/javascript'>\r\nPlyUBound= ",";");
	printf ("%s\n", buffer);

	int count = atoi (buffer);
	char temp [16];
	int i;

	for (i=0; i<count; i++)
	{
		sprintf (temp, "Boards[%d]=\"", i);
		parser.GetNextToken (buffer, sizeof (buffer), temp, "\";");
		printf ("%s\n", buffer);
	}

	for (i=0; i<count-1; i++)
	{
		sprintf (temp, "ActFrom[%d]=\"", i);
		parser.GetNextToken (buffer, sizeof (buffer), temp, "\";");
		printf ("%s -> ", buffer);

		sprintf (temp, "ActTo[%d]=\"", i);
		parser.GetNextToken (buffer, sizeof (buffer), temp, "\";");
		printf ("%s\n", buffer);		
	}
}

#include "ProblemIndexTokenParser.h"

class MyProblemIndexTokenParserListener :
	public ProblemIndexTokenParserListener
{
	virtual void OnToken (int index, const char * text)
	{
		printf ("%d: %s\n", index, text);
	}
};

#include "ProblemIndexFieldParser.h"

class MyProblemIndexFieldParserListener :
	public ProblemIndexFieldParserListener
{
	virtual void OnField (int index, unsigned short value)
	{
		static const char * FIELDS [] = {"ID:    ","POS:   ","Rating:","RD:    ","Moves: ","Color: "};
		printf ("%d. FIELD %s ", index, FIELDS [index]);
		printf ("%d", value);
		if (5==index) printf (value ? " (black)" : " (white)");
		printf ("\n");

		if (index > 4) printf ("\n");
	}
};

#include "ProblemIndexParser.h"

class MyProblemIndexParserListener :
	public ProblemIndexParserListener
{
	virtual void OnEntry (Database::Entry & entry)
	{
		entry.Print ();
		printf ("\n");
	}
};

#include "ProblemSolutionTokenParser.h"
#include "ProblemSolutionFieldParser.h"
#include "ProblemSolutionParser.h"

int main (int argc, char ** argv)
{
/*
	MyProblemIndexTokenParserListener listener;
	ProblemIndexTokenParser parser;

	parser.Add (&listener);
	parser.Open ("c:\\Projects\\Game\\CTSI\\Documentation\\index.txt");
	parser.Parse ();
*/

/*
	MyProblemIndexFieldParserListener listener;
	ProblemIndexFieldParser parser;

	parser.Add (&listener);
	parser.Open ("c:\\Projects\\Game\\CTSI\\Documentation\\index.txt");
	parser.Parse ();
*/

/*
	MyProblemIndexParserListener listener;
	ProblemIndexParser parser;
	parser.Add (&listener);
	parser.Open ("c:\\Projects\\Game\\CTSI\\Documentation\\index.txt");
	parser.Parse ();
*/

/*
	//ParseIndexBatch ();
	ParseSolution ();
*/

	ProblemSolutionTokenParser tokenParser;
	ProblemSolutionFieldParser fieldParser;
	ProblemSolutionParser parser;

	fieldParser.Add (&parser);
	tokenParser.Add (&fieldParser);
	//tokenParser.Parse ("c:\\Projects\\Game\\CTSI\\Documentation\\solution.txt");
	tokenParser.Parse ("c:\\Projects\\Game\\CTSI\\harvester\\solution.tmp");

	return 0;
}
