#include "BoardDrawer.h"

BoardDrawer::BoardDrawer(int* Squares)
{
	int id = 0;
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 8; file++)
		{
			sf::Color color = ((file + rank) % 2 != 0) ? this->darkCol : this->ligCol;

			sf::Vector2f pos = sf::Vector2f(squareSize*4.f/1.5f + file * squareSize, squareSize * 4.f/1.5f + rank * squareSize);

			id = this->fileRankToIndex(file, rank);

			Square sq(id,color, pos, squareSize);
			this->squares.push_back(sq);
		}
	}

	this->redrawPieces(Squares);
}

void BoardDrawer::draw(sf::RenderWindow& window)
{
	for (auto& square : this->squares)
	{
		window.draw(square.sprite);
	}
	for (auto& piece : this->pieces)
	{
		window.draw(piece.sprite);
	}
}

void BoardDrawer::redrawPieces(int* Squares)
{
	this->pieces.clear();
	for (int i = 0; i < 64; i++)
	{
		Board::Piece color = (Squares[squares[i].id] >> 3 == 1) ? Board::White : Squares[squares[i].id] >> 3 == 2 ? Board::Black : Board::None;
		if (color == Board::Piece::None)
		{
			continue;
		}

		std::string clr = Squares[squares[i].id] >> 3 == 1 ? "w" : "b";
		int piece = Board::Piece(Squares[squares[i].id] & 7);

		sf::Texture* txtr = new sf::Texture();
		txtr->loadFromFile("sprites/" + clr + "-" + std::to_string(piece) + ".png");

		Piece new_piece(squares[i].sprite.getPosition(), txtr, color);
		this->pieces.push_back(new_piece);
	}
}

void BoardDrawer::highlightMoves(std::vector<Board::Move>& moves)
{
	for (auto& move : moves)
	{
		Square *square = &squares[move.targetSquare];
		this->squares[move.targetSquare].sprite.setFillColor(sf::Color(square->sprite.getFillColor().r, square->sprite.getFillColor().g, square->sprite.getFillColor().b, 50));
	}
}

void BoardDrawer::highlightSquares(std::vector<int>& targetSquares)
{
	for (int targetSquare : targetSquares)
	{
		Square* square = &squares[targetSquare];
		this->squares[targetSquare].sprite.setFillColor(sf::Color(square->sprite.getFillColor().r, square->sprite.getFillColor().g, square->sprite.getFillColor().b, 50));
	}
}

void BoardDrawer::unhighlightMoves()
{
	for (auto& square : squares)
	{
		square.sprite.setFillColor(sf::Color(square.sprite.getFillColor().r, square.sprite.getFillColor().g, square.sprite.getFillColor().b, 255));
	}
}
