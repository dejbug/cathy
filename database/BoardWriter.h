#pragma once
#include "Board2.h"

class BoardWriter
{
private:
	unsigned char position [64];
	unsigned char moves [32];

public:
	bool SetPiece (unsigned char index, unsigned char token)
	{
		return true;
	}
};
