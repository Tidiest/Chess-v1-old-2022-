#include "MoveGenerator.h"

MoveGenerator::MoveGenerator(Board& board) : board(board)
{
	this->PrecomputedMoves();
}

void MoveGenerator::PrecomputedMoves()
{
	this->NumSquaresToEdge.resize(64);
	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int numNorth = 7 - rank;
			int numSouth = rank;
			int numWest = file;
			int numEast = 7 - file;

			int squareIndex = rank * 8 + file;

			std::vector<int> numSquaresOfIndex = 
			{ 
				numNorth,
				numSouth,
				numWest,
				numEast,
				std::min(numNorth, numWest),
				std::min(numSouth, numEast),
				std::min(numNorth, numEast),
				std::min(numSouth, numWest) 
			};
			this->NumSquaresToEdge[squareIndex] = (numSquaresOfIndex);

		}
	}
}

std::vector<Board::Move>& MoveGenerator::GenerateMoves()
{
	moves.clear();

	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int startSquare = rank * 8 + file;
			int piece = board.squares[startSquare];
			if (Board::getColor(piece) == board.colorToMove)
			{
				if (BoardDrawer::Piece::isSlidingPiece(Board::Piece(piece)))
				{
					GenerateSlidingMoves(startSquare, piece, false);
				}
				if (Board::getType(piece) == Board::Piece::King)
				{
					GenerateKingMoves(startSquare, piece);
					GenerateCastlingMoves(startSquare, piece);
				}
				if (Board::getType(piece) == Board::Piece::Knight)
				{
					GenerateKnightMoves(startSquare, piece, file, rank);
				}
				if (Board::getType(piece) == Board::Piece::Pawn)
				{
					GeneratePawnMoves(startSquare, piece, false, rank, file);
				}
			}
		}
	}
	return moves;
}

std::vector<Board::Move>& MoveGenerator::GenerateMoves(int color, bool generateOnlyAttacks = false)
{
	moves.clear();

	for (int file = 0; file < 8; file++)
	{
		for (int rank = 0; rank < 8; rank++)
		{
			int startSquare = rank * 8 + file;
			int piece = board.squares[startSquare];
			if (Board::getColor(piece) == color)
			{
				if (BoardDrawer::Piece::isSlidingPiece(Board::Piece(piece)))
				{
					GenerateSlidingMoves(startSquare, piece, generateOnlyAttacks);
				}
				if (Board::getType(piece) == Board::Piece::King)
				{
					GenerateKingMoves(startSquare, piece);
				}
				if (Board::getType(piece) == Board::Piece::Knight)
				{
					GenerateKnightMoves(startSquare, piece, file, rank);
				}
				if (Board::getType(piece) == Board::Piece::Pawn)
				{
					GeneratePawnMoves(startSquare, piece, generateOnlyAttacks, rank, file);
				}
			}
		}
	}
	return moves;
}

std::vector<Board::Move>& MoveGenerator::GenerateLegalMoves()
{
	GenerateOpponentAttacks(board.colorToMove);

	std::vector<Board::Move> pseudoLegalMoves = GenerateMoves();
	std::vector<Board::Move> legalMoves;
	bool legal;
	int illegalMovesCount = 0;

	for (Board::Move& moveToVerify : pseudoLegalMoves)
	{	
		#if 1
		legal = true;
		board.makeMove(moveToVerify);
		GenerateOpponentAttacks(Board::getOpponentColor(board.colorToMove));
		for (int attack : opponentAttacks)
		{
			if (attack == board.myKingSquare)
			{
				legal = false;
				illegalMovesCount++;
				break;
			}
		}
		if (legal)
		{
			legalMoves.push_back(moveToVerify);
		}
		board.unmakeMove();
		#endif
		//legalMoves.push_back(moveToVerify);
	}
	
	moves = legalMoves;
	return moves;
}

