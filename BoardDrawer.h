#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Board.h"

class BoardDrawer
{
public:
	struct Square
	{
		int id;
		sf::RectangleShape sprite;
		Square(int id, sf::Color color, sf::Vector2f pos, float size) : id(id)
		{
			sprite.setFillColor(color);
			sprite.setSize(sf::Vector2f(size, size));
			sprite.setPosition(pos);
		}
	};
	struct Piece
	{
		sf::Sprite sprite;
		Board::Piece color;
		bool isDragging = false;

		Piece(sf::Vector2f pos, sf::Texture* texture, Board::Piece color) : color(color)
		{
			sprite.setPosition(pos);
			sprite.setTexture(*texture);
			sprite.scale(1.f/2.5f,1.f/2.5f);
		}

		static bool isColor(Board::Piece piece, Board::Piece colorToMove)
		{
			return Board::getColor(piece) == colorToMove;
		}

		static bool isSlidingPiece(Board::Piece piece)
		{
			return Board::getType(piece) == Board::Piece::Queen || 
				Board::getType(piece) == Board::Piece::Bishop ||
				Board::getType(piece) == Board::Piece::Rook;
		}

		static bool isType(Board::Piece piece, Board::Piece type)
		{
			return Board::getType(piece) == type;
		}

	};
	BoardDrawer(int* Squares);

	void draw(sf::RenderWindow& window);
	void redrawPieces(int* Squares);

	void highlightMoves(std::vector<Board::Move>& moves);
	void highlightSquares(std::vector<int>& squares);
	void unhighlightMoves();

	std::vector<Piece>& getPieces() { return this->pieces; };
	std::vector<Square>& getSquares() { return this->squares; };
private:
	int fileRankToIndex(int file, int rank) 
	{
		return rank * 8 + file;
	}
private:
	const sf::Color ligCol = sf::Color(92, 186, 179), darkCol = sf::Color(72, 129, 124);
	const int squareSize = 66;
	std::vector<Square> squares;
	std::vector<Piece> pieces;
	sf::IntRect pawnRect;
	sf::Texture pieses_texture;
};

