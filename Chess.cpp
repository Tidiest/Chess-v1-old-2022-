#include <iostream>
#include <chrono>
#include "BoardDrawer.h"
#include "Board.h"
#include "MoveGenerator.h"
#include "Search.h"
#include "Evaluation.h"

#define release

bool makeRandomMove(MoveGenerator& moveGen, Board& board, BoardDrawer& boardDrawer, Evaluation& evaluator, MoveOrdering& moveOrdering)
{
	SearchAI* new_searcher = new SearchAI(moveGen, board, evaluator, moveOrdering);

	moveGen.GenerateLegalMoves();

	std::vector<int> moveScores;
	int maxScore = Evaluation::negativeInfinity;

	std::cout << "Final eval of search is " << new_searcher->Search(5, 5, Evaluation::negativeInfinity, -Evaluation::negativeInfinity) << " for " << board.colorToMove << std::endl;

	Board::Move bestMove = new_searcher->getBestMove();

	if (bestMove.moveValue == 0)
	{
		moveGen.GenerateLegalMoves();
		if (moveGen.moves.empty())
		{
			return false;
		}
		board.makeMove(moveGen.moves[rand()%moveGen.moves.size()]);
		boardDrawer.redrawPieces(board.squares);
		return true;
	}
	board.makeMove(bestMove);
	boardDrawer.redrawPieces(board.squares);

	return true;
}

