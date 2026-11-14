#include "Board.h"

Board::Board()
{
	this->squares = new int[64] {0};

	loadPositionFromFEN(FENs[0]); //8 - promotion test, 7 - enPassant + promotion, 0 - start position
	//loadPositionFromFEN(testFen);

	for (int i = 0; i < 2; i++)
	{
		std::vector<int> colorPawns;
		pawns.push_back(colorPawns);
		std::vector<int> colorKnights;
		knights.push_back(colorKnights);
		std::vector<int> colorBishops;
		bishops.push_back(colorBishops);
		std::vector<int> colorRooks;
		rooks.push_back(colorRooks);
		std::vector<int> colorQueens;
		queens.push_back(colorQueens);
		std::vector<int> colorKings;
		kings.push_back(colorKings);
	}
	this->updatePieceBoards();
}

Board::Board(std::string FEN)
{
	this->squares = new int[64] {0};

	loadPositionFromFEN(FEN);
}

void Board::makeMove(Move& move)
{
	this->moveID++;
	this->movesCount++;
	int enpassantTargetIndex = 0;
	int enpassantOffset = 0;
	bool isPush = false;
	bool isPromotion = (move.MoveFlag & 0b1110) >= 4;

	if (move.MoveFlag == Move::PawnTwoUpFlag)
	{
		isPush = true;
		if (getColor(squares[move.startSquare]) == Piece::White)
		{
			enpassantOffset = -8;
		}
		else
		{
			enpassantOffset = 8;
		}
		this->possibleEnPassantSquare = move.startSquare + enpassantOffset;
		EnPassant new_ep(moveID, possibleEnPassantSquare);
		this->enpassant_memory.push_back(new_ep);

		squares[move.targetSquare] = squares[move.startSquare];
	}
	else if (move.MoveFlag == Move::EnPassantCaptureFlag)
	{
		//if piece is moving on the possible enPassant square
		//and it's a pawn
		if (move.targetSquare == possibleEnPassantSquare && getType(squares[move.startSquare]) == Board::Piece::Pawn)
		{
			//if moving piece is White -> enPassant target is below the Black Pawn
			//else it's above
			enpassantTargetIndex = getColor(squares[move.startSquare]) == Piece::White ? move.targetSquare + 8 : move.targetSquare - 8;

			//capture the black pawn
			this->captures.push_back(squares[enpassantTargetIndex]);
			squares[enpassantTargetIndex] = 0;

			//updating enpassant moves table
			this->enpassant_moves.push_back(move);
		}
		else
		{
			//just capturing the piece on the target square
			this->captures.push_back(squares[move.targetSquare]);
		}
		//moving the piece on the target square from start square
		squares[move.targetSquare] = squares[move.startSquare];
	}
	else if (isPromotion)
	{
		if (isCapturePromotion(move))
		{
			move.MoveFlag |= Move::EnPassantCaptureFlag;
			//just capturing the piece on the target square
			this->captures.push_back(squares[move.targetSquare]);
		}
		switch ((move.MoveFlag & 0b1110))
		{
		case Move::PromoteToQueenFlag:
			(squares[move.targetSquare]) = getColor(squares[move.startSquare]) | Piece::Queen;
			break;
		case Move::PromoteToBishopFlag:
			(squares[move.targetSquare]) = getColor(squares[move.startSquare]) | Piece::Bishop;
			break;
		case Move::PromoteToKnightFlag:
			(squares[move.targetSquare]) = getColor(squares[move.startSquare]) | Piece::Knight;
			break;
		case Move::PromoteToRookFlag:
			(squares[move.targetSquare]) = getColor(squares[move.startSquare]) | Piece::Rook;
			break;
		default:
			break;
		}
	}
	else if (move.MoveFlag == Move::CastleFlag)
	{
		switch (move.targetSquare)
		{
		case 62:
			squares[move.targetSquare] = squares[move.startSquare];

			squares[move.targetSquare - 1] = squares[63];
			squares[63] = 0;
			break;
		case 58:
			squares[move.targetSquare] = squares[move.startSquare];

			squares[move.targetSquare + 1] = squares[56];
			squares[56] = 0;
			break;
		case 2:
			squares[move.targetSquare] = squares[move.startSquare];

			squares[move.targetSquare + 1] = squares[0];
			squares[0] = 0;
			break;
		case 6:
			squares[move.targetSquare] = squares[move.startSquare];

			squares[move.targetSquare - 1] = squares[7];
			squares[7] = 0;
			break;
		default:
			break;
		}
	}
	else
	{
		squares[move.targetSquare] = squares[move.startSquare];
	}

	if (getType(squares[move.startSquare]) == Piece::King)
	{
		if (getColor(squares[move.startSquare]) == Piece::White)
		{
			this->kingMoves[0]++;
		}
		else
		{
			this->kingMoves[1]++;
		}
	}
	if (getType(squares[move.startSquare]) == Piece::Rook)
	{
		if (getColor(squares[move.startSquare]) == Piece::White)
		{
			if (move.startSquare == 63)
			{
				this->rookMoves[0]++;
			}
			else if (move.startSquare == 56)
			{
				this->rookMoves[1]++;
			}
		}
		else
		{
			if (move.startSquare == 7)
			{
				this->rookMoves[2]++;
			}
			else if (move.startSquare == 0)
			{
				this->rookMoves[3]++;
			}
		}
	}

	squares[move.startSquare] = 0;

	this->made_moves.push_back(move);
	
	//this->printBoard();

	if (!isPush)
	{
		possibleEnPassantSquare = -1;
	}

	#if 1
	if (move.MoveFlag == move.CastleFlag)
	{
		this->updatePieceBoards(Piece::King);
		this->updatePieceBoards(Piece::Rook);
	}
	else if (isPromotion)
	{
		this->updatePieceBoards();
	}
	else if ((move.MoveFlag & 1) == move.EnPassantCaptureFlag)
	{
		//this->updatePieceBoards(getType(captures.back()));
		//this->updatePieceBoards(getType(squares[move.targetSquare]));

		this->updatePieceBoards();
	}
	else
	{
		this->updatePieceBoards(getType(squares[move.targetSquare]));
	}
	#endif

	int colorIndex = colorToMove == Piece::White ? 0 : 1;
	this->myKingSquare = kings[colorIndex][0];

	this->colorToMove = this->colorToMove == Piece::White ? Piece::Black : Piece::White;
	
}