std::vector<Board::Move>& MoveGenerator::GenerateLegalMoves2()
{
	GenerateOpponentAttacks(board.colorToMove);

	std::vector<Board::Move> pseudoLegalMoves = GenerateMoves();
	std::vector<Board::Move> legalMoves;
	bool legal;
	int illegalMovesCount = 0;

	for (Board::Move& moveToVerify : pseudoLegalMoves)
	{
		#if 0
		legal = true;
		board.makeMove(moveToVerify);
		GenerateOpponentAttacks(Board::getOpponentColor(board.colorToMove));
		for (int attack : opponentAttacks)
		{
			if (attack == board.myKingSquare)
			{
				legal = false;
				illegalMovesCount++;
				break;
			}
		}
		if (legal)
		{
			legalMoves.push_back(moveToVerify);
		}
		board.unmakeMove();
		#endif
		legalMoves.push_back(moveToVerify);
	}

	moves = legalMoves;
	return moves;
}

void MoveGenerator::GenerateSlidingMoves(int startSquare, int piece, bool genOnlyAttacks = false)
{
	int startDirIndex = (BoardDrawer::Piece::isType((Board::Piece)piece, Board::Piece::Bishop)) ? 4 : 0;
	int endDirIndex = (BoardDrawer::Piece::isType((Board::Piece)piece, Board::Piece::Rook)) ? 4 : 8;

	for (int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++)
	{
		bool isPin = false;
		bool genComplete = false;
		for (int n = 0; n < NumSquaresToEdge[startSquare][directionIndex]; n++)
		{
			int targetSquare = startSquare + DirectionOffsets[directionIndex] * (n+1);
			int pieceOnTargetSquare = board.squares[targetSquare];

			//Blocked by friendly piece, so can't move further
			if (BoardDrawer::Piece::isColor( (Board::Piece)pieceOnTargetSquare, board.getFriendlyColor(piece)))
			{
				if (genOnlyAttacks)
				{
					Board::Move new_move(startSquare, targetSquare, Board::Move::NoFlag);
					moves.push_back(new_move);
				}
				break;
			}
			Board::Move new_move(startSquare, targetSquare, Board::Move::NoFlag);
			moves.push_back(new_move);
			/*if (!genOnlyAttacks)
			{

				if (!GenerateCheck(Board::getColor(piece)))
				{
					moves.push_back(new_move);
				}
				else
				{
					for (int attack : opponentAttacks)
					{
						if (targetSquare == attack)
						{
							if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
							{
								new_move.MoveFlag = Board::Move::EnPassantCaptureFlag;
							}
							moves.push_back(new_move);
							genComplete = true;
							break;
						}
					}
				}
			}
			else
			{
				if (!isPin)
				{
					moves.push_back(new_move);
				}
				else
				{
					opponentPins.push_back(targetSquare);
					continue;
				}
			}
			*/

			//Can't move any further after capturing opponent's piece
			if (!genOnlyAttacks)
			{
				if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)) && !genComplete)
				{
					moves.pop_back();
					new_move.MoveFlag = Board::Move::EnPassantCaptureFlag;
					moves.push_back(new_move);
					break;
				}
			}
			else
			{
				if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
				{
					moves.pop_back();
					new_move.MoveFlag = Board::Move::EnPassantCaptureFlag;
					moves.push_back(new_move);
					if (Board::getType(pieceOnTargetSquare) != Board::King)
					{
						isPin = true;
						break;
					}
				}
			}
			
		}
	}
}

void MoveGenerator::GenerateKingMoves(int startSquare, int piece)
{
	for (int directionIndex = 0; directionIndex < 8; directionIndex++)
	{
		bool targetSquareIsAttacked = false;
		for (int n = 0; n < std::min(NumSquaresToEdge[startSquare][directionIndex], 1); n++)
		{
			int targetSquare = startSquare + DirectionOffsets[directionIndex] * (n + 1);
			int pieceOnTargetSquare = board.squares[targetSquare];

			//Blocked by friendly piece, so can't move further
			if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getFriendlyColor(piece)))
			{
				break;
			}
			for (int attack : opponentAttacks)
			{
				if (attack == targetSquare)
				{
					targetSquareIsAttacked = true;
					break;
				}
			}

			if (targetSquareIsAttacked)
			{
				break;
			}

			Board::Move new_move(startSquare, targetSquare, Board::Move::NoFlag);
			moves.push_back(new_move);

			//Can't move any further after capturing opponent's piece
			if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
			{
				moves.pop_back();
				new_move.MoveFlag = Board::Move::EnPassantCaptureFlag;
				moves.push_back(new_move);
				break;
			}
		}
	}
}

