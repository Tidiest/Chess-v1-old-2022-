#pragma once
#include "MoveGenerator.h"
#include "Board.h"
#include "Evaluation.h"
#include "MoveOrdering.h"

class SearchAI
{
public:
	SearchAI(MoveGenerator& moveGen, Board& board, Evaluation& evaluator, MoveOrdering& moveOrdering);

	int MoveGenTest(int depth, Board::Move& move, int ply);
	int Search(int depth, int maxDepth, int alpha, int beta);
	int Search(int depth, int maxDepth);

	Board::Move getBestMove() { return this->bestMove; };

private:
	MoveGenerator& moveGen;
	Board& board;
	Evaluation& evaluator;
	Board::Move bestMove;
	MoveOrdering& moveOrdering;
};