void Board::unmakeMove(Move& move)
{

	//possibleEnPassantSquare = -1;
	int type = move.MoveFlag & 0b1110;
	if (move.MoveFlag < 0)
	{
		int x = 6;
	}
	bool isEnPassant = false;
	bool isPromotion = type == move.PromoteToBishopFlag || type == move.PromoteToKnightFlag || type == move.PromoteToQueenFlag || type == move.PromoteToRookFlag;

	if (isPromotion)
	{ 
		(squares[move.startSquare]) = getColor(squares[move.targetSquare]) | Piece::Pawn;
		squares[move.targetSquare] = 0;

		if ((move.MoveFlag & 1) == Move::EnPassantCaptureFlag)
		{
			squares[move.targetSquare] = this->captures.back();
			this->captures.pop_back();
		}

	}
	else if (move.MoveFlag == Move::EnPassantCaptureFlag)
	{
		//move the piece back to its start square from the target square
		squares[move.startSquare] = (squares[move.targetSquare]);
		for (int i = 0; i < enpassant_moves.size(); i++)
		{
			//if move is one of the previous enPassant moves 
			if (move.isSame(enpassant_moves[i]))
			{
				//get index of opponent's pawn
				int enpassantIndex = getColor(squares[enpassant_moves[i].startSquare]) == Piece::White ? enpassant_moves[i].targetSquare + 8 : enpassant_moves[i].targetSquare - 8;
				//set the opponent's pawn back to its square
				(squares[enpassantIndex]) = this->captures.back();
				//set the possible enPassant index to previously targeted square below the opponent's pawn
				possibleEnPassantSquare = enpassant_moves[i].targetSquare;
				//clear the target square
				squares[enpassant_moves[i].targetSquare] = 0;
				//erase the captured pawn from captures
				this->captures.pop_back();
				isEnPassant = true;
				//erase this enPassant move from table
				enpassant_moves.erase(enpassant_moves.begin() + i);
				break;
			}
		}
		if (!isEnPassant)
		{
			squares[move.targetSquare] = this->captures.back();
			this->captures.pop_back();
		}
	}
	else if (move.MoveFlag == Move::PawnTwoUpFlag)
	{
		possibleEnPassantSquare = -1;
		this->enpassant_memory.pop_back();
		squares[move.startSquare] = squares[move.targetSquare];
		squares[move.targetSquare] = 0;
	}
	else if (move.MoveFlag == Move::CastleFlag)
	{
		switch (move.targetSquare)
		{
		case 62:
			squares[move.startSquare] = squares[move.targetSquare];
			squares[move.targetSquare] = 0;

			squares[63] = squares[move.targetSquare - 1];
			squares[move.targetSquare - 1] = 0;
			break;
		case 58:
			squares[move.startSquare] = squares[move.targetSquare];
			squares[move.targetSquare] = 0;

			squares[56] = squares[move.targetSquare + 1];
			squares[move.targetSquare + 1] = 0;
			break;
		case 2:
			squares[move.startSquare] = squares[move.targetSquare];
			squares[move.targetSquare] = 0;

			squares[0] = squares[move.targetSquare + 1];
			squares[move.targetSquare + 1] = 0;
			break;
		case 6:
			squares[move.startSquare] = squares[move.targetSquare];
			squares[move.targetSquare] = 0;

			squares[7] = squares[move.targetSquare - 1];
			squares[move.targetSquare - 1] = 0;
			break;
		default:
			break;
		}
	}
	else
	{
		squares[move.startSquare] = squares[move.targetSquare];
		squares[move.targetSquare] = 0;
	}
	//this->printBoard();

	if (getType(squares[move.startSquare]) == Piece::King)
	{
		if (getColor(squares[move.startSquare]) == Piece::White)
		{
			this->kingMoves[0]--;
		}
		else
		{
			this->kingMoves[1]--;
		}
	}

	if (getType(squares[move.startSquare]) == Piece::Rook)
	{
		if (getColor(squares[move.startSquare]) == Piece::White)
		{
			if (move.startSquare == 63)
			{
				this->rookMoves[0]--;
			}
			else if (move.startSquare == 56)
			{
				this->rookMoves[1]--;
			}
		}
		else
		{
			if (move.startSquare == 7)
			{
				this->rookMoves[2]--;
			}
			else if (move.startSquare == 0)
			{
				this->rookMoves[3]--;
			}
		}
	}
	#if 1
	if (move.MoveFlag == move.CastleFlag)
	{
		this->updatePieceBoards(Piece::King);
		this->updatePieceBoards(Piece::Rook);
	}
	else if (isPromotion)
	{
		this->updatePieceBoards();
	}
	else if ((move.MoveFlag & 1) == move.EnPassantCaptureFlag)
	{
		this->updatePieceBoards(getType(squares[move.targetSquare]));
		this->updatePieceBoards(getType(squares[move.startSquare]));
	}
	else
	{
		this->updatePieceBoards(getType(squares[move.startSquare]));
	}
	#endif
	this->colorToMove = this->colorToMove == Piece::White ? Piece::Black : Piece::White;
	this->moveID--;

	for (EnPassant& ep : this->enpassant_memory)
	{
		if (ep.moveID == this->moveID)
		{
			possibleEnPassantSquare = ep.enPassantSquare;
			break;
		}
	}
}