void MoveGenerator::GenerateCastlingMoves(int startSquare, int piece)
{
	int castlingIndex;
	int kingIndex;
	int pieceColor = Board::getColor(piece);
	pieceColor == Board::Piece::White ? castlingIndex = 0 : castlingIndex = 2;
	pieceColor == Board::Piece::White ? kingIndex = 0 : kingIndex = 1;
	bool blocked = false;

	GenerateCheck(pieceColor);

	if (board.kingMoves[kingIndex] != 0 || board.isInCheck)
	{
		board.castlingRight[castlingIndex] = false;
		board.castlingRight[castlingIndex+1] = false;
		return;
	}
	else
	{
		board.castlingRight[castlingIndex] = true;
		board.castlingRight[castlingIndex + 1] = true;
	}

	if (board.rookMoves[castlingIndex] != 0 or board.squares[board.rookSquares[castlingIndex]] != (Board::Rook | pieceColor))
	{
		board.castlingRight[castlingIndex] = false;
	}
	else
	{
		board.castlingRight[castlingIndex] = true;
	}

	if (board.rookMoves[castlingIndex+1] != 0 or board.squares[board.rookSquares[castlingIndex+1]] != (Board::Rook | pieceColor))
	{
		board.castlingRight[castlingIndex + 1] = false;
	}
	else
	{
		board.castlingRight[castlingIndex + 1] = true;
	}
	
	for (int directionIndex = 0; directionIndex < 2; directionIndex++)
	{
		blocked = false;
		if (board.castlingRight[castlingIndex+directionIndex] != true)
		{
			continue;
		}
		for (int n = 0; n < 2; n++)
		{
			int targetSquare = startSquare + CastlingDirectionOffsets[directionIndex] * (n + 1);
			int pieceOnTargetSquare = board.squares[targetSquare];

			//Blocked by friendly piece, so can't move further
			if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getFriendlyColor(piece)) ||
				BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
			{
				blocked = true;
				break;
			}
			for (int attack : opponentAttacks)
			{
				if (attack == targetSquare)
				{
					blocked = true;
					break;
				}
			}
			if (blocked)
			{
				break;
			}
			if (directionIndex == 1)
			{
				int targetSquare = startSquare + CastlingDirectionOffsets[directionIndex] * (n + 2);
				int pieceOnTargetSquare = board.squares[targetSquare];
				if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getFriendlyColor(piece)) ||
					BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
				{
					blocked = true;
					break;
				}
			}
		}
		if (!blocked)
		{
			int targetSquare = startSquare + CastlingDirectionOffsets[directionIndex] * 2;
			Board::Move new_move(startSquare, targetSquare, Board::Move::CastleFlag);
			moves.push_back(new_move);
		}
	}
}

