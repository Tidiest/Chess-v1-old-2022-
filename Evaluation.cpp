#include "Evaluation.h"

Evaluation::Evaluation(Board& board, MoveGenerator& moveGen) : board(board), moveGen(moveGen)
{
}

int Evaluation::Evaluate()
{
    int whiteMaterial = CountMaterial(Board::WhiteIndex);
    int blackMaterial = CountMaterial(Board::BlackIndex);

    int whiteEvaluation = 0;
    int blackEvaluation = 0;

    whiteEvaluation += whiteMaterial;
    blackEvaluation += blackMaterial;

    int whiteMaterialWithoutPawns = whiteMaterial - board.pawns[Board::WhiteIndex].size() * pawnValue;
    int blackMaterialWithoutPawns = blackMaterial - board.pawns[Board::BlackIndex].size() * pawnValue;

    float whiteEndgamePhaseWeight = EndgamePhaseWeight(whiteMaterialWithoutPawns);
    float blackEndgamePhaseWeight = EndgamePhaseWeight(blackMaterialWithoutPawns);
    whiteEvaluation += ForceKingToCenter(board.kings[Board::WhiteIndex][0], board.kings[Board::BlackIndex][0], blackEndgamePhaseWeight);
    blackEvaluation += ForceKingToCenter(board.kings[Board::BlackIndex][0], board.kings[Board::WhiteIndex][0], whiteEndgamePhaseWeight);

    whiteEvaluation += EvaluatePieceSquareTables(Board::WhiteIndex, blackEndgamePhaseWeight);
    blackEvaluation += EvaluatePieceSquareTables(Board::BlackIndex, whiteEndgamePhaseWeight);

    int evaluation = whiteEvaluation - blackEvaluation;

    moveGen.GenerateCheck(board.colorToMove);
    if (board.isInCheck)
    {
        evaluation -= 5;
    }

    int perspective = board.colorToMove == Board::White ? 1 : -1;

    return evaluation * perspective;
}

int Evaluation::CountMaterial(int colorIndex)
{
    int material = 0;
    material += board.pawns[colorIndex].size() * pawnValue;
    material += board.knights[colorIndex].size() * knightValue;
    material += board.bishops[colorIndex].size() * bishopValue;
    material += board.rooks[colorIndex].size() * rookValue;
    material += board.queens[colorIndex].size() * queenValue;

    return material;
}

int Evaluation::getPieceValue(int piece)
{
    switch (piece)
    {
    case Board::Piece::Pawn:
        return pawnValue;
        break;
    case Board::Piece::Knight:
        return knightValue;
        break;
    case Board::Piece::Bishop:
        return bishopValue;
        break;
    case Board::Piece::Rook:
        return rookValue;
        break;
    case Board::Piece::Queen:
        return queenValue;
        break;
    default:
        return -1;
        break;
    }
}

int Evaluation::ForceKingToCenter(int myKingSquare, int opponentKingSquare, int endGameValue)
{
    int eval = 0;

    //force opponent king away from the center of the board
    //to help force a checkmate in endgame
    int opponentKingsRank = BoardHelper::indexToRank(opponentKingSquare);
    int opponentKingsFile = BoardHelper::indexToFile(opponentKingSquare);

    int oppKingDstToCenterFile = std::max(3 - opponentKingsRank, opponentKingsRank - 4);
    int oppKingDstToCenterRank = std::max(3 - opponentKingsFile, opponentKingsFile - 4);
    int oppDstFromCenter = oppKingDstToCenterFile + oppKingDstToCenterRank;

    eval += oppDstFromCenter;

    int myKingRank = BoardHelper::indexToRank(myKingSquare);
    int myKingFile = BoardHelper::indexToFile(myKingSquare);

    int dstBetweenKingsFile = std::abs(myKingFile - opponentKingsFile);
    int dstBetweenKingsRank = std::abs(myKingRank - opponentKingsRank);
    int dstBetweenKings = dstBetweenKingsFile + dstBetweenKingsRank;

    eval += (14 - dstBetweenKings);

    return int(eval * 10 * endGameValue);
}

int Evaluation::EvaluatePieceSquareTables(int colourIndex, float endgamePhaseWeight) 
{
    int value = 0;
    bool isBlack = colourIndex == Board::BlackIndex;
    value += EvaluatePieceSquareTable(PieceBoard::pawns, board.pawns[colourIndex], isBlack);
    value += EvaluatePieceSquareTable(PieceBoard::rooks, board.rooks[colourIndex], isBlack);
    value += EvaluatePieceSquareTable(PieceBoard::knights, board.knights[colourIndex], isBlack);
    value += EvaluatePieceSquareTable(PieceBoard::bishops, board.bishops[colourIndex], isBlack);
    value += EvaluatePieceSquareTable(PieceBoard::queens, board.queens[colourIndex], isBlack);
    int kingEarlyPhase = PieceBoard::Read(PieceBoard::kingMiddle, board.kings[colourIndex][0], isBlack);
    int kingEndPhase = PieceBoard::Read(PieceBoard::kingEnd, board.kings[colourIndex][0], isBlack);
    value += (int)(kingEarlyPhase * (1 - endgamePhaseWeight));
    value += (int)(kingEndPhase * endgamePhaseWeight);
    //value += PieceSquareTable.Read (PieceSquareTable.kingMiddle, board.KingSquare[colourIndex], isBlack);

    return value;
}

int Evaluation::EvaluatePieceSquareTable(const int* table, const std::vector<int> pieceList, bool isWhite) 
{
    int value = 0;
    for (int i = 0; i < pieceList.size(); i++) 
    {
        value += PieceBoard::Read(table, pieceList[i], isWhite);
    }
    return value;
}