void Board::unmakeMove()
{
	if (made_moves.empty())
	{
		return;
	}
	this->unmakeMove(this->made_moves.back());
	this->made_moves.pop_back();
}

void Board::printBoard()
{
	system("cls");
	for (int i = 1; i <= 64; i++)
	{
		std::cout << " " << squares[i - 1] << " ";
		if (i%8==0)
		{
			std::cout << std::endl;
		}
	}
}

void Board::loadPositionFromFEN(std::string fen)
{
	// Define the mapping of characters to enum values
	std::map<char, Piece> pieceMap = 
	{
		{'k', Piece::King},
		{'q', Piece::Queen},
		{'r', Piece::Rook},
		{'b', Piece::Bishop},
		{'n', Piece::Knight},
		{'p', Piece::Pawn}
	};

	int file = 0, rank = 0;
	std::string fenBoard = fen;

	for (char symbol : fenBoard)
	{
		if (symbol == '/')
		{
			file = 0;
			rank++;
		}
		else
		{
			if (isdigit(symbol))
			{
				file += charDigitToInt(symbol);
			}
			else
			{
				int pieceColor = isupper(symbol) ? Piece::White : Piece::Black;
				int pieceType = pieceMap[tolower(symbol)];
				int index = (rank * 8 + file);
				this->squares[index] = pieceColor | pieceType;
				file++;
			}
		}
	}

}

