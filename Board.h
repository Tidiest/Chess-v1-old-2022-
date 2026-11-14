#pragma once
#include <iostream>
#include <vector>
#include <map>

class Board
{
public:
	enum Piece
	{
		None, King, Pawn, Knight, Bishop, Rook, Queen,
		White = 8, Black = 16
	};

	struct EnPassant
	{
		int moveID;
		int enPassantSquare;

		EnPassant(int moveID, int square) : moveID(moveID), enPassantSquare(square)
		{

		};
	};

	struct Move
	{
		int moveValue;

		static const int NoFlag = 0b0000;
		static const int EnPassantCaptureFlag = 0b0001;
		static const int CastleFlag = 0b0010;
		static const int PawnTwoUpFlag = 0b0100;
		static const int PromoteToQueenFlag = 0b0110;
		static const int PromoteToRookFlag = 0b1000;
		static const int PromoteToBishopFlag = 0b1010;
		static const int PromoteToKnightFlag = 0b1100;

		int startSquare;
		int targetSquare;
		int MoveFlag;

		Move(int startSquare, int targetSquare, int flag)
		{
			moveValue = (startSquare | targetSquare << 6 | flag << 12);
			this->startSquare = moveValue & 0b0000000000111111;
			this->targetSquare = (moveValue & 0b0000111111000000) >> 6;
			MoveFlag = moveValue >> 12;
		}

		static const Move NullMove;

		bool isSame(const Move& move)
		{
			if (move.startSquare == this->startSquare && move.targetSquare == this->targetSquare)
			{
				return true;
			}
			return false;
		}
	};

	Board();
	Board(std::string FEN);
	int* getSquares() { return this->squares; };
	void makeMove(Move& move);
	void unmakeMove(Move& move);
	void unmakeMove();
	void printBoard();
	bool isEmpty(int index) { return squares[index] == 0; };
	void loadPositionFromFEN(std::string fen);
	bool isOpponentTeam(int index, Piece color);
	bool isCapturePromotion(Move& move);

	static int getColor(int piece) { return piece >> 3 == 1 ? Piece::White : piece >> 3 == 2 ? Piece::Black : Piece::None; };
	static int getType(int piece) { return Piece(piece & 7); };

	void updatePieceBoards();
	void updatePieceBoards(int pieceType);

	void printPieceBoard(int pieceType, int color);

	static Piece getFriendlyColor(int piece);
	static Piece getOpponentColor(int piece);
public:
	static const int WhiteIndex = 0;
	static const int BlackIndex = 1;
	int movesCount = 0;
	int* squares;
	int colorToMove = Piece::White;
	std::vector<Move> made_moves;
	int possibleEnPassantSquare = -1;
	int myKingSquare = -1;
	bool isInCheck = false;
	bool castlingRight[4]{true, true, true, true}; //white|kingside, white|queenside, black|kingside, black|queenside
	int kingMoves[2] = { 0, 0 }; //white, black
	int rookMoves[4] = { 0, 0, 0, 0 }; //white|kingside, white|queenside, black|kingside, black|queenside
	int rookSquares[4] = {63, 56, 7, 0};
	std::vector<std::vector<int>> pawns; //0-white, 1-black | square index
	std::vector<std::vector<int>> knights; //0-white, 1-black | square index
	std::vector<std::vector<int>> bishops; //0-white, 1-black | square index
	std::vector<std::vector<int>> rooks; //0-white, 1-black | square index
	std::vector<std::vector<int>> queens; //0-white, 1-black | square index
	std::vector<std::vector<int>> kings; //0-white, 1-black | square index
private:
	int charDigitToInt(char digit) 
	{
		if (isdigit(digit)) 
		{ // Check if the character is a digit
			return digit - '0';
		}
		else 
		{
			// Handle error case (e.g., character is not a digit)
			// For simplicity, in this example, we return -1 to indicate an error.
			return -1;
		}
	}
private:
	
	const std::string FENs[16] = { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", // w KQkq - 0 1
							"r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1",
							"8/8/8/4p1K1/2k1P3/8/8/8",
							"qrb5/8/8/8/8/8/8/5BRQ",
							"r1bq1b1r/8/8/8/8/8/8/R1BQ1B1R",
							"8/p7/8/P7/8/8/8/8",
							"8/7p/8/8/6P1/pp6/8/Q7",
							"6r1/7P/8/8/8/8/8/8",
							"K4k2/pppppppp/8/8/5P2/pp5p/PPPPPPPP/Q7",
							"r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
							"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R", //w KQ LETS FUCKING GOOOOOOOOOOO	//10 oppening
							"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R", //w KQkq LETS FUCKING GOOOOOOOOOOOOO //11 middlegame
							"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8", //LETS FUCKING GOOOOOOO //12 endgame
							"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1", //w kq - 0 1
							"3bb4/8/3k4/8/4K3/8/8/8", // w - -	//14 king & rook endgame
							"8/3K4/4P3/8/8/8/6k1/7q" //w - - //15 queen vs pawn endgame
	};
	std::string testFen = "8/5K2/7k/8/8/8/5Q1/8"; // w - -
	
	std::vector<int> captures;
	std::vector<Move> enpassant_moves;
	std::vector<EnPassant> enpassant_memory;
	int moveID = 0;
};

