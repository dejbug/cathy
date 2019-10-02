#pragma once
#include <ctype.h>

class Memory
{
public:
	static bool Dump (void * begin, int length)
	{
		int i;

		if (length > 16)
		{
			for (i=0; length > 16; i++, length-=16)
				Dump ((unsigned char *) begin + i*16, 16);
			if (length > 0) Dump ((unsigned char *) begin + i*16, length);
			return true;
		}

		for (i=0; i<length; ++i)
		{
			unsigned char byte = 0xFF & ((unsigned char *) begin)[i];
			printf ("%02x ", byte);
		}

		int padding = 16-length; padding = padding < 0 ? 0 : padding;

		for (i=0; i<padding; ++i)
		{
			printf ("   ");
		}
		for (i=0; i<length; i++)
		{
			unsigned char byte = 0xFF & ((unsigned char *) begin)[i];
			printf ("%c", isprint (byte) ? byte : '.');
		}
		printf ("\n");

		return true;
	}

};