bool Board::isOpponentTeam(int index, Piece color)
{
	if (getColor(squares[index]) == color)
	{
		return false;
	}
	return true;
}

bool Board::isCapturePromotion(Move& move)
{
	if (squares[move.targetSquare] != Board::Piece::None)
	{
		return true;
	}
	return false;
}

void Board::updatePieceBoards()
{
	for (int i = 0; i < 2; i++)
	{
		pawns[i].clear();
		knights[i].clear();
		bishops[i].clear();
		rooks[i].clear();
		queens[i].clear();
		kings[i].clear();
	}

	for (int squareIndex = 0; squareIndex < 64; squareIndex++)
	{
		if (getType(squares[squareIndex]) == Piece::None)
		{
			continue;
		}
		int colorIndex = getColor(squares[squareIndex]) == Piece::White ? 0 : 1;
		switch (getType(squares[squareIndex]))
		{
		case Piece::Pawn:
			pawns[colorIndex].push_back(squareIndex);
			break;
		case Piece::Knight:
			knights[colorIndex].push_back(squareIndex);
			break;
		case Piece::Bishop:
			bishops[colorIndex].push_back(squareIndex);
			break;
		case Piece::Rook:
			rooks[colorIndex].push_back(squareIndex);
			break;
		case Piece::Queen:
			queens[colorIndex].push_back(squareIndex);
			break;
		case Piece::King:
			kings[colorIndex].push_back(squareIndex);
			break;
		default:
			break;
		}
	}
}

