#pragma once
#include "BoardHelper.h"
class PieceBoard
{
public:
	static int Read(const int* table, int square, bool isBlack)
	{
		if (isBlack) 
		{
			int file = BoardHelper::indexToFile(square);
			int rank = BoardHelper::indexToRank(square);
			rank--;
			square = rank * 8 + file;
		}

		return table[square];
	};

	static const int pawns[64];

	static const int knights[64];

	static const int bishops[64];

	static const int rooks[64];

	static const int queens[64];

	static const int kingMiddle[64];

	static const int kingEnd[64];
};