void MoveGenerator::GenerateKnightMoves(int startSquare, int piece, int file, int rank)
{
	for (int directionIndex = 0; directionIndex < 8; directionIndex++)
	{
		if (GenerateCheck(Board::getColor(piece)))
		{
			//break;
		}
		for (int n = 0; n < 1; n++)
		{
			if ((file == 0 && (KnightDirectionOffsets[directionIndex] == -17 or KnightDirectionOffsets[directionIndex] == 15)) ||
				((file == 0 or file == 1) && (KnightDirectionOffsets[directionIndex] == -10 or KnightDirectionOffsets[directionIndex] == 6)) ||
				(file == 7 && (KnightDirectionOffsets[directionIndex] == -15 or KnightDirectionOffsets[directionIndex] == 17)) ||
				((file == 7 or file == 6) && (KnightDirectionOffsets[directionIndex] == -6 or KnightDirectionOffsets[directionIndex] == 10)))
			{
				break;
			}

			if ((rank == 0 or rank == 1) && (KnightDirectionOffsets[directionIndex] == -15 or KnightDirectionOffsets[directionIndex] == -17) ||
				(rank == 7 or rank == 6) && (KnightDirectionOffsets[directionIndex] == 15 or KnightDirectionOffsets[directionIndex] == 17) ||
				rank == 7 && (KnightDirectionOffsets[directionIndex] == 10 or KnightDirectionOffsets[directionIndex] == 6) ||
				rank == 0 && (KnightDirectionOffsets[directionIndex] == -10 or KnightDirectionOffsets[directionIndex] == -6))
			{
				break;
			}

			int targetSquare = startSquare + KnightDirectionOffsets[directionIndex] * (n+1);
			int pieceOnTargetSquare = board.squares[targetSquare];

			//Blocked by friendly piece, so can't move further
			if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getFriendlyColor(piece)))
			{
				continue;
			}
			Board::Move new_move(startSquare, targetSquare, Board::Move::NoFlag);
			moves.push_back(new_move);

			//Can't move any further after capturing opponent's piece
			if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
			{
				moves.pop_back();
				new_move.MoveFlag = Board::Move::EnPassantCaptureFlag;
				moves.push_back(new_move);
				break;
			}
		}
	}
}

void MoveGenerator::GeneratePawnMoves(int startSquare, int piece, bool generateOnlyAttacks, int rank, int file = 0)
{
	int pieceColor = Board::getColor(piece);
	int startDirIndex = (BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::White)) ? 4 : 0;
	int endDirIndex = (BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::White)) ? 6 : 2;
	(BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::White) && rank == 6) ? endDirIndex = 7 :
	(BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::Black) && rank == 1) ? endDirIndex = 3 : 0;

	if (generateOnlyAttacks)
	{
		int startDirIndex = (BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::White)) ? 5 : 1;
		int endDirIndex = (BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::White)) ? 6 : 2;

		for (int directionIndex = startDirIndex; directionIndex <= endDirIndex; directionIndex++)
		{
			int targetSquare = startSquare + PawnDirectionOffsets[directionIndex];
			int pieceOnTargetSquare = board.squares[targetSquare];

			if ((PawnDirectionOffsets[directionIndex] == 7 && file != 0) || (PawnDirectionOffsets[directionIndex] == 9 && file != 7) ||
					(PawnDirectionOffsets[directionIndex] == -7 && BoardHelper::indexToFile(startSquare) != 7) || 
					(PawnDirectionOffsets[directionIndex] == -9 && BoardHelper::indexToFile(startSquare) != 0))
			{
				
				Board::Move new_move0(startSquare, targetSquare, Board::Move::EnPassantCaptureFlag);
				moves.push_back(new_move0);
			}
		}
		return;
	}

	for (int directionIndex = startDirIndex; directionIndex <= endDirIndex; directionIndex++)
	{
		if (GenerateCheck(Board::getColor(piece)))
		{
			//break;
		}
		for (int n = 0; n < 1; n++)
		{
			int targetSquare = startSquare + PawnDirectionOffsets[directionIndex] * (n + 1);
			int pieceOnTargetSquare = board.squares[targetSquare];

			//Blocked by friendly piece, so can't move further
			if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getFriendlyColor(piece)))
			{
				break;
			}

			if (PawnDirectionOffsets[directionIndex] == 8 || PawnDirectionOffsets[directionIndex] == -8)
			{
				if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
				{
					break;
				}
			}

			if (((rank == 1 && BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::White)) ||
				(rank == 6 && BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::Black))) && 
				(PawnDirectionOffsets[directionIndex] == 8 || PawnDirectionOffsets[directionIndex] == -8))
			{
				Board::Move new_move(startSquare, targetSquare, Board::Move::PromoteToQueenFlag);
				moves.push_back(new_move);
				Board::Move new_move2(startSquare, targetSquare, Board::Move::PromoteToKnightFlag);
				moves.push_back(new_move2);
				Board::Move new_move3(startSquare, targetSquare, Board::Move::PromoteToBishopFlag);
				moves.push_back(new_move3);
				Board::Move new_move4(startSquare, targetSquare, Board::Move::PromoteToRookFlag);
				moves.push_back(new_move4);
				break;
			}
			
			//Can't move any further after capturing opponent's piece
			if ((BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)) || targetSquare == board.possibleEnPassantSquare) &&
				((PawnDirectionOffsets[directionIndex] == 7 && file != 0) || (PawnDirectionOffsets[directionIndex] == 9 && file != 7) ||
					(PawnDirectionOffsets[directionIndex] == -7 && BoardHelper::indexToFile(startSquare) != 7) || 
					(PawnDirectionOffsets[directionIndex] == -9 && BoardHelper::indexToFile(startSquare) != 0)))
			{
				if ((rank == 1 && BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::White)) ||
					(rank == 6 && BoardDrawer::Piece::isColor((Board::Piece)piece, Board::Piece::Black)))
				{
					Board::Move new_move(startSquare, targetSquare, Board::Move::PromoteToQueenFlag);
					moves.push_back(new_move);
					Board::Move new_move2(startSquare, targetSquare, Board::Move::PromoteToKnightFlag);
					moves.push_back(new_move2);
					Board::Move new_move3(startSquare, targetSquare, Board::Move::PromoteToBishopFlag);
					moves.push_back(new_move3);
					Board::Move new_move4(startSquare, targetSquare, Board::Move::PromoteToRookFlag);
					moves.push_back(new_move4);
					break;
				}
				else
				{
					Board::Move new_move0(startSquare, targetSquare, Board::Move::EnPassantCaptureFlag);
					moves.push_back(new_move0);
					break;
				}
				
			}
			else if ((PawnDirectionOffsets[directionIndex] == 7 || PawnDirectionOffsets[directionIndex] == -7 ||
					  PawnDirectionOffsets[directionIndex] == 9 || PawnDirectionOffsets[directionIndex] == -9))
			{
				break;
			}
			else if (BoardDrawer::Piece::isColor((Board::Piece)pieceOnTargetSquare, board.getOpponentColor(piece)))
			{
				break;
			}
			else if ((PawnDirectionOffsets[directionIndex] == -16 || PawnDirectionOffsets[directionIndex] == 16))
			{
				if (board.squares[targetSquare - PawnDirectionOffsets[directionIndex] / 2] == Board::Piece::None)
				{
					Board::Move new_move(startSquare, targetSquare, Board::Move::PawnTwoUpFlag);
					moves.push_back(new_move);
					break;
				}
				else
				{
					break;
				}
				
			}

			Board::Move new_move(startSquare, targetSquare, Board::Move::NoFlag);
			moves.push_back(new_move);
			
		}
	}
}

