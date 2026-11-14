#pragma once
#include <iostream>
#include "Board.h"
#include "BoardDrawer.h"
#include "BoardHelper.h"

class MoveGenerator
{
public:

	MoveGenerator(Board& board);

	void PrecomputedMoves();
	std::vector<Board::Move>& GenerateMoves();
	std::vector<Board::Move>& GenerateMoves(int color, bool generateOnlyAttacks);
	void GenerateSlidingMoves(int startSquare, int piece, bool genOnlyAttacks);
	void GenerateKingMoves(int startSquare, int piece);
	void GenerateCastlingMoves(int startSquare, int piece);
	void GenerateKnightMoves(int startSquare, int piece, int file, int rank);
	void GeneratePawnMoves(int startSquare, int piece, bool generateOnlyAttacks, int rank, int file);

	void GenerateOpponentAttacks(int color);

	std::vector<Board::Move>& GenerateLegalMoves();

	std::vector<Board::Move>& GenerateLegalMoves2();

	bool GenerateCheck(int color);

	bool isMove(Board::Move& move);

	std::vector<Board::Move>& getMovesForSquare(int index);

public:
	std::vector<Board::Move> moves;
	std::vector<Board::Move> movesForPiece;
	std::vector<int> opponentAttacks;
	std::vector<int> opponentPins;
	std::vector<int> checkAttacks;

private:
	int DirectionOffsets[8] = { 8, -8, -1, 1, 7, -7, 9, -9 };
	int PawnDirectionOffsets[8] = { 8, 7, 9, 16, -8, -7, -9, -16 };
	int KnightDirectionOffsets[8] = {15, -15, -6, 6, 10, -10, 17, -17};
	int CastlingDirectionOffsets[2] = {1, -1};
	Board& board;
	std::vector<std::vector<int>> NumSquaresToEdge;
};

