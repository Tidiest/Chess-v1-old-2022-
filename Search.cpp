#include "Search.h"

SearchAI::SearchAI(MoveGenerator& moveGen, Board& board, Evaluation& evaluator, MoveOrdering& moveOrdering) :
    moveGen(moveGen), board(board), evaluator(evaluator), moveOrdering(moveOrdering),
    bestMove(0,0,0)
{
    bestMove.moveValue = 0;
}

int SearchAI::MoveGenTest(int depth, Board::Move& move, int ply)
{
    if (depth == 0)
    {
        return 1;
    }

    std::vector<Board::Move> moves = moveGen.GenerateLegalMoves();

    int numPositions = 0;
    for (Board::Move& move : moves)
    {
        board.makeMove(move);
        numPositions += MoveGenTest(depth - 1, move, ply);
        board.unmakeMove();
    }
    if (depth == ply - 1)
    {
        //std::cout << BoardHelper::indexToSquare(move.startSquare) << BoardHelper::indexToSquare(move.targetSquare) << ": " << numPositions << std::endl;
    }
    if (depth == ply)
    {
        std::cout << "Moves made and unmade: " << board.movesCount*2 << std::endl;
        board.movesCount = 0;
    }
    return numPositions;
}

int SearchAI::Search(int depth, int maxDepth, int alpha, int beta)
{
    if (depth == 0)
    {
        return evaluator.Evaluate();
    }

    std::vector<Board::Move> moves = moveGen.GenerateLegalMoves();
    moveOrdering.OrderMoves(board, moves);

    if (moves.empty())
    {
        moveGen.GenerateCheck(board.colorToMove);
        if (board.isInCheck)
        {
            return Evaluation::negativeInfinity;
        }
        return 0;
    }

    for (Board::Move& move : moves)
    {
        board.makeMove(move);
        int evaluation = -Search(depth - 1, maxDepth, -beta, -alpha);
        if (evaluation > 900) 
        {
            evaluation--;
        }
        board.unmakeMove();
        if (evaluation >= beta)
        {
            //move was so good, opponent will avoid playing it
            return beta;
        }
        if (evaluation > alpha)
        {
            alpha = evaluation;
            if (depth == maxDepth)
            {
                bestMove = move;
            }
        }
    }
    return alpha;
}

int SearchAI::Search(int depth, int maxDepth = 1)
{
    if (depth == 0)
    {
        return evaluator.Evaluate();
    }

    std::vector<Board::Move> moves = moveGen.GenerateLegalMoves();
    moveOrdering.OrderMoves(board, moves);

    if (moves.empty())
    {
        if (board.isInCheck)
        {
            return Evaluation::negativeInfinity;
        }
        return 0;
    }

    int bestEval = Evaluation::negativeInfinity;

    for (Board::Move& move : moves)
    {
        board.makeMove(move);
        int evaluation = -Search(depth - 1, maxDepth);
        if (bestEval <= evaluation)
        {
            bestEval = evaluation;
            if (depth == maxDepth)
            {
                bestMove = move;
            }
        }
        board.unmakeMove();
    }
    //std::cout << "Beat move for " << board.colorToMove << " is " << bestMove.startSquare << "-" << bestMove.targetSquare << std::endl;
    return bestEval;
}
