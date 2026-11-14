#pragma once
#include <iostream>
#include "MoveGenerator.h"
#include "Evaluation.h"
class MoveOrdering
{
public:
	MoveOrdering(MoveGenerator& moveGen);

	void OrderMoves(Board& board, std::vector<Board::Move>& moves);
	void Sort(std::vector<Board::Move>& moves);
private:
	int* moveScores;
	const int maxMoveCount = 218;
	const int capturedPieceValueMultiplier = 10;

	MoveGenerator& moveGenerator;
};

