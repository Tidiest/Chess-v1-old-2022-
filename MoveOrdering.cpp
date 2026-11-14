#include "MoveOrdering.h"

MoveOrdering::MoveOrdering(MoveGenerator& moveGen) : moveGenerator(moveGen)
{
	moveScores = new int[maxMoveCount];
}

void MoveOrdering::OrderMoves(Board& board, std::vector<Board::Move>& moves)
{
	for (int i = 0; i < moves.size(); i++) 
	{
		int score = 0;
		int movePieceType = Board::getType(board.squares[moves[i].startSquare]);
		int capturePieceType = Board::getType(board.squares[moves[i].targetSquare]);
		int flag = moves[i].MoveFlag;

		if (capturePieceType != Board::Piece::None) 
		{
			// Order moves to try capturing the most valuable opponent piece with least valuable of own pieces first
			// The capturedPieceValueMultiplier is used to make even 'bad' captures like QxP rank above non-captures
			score = capturedPieceValueMultiplier * Evaluation::getPieceValue(capturePieceType) - Evaluation::getPieceValue(movePieceType);
		}

		if (movePieceType == Board::Pawn) 
		{

			if ((flag & 0b1110) == Board::Move::PromoteToQueenFlag) 
			{
				score += Evaluation::queenValue;
			}
			else if ((flag & 0b1110) == Board::Move::PromoteToKnightFlag)
			{
				score += Evaluation::knightValue;
			}
			else if ((flag & 0b1110) == Board::Move::PromoteToRookFlag)
			{
				score += Evaluation::rookValue;
			}
			else if ((flag & 0b1110) == Board::Move::PromoteToBishopFlag)
			{
				score += Evaluation::bishopValue;
			}
		}
		/*else {
			// Penalize moving piece to a square attacked by opponent pawn
			if (BitBoardUtility.ContainsSquare(moveGenerator.opponentPawnAttackMap, moves[i].TargetSquare)) {
				score -= squareControlledByOpponentPawnPenalty;
			}
		}*/

		moveScores[i] = score;
	}
	
	Sort(moves);
}

void MoveOrdering::Sort(std::vector<Board::Move>& moves)
{
	// Sort the moves list based on scores
	if (moves.empty())
	{
		return;
	}
	for (int i = 0; i < moves.size() - 1; i++) 
	{
		for (int j = i + 1; j > 0; j--) 
		{
			int swapIndex = j - 1;
			if (moveScores[swapIndex] < moveScores[j])
			{
				Board::Move temp = moves[j];
				moves[j] = moves[swapIndex];
				moves[swapIndex] = temp;

				int tempScore = moveScores[j];
				moveScores[j] = moveScores[swapIndex];
				moveScores[swapIndex] = tempScore;
			}
			
		}
	}
}
