#pragma once

#ifndef PIECE
#define PIECE
#include "Defs.hpp"
#include "Board.hpp"
#include "PreCompiledMoves.hpp"
extern const char kingPrecompiledMoves[64][9][5];
extern const char knightPrecompiledMoves[64][9][5];
extern const char whitePawnPrecompiledMoves[64][3][5];
extern const char blackPawnPrecompiledMoves[64][3][5];
extern const char bishopPrecompiledMoves[64][4][8][5];
extern const char rookPrecompiledMoves[64][4][8][5];

static bool
cmp(const move::MoveCell a, const move::MoveCell b)
{
    return a.type > b.type;
}

class Board;
class Piece;
enum Square120
{
	a1 = 21, b1, c1, d1, e1, f1, g1, h1,
	a2 = 31, b2, c2, d2, e2, f2, g2, h2,
	a3 = 41, b3, c3, d3, e3, f3, g3, h3,
	a4 = 51, b4, c4, d4, e4, f4, g4, h4,
	a5 = 61, b5, c5, d5, e5, f5, g5, h5,
	a6 = 71, b6, c6, d6, e6, f6, g6, h6,
	a7 = 81, b7, c7, d7, e7, f7, g7, h7,
	a8 = 91, b8, c8, d8, e8, f8, g8, h8, EDGE = 0
};

static Square120 SquareArr120[64] =
{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};
namespace behavior
{
    class PieceBehavior
    {
    public:
        bool isPinned(Board board, loc::Square64 square, std::list<move::MoveCell> *moves_list);

        bool isKingInCheck(Board &board, int king_color, std::list<move::MoveCell> *moves_list, int &number_of_attackers);

        bool *calculateAttackedSquares(Board board, int attacking_color);

        bool isSquareAttacked(Board &board, loc::Square64 square, int attacking_color);

        virtual void behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list) = 0;
    };

    class KingBehavior : public PieceBehavior
    {
    public:
        // subclass virtual function implementation \
           returns the availabe squares for the king in a linked list
        virtual void behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list) override;
    };

    class KnightBehavior : public PieceBehavior
    {
    public:
        virtual void behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list) override;
    };

    class BishopBehavior : public PieceBehavior
    {
    public:
        virtual void behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list) override;
    };

    class RookBehavior : public PieceBehavior
    {
    public:
        virtual void behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list) override;
    };

    class QueenBehavior : public PieceBehavior
    {
    public:
        virtual void behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list) override;
    };

    class PawnBehavior : public PieceBehavior
    {
    public:
        virtual void behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list) override;
    };
}

#endif // !PIECE