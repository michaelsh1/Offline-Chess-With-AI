#pragma once
#include "Board.hpp"
#include <stack>

#ifndef _AI
#define _AI

class Evaluate
{
public:
    int evaluate(Board board, game_outcomes outcome, int depth);

private:

    int evaluateMaterial(Board &board);

    int evaluateSpaceAdvantage(Board &board);

    int evaluateOptimalSquares(Board &board);

    int ratePieceSquare(Board &board, loc::Square64 square);

    bool isEndgame(Board &board);
};

class ArtInt
{
public:
    int negaMax(Board &board, int depth, int alpha, int beta);

    move::MoveCell generateComputerMove(Board board, int &promote_to, int depth);

private:
    std::stack<Board> position_stack;
};

#endif // !_AI