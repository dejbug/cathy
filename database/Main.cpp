#include <stdio.h>
#include "Database.h"

#include "Database2.h"
#include "Board2.h"
#include "Position.h"

int test1 (int argc, char ** argv)
{
	Database2::Entry entry;
	entry.header.Set (13, 11637, 1382, 34, 1, 0);
	
	Position position;
	position.Set (0, 'B');
	position.Set (27, 'k');
	entry.board = position;

	History history;
	history.Set (0, 45, 46);
	entry.board = history;

	entry.Print ();

	Database2 base;
	base.Open ("base.txt");

	base.Insert (0, entry);

	base.Close ();
	return 0;
}

int test5 (int argc, char ** argv)
{
	Database2::Entry entry;
	Database2 base;

	base.Open ("base.txt");

	entry.Read (base);
	entry.Print ();
	return 0;
}

/**
	Testing Board2/Position interconnection.
*/
int test2 (int argc, char ** argv)
{
	const Board2::Symbol pieces [] = {'K','Q','R','B','N','P','k','q','r','b','n','p'};

	Board2 board;
	Position input, output;

	// Fill the input position with some values.
	for (Board2::Square i=0, j=0; i<64; i++, j=(j+1)%sizeof(pieces))
		input [i] = pieces [j];
	input.Print ();

	// Fill board from input position.
	board = input;
	
	// Fill output position from board.
	output = board;
	output.Print ();

	return 0;
}

#include "History.h"

/**
	Testing Board2/History interconnection.
*/
int test3 (int argc, char ** argv)
{
	Board2 board;
	History input, output;

	input.Set (0, 0x0A, 0x16);
	input.Set (1, 0x1C, 0x07);
	input.Print ();

	board = input;
	output = board;
	output.Print ();
	return 0;
}

int test4 (int argc, char ** argv)
{
	printf ("sizeof (Database2::Entry) = %d\n\n",
		sizeof (Database2::Entry));

	Database2::Entry entry;
	entry.header.id = 65535;
	entry.header.pos = 65535;
	entry.header.rating = 4095;
	entry.header.rd = 511;
	entry.header.moves = 15;
	entry.header.color = 1;

	Position position;
	position.Set (11, 'Q');
	position.Set (45, 'b');
	entry.board = position;

	History history;
	history.Set (0, 11, 43);
	history.Set (1, 43, 45);
	entry.board = history;

	entry.Print ();
	return 0;
}

#include "PGN.H"

/**
	Converting move histories into short algebraic notation.
*/
void test6 ()
{
	Database2::Entry entry;
	GetProblem (6, entry);
	
	Position position = entry.board;
	History history = entry.board;

	position.Print ();

	PrintAlgebraic (position, history);

	position.Print ();
}

int main (int argc, char ** argv)
{
	return test6 (), 0;

	return test5 (argc, argv);
	return test1 (argc, argv);
	return test4 (argc, argv);
	return test3 (argc, argv);
	return test2 (argc, argv);
}