int main()
{
	srand(time(NULL));
	const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800;
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chess Bot Coding Adventure 0.8v", sf::Style::Close);

	Board board;
	BoardDrawer boardDrawer(board.getSquares());
	MoveGenerator moveGen(board);
	Evaluation evaluator(board, moveGen);
	MoveOrdering moveOrdering(moveGen);

	sf::Vector2f offset;
	int indexfrom;
	sf::Vector2f posFrom;
	bool play = false;
	bool generatedToDraw = false;

	#ifdef release 
	std::cout << "Release version" << std::endl;
	#else
	std::cout << "Testing version" << std::endl;
	#endif

	while (window.isOpen())
	{
		sf::Event evnt;

		while (window.pollEvent(evnt))
		{
			if (evnt.type == sf::Event::Closed)
				window.close();

			if (evnt.type == sf::Event::KeyPressed)
			{
				if (evnt.key.code == sf::Keyboard::Backspace)
				{
					board.unmakeMove();

					boardDrawer.redrawPieces(board.getSquares());
				}
				#ifdef release
				#else
				if (evnt.key.code == sf::Keyboard::Space)
				{
					//makeRandomMove(moveGen, board, boardDrawer);
					moveGen.GenerateOpponentAttacks(board.colorToMove);
					boardDrawer.highlightSquares(moveGen.opponentAttacks);

					//play = !play;
			}
				#endif
				
			}

			if (evnt.type == sf::Event::KeyReleased)
			{
				#ifdef release
				#else
				if (evnt.key.code == sf::Keyboard::Space)
				{
					//moveGen.GenerateMoves(board);
					boardDrawer.unhighlightMoves();
				}
				#endif
			}

			if (evnt.type == sf::Event::MouseButtonPressed && evnt.mouseButton.button == sf::Mouse::Left) 
			{
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				for (auto& piece : boardDrawer.getPieces())
				{
					if (piece.sprite.getGlobalBounds().contains(mousePos))
					{
						for (auto& square : boardDrawer.getSquares())
						{
							sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
							if (square.sprite.getGlobalBounds().contains(mousePos))
							{
								indexfrom = square.id;
								posFrom = square.sprite.getPosition();
							}
						}
						piece.isDragging = true;
						generatedToDraw = false;
						offset = piece.sprite.getPosition() - mousePos;
					}
				}
				
			}

			if (evnt.type == sf::Event::MouseButtonReleased && evnt.mouseButton.button == sf::Mouse::Left) 
			{
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				bool placed = false;
				for (auto& piece : boardDrawer.getPieces())
				{
					if (piece.sprite.getGlobalBounds().contains(mousePos) && piece.isDragging)
					{
						piece.isDragging = false;
						generatedToDraw = false;
						boardDrawer.unhighlightMoves();
						for (auto& square : boardDrawer.getSquares())
						{
							if (square.sprite.getGlobalBounds().contains(mousePos))
							{
								if (indexfrom == square.id)
								{
									piece.sprite.setPosition(posFrom);
									break;
								}
								//Search legal moves
								moveGen.GenerateLegalMoves();

								if (moveGen.moves.empty())
								{
									std::cout << "Checkmate. You Lose!" << std::endl;
									piece.sprite.setPosition(posFrom);
									break;
								}

								moveGen.moves = moveGen.getMovesForSquare(indexfrom);
								//Create a new move
								Board::Move move(indexfrom, square.id, Board::Move::NoFlag);

								//if target square is empty and is legal move
								//make a move
								if (moveGen.isMove(move))
								{
									placed = true;
									piece.sprite.setPosition(square.sprite.getPosition());
									if (move.MoveFlag >= 6)
									{
										bool isCapture = move.MoveFlag & 1;
										std::cout << "Choose the piece to promote to (q,n,b,r): ";
										char piece;
										std::cin >> piece; std::cout << std::endl;
										bool choosed = false;
										while (!choosed)
										{
											switch (piece)
											{
											case 'q':
												move.MoveFlag = Board::Move::PromoteToQueenFlag;
												choosed = true;
												break;
											case 'n':
												move.MoveFlag = Board::Move::PromoteToKnightFlag;
												choosed = true;
												break;
											case 'b':
												move.MoveFlag = Board::Move::PromoteToBishopFlag;
												choosed = true;
												break;
											case 'r':
												move.MoveFlag = Board::Move::PromoteToRookFlag;
												choosed = true;
												break;
											default:
												std::cout << "Incorrect symbol. Enter again: ";
												std::cin >> piece; std::cout << std::endl;
												break;
											}
										}
										if (isCapture)
										{
											move.MoveFlag |= 1;
										}
									}

									board.makeMove(move);

									boardDrawer.redrawPieces(board.getSquares());

									#if 1
									auto start = std::chrono::high_resolution_clock::now();
									bool botMove = makeRandomMove(moveGen, board, boardDrawer, evaluator, moveOrdering);
									auto stop = std::chrono::high_resolution_clock::now();
									auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
									std::cout << duration.count() << "ms" << std::endl;
									if (!botMove)
									{
										std::cout << "Checkmate. You Win!" << std::endl;
										break;
									}
									#endif
									break;
								}
								//don't make a move
								else
								{
									piece.sprite.setPosition(posFrom);
									break;
								}
							}
							//if out of bounds of the chessboard
							else
							{
								piece.sprite.setPosition(posFrom);
							}
						}
					}
				}
			}
			
		}

		#ifdef release
		#else		
		if (play)
		{
			makeRandomMove(moveGen, board, boardDrawer, evaluator, moveOrdering);
		}
		if (moveGen.moves.empty() && play)
		{
			play = false;
			if (board.colorToMove == Board::Piece::White)
			{
				std::cout << "Black Wins" << std::endl;
			}
			else
			{
				std::cout << "White Wins" << std::endl;
			}
		}
		#endif

		for (auto& piece : boardDrawer.getPieces())
		{
			if (piece.isDragging)
			{
				sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				piece.sprite.setPosition(mousePos + offset);
				if (!generatedToDraw)
				{
					moveGen.moves = moveGen.GenerateLegalMoves();
					generatedToDraw = true;
				}
				boardDrawer.highlightMoves(moveGen.getMovesForSquare(indexfrom));
			}
		}

		window.clear(sf::Color(30, 30, 30));
		
		boardDrawer.draw(window);

		window.display();
		
		//ply test
		#if (0)
		int expectedResultStart[] = { 1, 400, 1486, 62379, 2103487 };
		int expectedResultCustom10[] = {1, 44, 1486, 62379, 2103487};
		int expectedResultCustom11[] = { 1, 48, 2039, 97862, 4085603 };
		int expectedResultCustom12[] = {1, 14, 191, 2812, 43238, 674624, 11030083 };
		int expectedResultCustom13[] = {1, 6, 264, 9467, 422333, 15833292};
		std::vector<int*> expectedResults;
		expectedResults.push_back(expectedResultStart);
		expectedResults.push_back(expectedResultCustom10);
		expectedResults.push_back(expectedResultCustom11);
		expectedResults.push_back(expectedResultCustom12);
		expectedResults.push_back(expectedResultCustom13);
		int tableIndex = 4;
		SearchAI* searchAI = new SearchAI(moveGen, board, evaluator, moveOrdering);
		for (int ply = 0; ply <= 4; ply++)
		{
			auto start = std::chrono::high_resolution_clock::now();
			Board::Move move(0,0,0);
			int result = searchAI->MoveGenTest(ply, move, ply);
			std::cout << "Depth: " << ply << " ply\tResult: " << result << " positions ";
			if (result == expectedResults[tableIndex][ply])
			{
				std::cout << "OK. EXPECTED " << expectedResults[tableIndex][ply] << "(" << result - expectedResults[tableIndex][ply] << ")" << std::endl;
			}
			else
			{
				std::cout << "WRONG. EXPECTED " << expectedResults[tableIndex][ply] << "(" << result - expectedResults[tableIndex][ply] << ")" << std::endl;
			}
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

			std::cout << duration.count() << "ms" << std::endl;
		}
		/*int ply = 4;
		Board::Move move(0, 0, 0);
		int result = searchAI.MoveGenTest(ply, move, ply-1);
		std::cout << "Depth: " << ply << " ply\tResult: " << result << " positions ";
		if (result == expectedResults[tableIndex][ply])
		{
			std::cout << "OK. EXPECTED " << expectedResults[tableIndex][ply] << "(" << result - expectedResults[tableIndex][ply] << ")" << std::endl;
		}
		else
		{
			std::cout << "WRONG. EXPECTED " << expectedResults[tableIndex][ply] << "(" << result - expectedResults[tableIndex][ply] << ")" << std::endl;
		}*/
		int a;
		std::cin >> a;
		break;
		#endif
	}

	return 0;
}