void MoveGenerator::GenerateOpponentAttacks(int color)
{
	this->opponentAttacks.clear();
	int opponentColor = Board::getOpponentColor(color);

	std::vector<Board::Move> opponentMoves = GenerateMoves(opponentColor, true);

	for (Board::Move& move : opponentMoves)
	{
		this->opponentAttacks.push_back(move.targetSquare);
	}
}

bool MoveGenerator::GenerateCheck(int color)
{
	int kingIndex = color == Board::Piece::White ? 0 : 1;
	int inCheckAttacks = 0;

	int king = board.kings[kingIndex][0];

	for (int attack : opponentAttacks)
	{
		if (attack == king)
		{
			inCheckAttacks++;
			break;
		}
	}
	if (inCheckAttacks == 0)
	{
		board.isInCheck = false;
		return false;
	}
	else
	{
		board.isInCheck = true;
		return true;
	}
}

bool MoveGenerator::isMove(Board::Move& Move)
{
	for (auto& move : this->moves)
	{
		if (move.isSame(Move))
		{
			Move.MoveFlag = move.MoveFlag;
			return true;
		}
	}
	return false;
}

std::vector<Board::Move>& MoveGenerator::getMovesForSquare(int index)
{
	movesForPiece.clear();

	for (auto& move : moves)
	{
		if (move.startSquare == index)
		{
			movesForPiece.push_back(move);
		}
	}
	return movesForPiece;
}