void Board::updatePieceBoards(int pieceType)
{
	switch (pieceType)
	{
	case Piece::Pawn:
		for (int i = 0; i < 2; i++)
		{
			pawns[i].clear();
		}
		for (int squareIndex = 0; squareIndex < 64; squareIndex++)
		{
			if (getType(squares[squareIndex]) == Piece::None)
			{
				continue;
			}
			int colorIndex = getColor(squares[squareIndex]) == Piece::White ? 0 : 1;
			if (getType(squares[squareIndex]) == Piece::Pawn)
			{
				pawns[colorIndex].push_back(squareIndex);
			}
		}
		break;
	case Piece::Knight:
		for (int i = 0; i < 2; i++)
		{
			knights[i].clear();
		}
		for (int squareIndex = 0; squareIndex < 64; squareIndex++)
		{
			if (getType(squares[squareIndex]) == Piece::None)
			{
				continue;
			}
			int colorIndex = getColor(squares[squareIndex]) == Piece::White ? 0 : 1;
			if (getType(squares[squareIndex]) == Piece::Knight)
			{
				knights[colorIndex].push_back(squareIndex);
			}
		}
		break;
	case Piece::Bishop:
		for (int i = 0; i < 2; i++)
		{
			bishops[i].clear();
		}
		for (int squareIndex = 0; squareIndex < 64; squareIndex++)
		{
			if (getType(squares[squareIndex]) == Piece::None)
			{
				continue;
			}
			int colorIndex = getColor(squares[squareIndex]) == Piece::White ? 0 : 1;
			if (getType(squares[squareIndex]) == Piece::Bishop)
			{
				bishops[colorIndex].push_back(squareIndex);
			}
		}
		break;
	case Piece::Rook:
		for (int i = 0; i < 2; i++)
		{
			rooks[i].clear();
		}
		for (int squareIndex = 0; squareIndex < 64; squareIndex++)
		{
			if (getType(squares[squareIndex]) == Piece::None)
			{
				continue;
			}
			int colorIndex = getColor(squares[squareIndex]) == Piece::White ? 0 : 1;
			if (getType(squares[squareIndex]) == Piece::Rook)
			{
				rooks[colorIndex].push_back(squareIndex);
			}
		}
		break;
	case Piece::Queen:
		for (int i = 0; i < 2; i++)
		{
			queens[i].clear();
		}
		for (int squareIndex = 0; squareIndex < 64; squareIndex++)
		{
			if (getType(squares[squareIndex]) == Piece::None)
			{
				continue;
			}
			int colorIndex = getColor(squares[squareIndex]) == Piece::White ? 0 : 1;
			if (getType(squares[squareIndex]) == Piece::Queen)
			{
				queens[colorIndex].push_back(squareIndex);
			}
		}
		break;
	case Piece::King:
		for (int i = 0; i < 2; i++)
		{
			kings[i].clear();
		}
		for (int squareIndex = 0; squareIndex < 64; squareIndex++)
		{
			if (getType(squares[squareIndex]) == Piece::None)
			{
				continue;
			}
			int colorIndex = getColor(squares[squareIndex]) == Piece::White ? 0 : 1;
			if (getType(squares[squareIndex]) == Piece::King)
			{
				kings[colorIndex].push_back(squareIndex);
			}
		}
		break;
	default:
		break;
	}
}

void Board::printPieceBoard(int pieceType, int color)
{
	int colorIndex = color == Piece::White ? 0 : 1;
	switch (pieceType)
	{
	case Piece::Pawn:
		for (int pawn : pawns[colorIndex])
		{
			std::cout << pawn << std::endl;
		}
		break;
	case Piece::Knight:
		for (int pawn : knights[colorIndex])
		{
			std::cout << pawn << std::endl;
		}
		break;
	case Piece::Bishop:
		for (int pawn : bishops[colorIndex])
		{
			std::cout << pawn << std::endl;
		}
		break;
	case Piece::Rook:
		for (int pawn : rooks[colorIndex])
		{
			std::cout << pawn << std::endl;
		}
		break;
	case Piece::Queen:
		for (int pawn : queens[colorIndex])
		{
			std::cout << pawn << std::endl;
		}
		break;
	case Piece::King:
		for (int pawn : kings[colorIndex])
		{
			if (pawn == 11)
			{
				break;
			}
			std::cout << pawn << std::endl;
		}
		break;
	default:
		break;
	}
	
}

Board::Piece Board::getFriendlyColor(int piece)
{
	if (getColor(piece) == Board::Piece::White)
	{
		return Board::Piece::White;
	}
	else if (getColor(piece) == Board::Piece::Black)
	{
		return Board::Piece::Black;
	}
	else
	{
		return Board::Piece::None;
	}
}

Board::Piece Board::getOpponentColor(int piece)
{
	if (getColor(piece) == Board::Piece::White)
	{
		return Board::Piece::Black;
	}
	else if (getColor(piece) == Board::Piece::Black)
	{
		return Board::Piece::White;
	}
	else
	{
		return Board::Piece::None;
	}

}
