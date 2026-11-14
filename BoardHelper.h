#pragma once
#include <iostream>
#include <string>

class BoardHelper
{
public:
	BoardHelper() {};

	static int indexToFile(int squareIndex) { return squareIndex & 7; };
	static int indexToRank(int squareIndex) { return std::abs((squareIndex >> 3)-8); };

	static std::string indexToSquare(int squareIndex) { std::string ranks[8]{ "a", "b", "c", "d", "e", "f", "g", "h" }; return ranks[indexToFile(squareIndex)] + std::to_string(indexToRank(squareIndex)); };
public:
	
private:
	
};

