#pragma once
#include "Board.h"
#include "MoveGenerator.h"
#include "BoardHelper.h"
#include "PieceBoard.h"
class Evaluation
{
public:
	Evaluation(Board& board, MoveGenerator& moveGen);

	int Evaluate();
	int CountMaterial(int colorIndex);
	int EvaluatePieceSquareTables(int colourIndex, float endgamePhaseWeight);

	static int getPieceValue(int piece);

public:
	static const int pawnValue = 100;
	static const int knightValue = 300;
	static const int bishopValue = 300;
	static const int rookValue = 500;
	static const int queenValue = 900;

	static const int negativeInfinity = -99999;

	const float endgameMaterialStart = rookValue * 2 + bishopValue + knightValue;
private:

	static int EvaluatePieceSquareTable(const int* table, const std::vector<int> pieceList, bool isWhite);
	
	float EndgamePhaseWeight(int materialCountWithoutPawns) 
	{
		const float multiplier = 1.f / endgameMaterialStart;
		return 1.f - std::min(1.f, materialCountWithoutPawns * multiplier);
	}
	
	int ForceKingToCenter(int myKingSquare, int opponentKingSquare, int endGameValue);

	Board& board;
	MoveGenerator& moveGen;
};

