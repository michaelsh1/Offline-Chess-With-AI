#include "AI.hpp"

namespace PieceSquareTable /* https://www.adamberent.com/2019/03/02/piece-square-table/ */
{
    // for black its normal for white its flip
    const int flip[64] = {

        63, 62, 61, 60, 59, 58, 57, 56, 
        55, 54, 53, 52, 51, 50, 49, 48,
        47, 46, 45, 44, 43, 42, 41, 40,
        39, 38, 37, 36, 35, 34, 33, 32,
        31, 30, 29, 28, 27, 26, 25, 24,
        23, 22, 21, 20, 19, 18, 17, 16,
        15, 14, 13, 12, 11, 10,  9,  8,
         7,  6,  5,  4,  3,  2,  1,  0
    };

    const int PawnSquareTable[64] = {

        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 27, 27, 10,  5,  5,
         0,  0,  0, 25, 25,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-25,-25, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    };

    const int KnightSquareTable[64] = {

        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-20,-30,-30,-20,-40,-50
    };

    const int BishopSquareTable[64] = {

        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-40,-10,-10,-40,-10,-20
    };

    const int RookSquareTable[64] = {

        0,  0,  0,  0,  0,  0,  0,  0,
       10, 20, 20, 20, 20, 20, 20, 10,
      -10,  0,  0,  0,  0,  0,  0,-10,
      -10,  0,  0,  0,  0,  0,  0,-10,
      -10,  0,  0,  0,  0,  0,  0,-10,
      -10,  0,  0,  0,  0,  0,  0,-10, 
      -10,  0,  0,  0,  0,  0,  0,-10,
      -30, 30, 40, 10, 10,  0,  0,-30
    };

    const int QueenSquareTable[64] = {

      -40,-20,-20,-10,-10,-20,-20,-40,
      -20,  0,  0,  0,  0,  0,  0,-20,
      -20,  0, 10, 10, 10, 10,  0,-20,
      -10,  0, 10, 10, 10, 10,  0,-10,
        0,  0, 10, 10, 10, 10,  0,-10,
      -20, 10, 10, 10, 10, 10,  0,-20,
      -20,  0, 10,  0,  0,  0,  0,-20,
      -40,-20,-20,-10,-10,-20,-20,-40
    };

    const int KingSquareTable[64] = {

        -60,-80,-80, -2,-20,-80,-80,-60,
        -60,-80,-80, -2,-20,-80,-80,-60,
        -60,-80,-80, -2,-20,-80,-80,-60,
        -60,-80,-80, -2,-20,-80,-80,-60,
        -40,-60,-60, -8,-80,-60,-60,-40,
        -20,-40,-40,-40,-40,-40,-40,-20,
         40, 40,  0,  0,  0,  0, 40, 40,
         40, 60, 20,  0,  0, 20, 60, 40 
    };

    const int KingSquareTableEndgame[64] = {

        -175,-175,-175,-175,-175,-175,-175,-175,
        -175, -50, -50, -50, -50, -50, -50,-175,
        -175, -50,  50,  50,  50,  50, -50,-175,
        -175, -50,  50, 150, 150,  50, -50,-175,
        -175, -50,  50, 100, 100,  50, -50,-175,
        -175, -50,  50,  50,  50,  50, -50,-175,
        -175, -50, -50, -50, -50, -50, -50,-175,
        -175,-175,-175,-175,-175,-175,-175,-175
    };
};

int positions_evaluated = 0;
int Evaluate::evaluateMaterial(Board &board)
{
    int black_ds, white_ds;
    int black_ls, white_ls;
    std::map<int, int> material_map = {
        // black material
        { PieceType::black | PieceType::pawn,   0 },
        { PieceType::black | PieceType::knight, 0 },
        { PieceType::black | PieceType::bishop, 0 },
        { PieceType::black | PieceType::rook,   0 },
        { PieceType::black | PieceType::queen,  0 },

        // white material
        { PieceType::white | PieceType::pawn,   0 },
        { PieceType::white | PieceType::knight, 0 },
        { PieceType::white | PieceType::bishop, 0 },
        { PieceType::white | PieceType::rook,   0 },
        { PieceType::white | PieceType::queen,  0 } };

    std::map<int, int> piece_value = {

        { PieceType::pawn,   100 },
        { PieceType::knight, 300 },
        { PieceType::bishop, 325 },
        { PieceType::rook,   500 },
        { PieceType::queen,  900 } };

    double total_material_count = 0;

    // count the pieces
    for (int i = 0; i < 64; i++)
    {
        if ((board.Square[i] != PieceType::none) &&
            ((board.Square[i] & PieceType::piece_mask) != PieceType::king))
        {
            material_map[board.Square[i]]++;
        }
        // count for bishop pair
        if ((board.Square[i] & PieceType::piece_mask) == PieceType::bishop)
        {
            switch (i % 2)
            {
            case 0:
                switch (board.Square[i] & PieceType::color_mask)
                {
                case PieceType::white:
                    white_ls++;
                    break;
                case PieceType::black:
                    black_ls++;
                    break;
                }
                break;
            case 1:
                switch (board.Square[i] & PieceType::color_mask)
                {
                case PieceType::white:
                    white_ds++;
                    break;
                case PieceType::black:
                    black_ds++;
                    break;
                }
                break;
            }
        }
    }

    // bishop pair calculation
    // bishop pair is worth 50 points (half a pawn)
    if ((white_ds + white_ls) >= 2)
        total_material_count += 50;
    if ((black_ds + black_ls) >= 2)
        total_material_count -= 50;


    // add up all the pieces to detamain the material
    // subtract black material
    for (int i = 10; i <= 14; i++)
        total_material_count -= (material_map[i] * piece_value[i & PieceType::piece_mask]);
    // add white material
    for (int i = 18; i <= 22; i++)
        total_material_count += (material_map[i] * piece_value[i & PieceType::piece_mask]);

    return total_material_count;
}

int Evaluate::evaluate(Board board, game_outcomes outcome, int depth)
{
    positions_evaluated++;
    // std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b" << positions_evaluated;
    int multiplier = (board.rights & 1) == 0 ? -1 : 1;
    if (outcome == game_outcomes::WHITE)
        return (multiplier * (INFINITY) + depth);
    else if (outcome == game_outcomes::BLACK)
        return (multiplier * (-INFINITY) + depth);
    else if (outcome == game_outcomes::DRAW)
        return 0;
    else 
        return (multiplier * (evaluateMaterial(board) + evaluateOptimalSquares(board) + evaluateSpaceAdvantage(board)));
}

int Evaluate::evaluateSpaceAdvantage(Board &board)
{
    int space_white = 0;
    int space_black = 0;

    behavior::KingBehavior behavior;
    auto white_attacked_sq = behavior.calculateAttackedSquares(board, PieceType::white);
    auto black_attacked_sq = behavior.calculateAttackedSquares(board, PieceType::black);

    for (int i = 0; i < 64; i++)
    {
        space_white += white_attacked_sq[i];
        space_black += black_attacked_sq[i];
    }

    return 8 * (space_white - space_black);
}

int Evaluate::evaluateOptimalSquares(Board &board)
{
    int white_square_score = 0;
    int black_square_score = 0;

    for (int i = 0; i < 64; i++)
    {
        if ((board.Square[i] & PieceType::color_mask) == PieceType::white)
        {
            white_square_score += ratePieceSquare(board, loc::Square64(i));
        }
        else if ((board.Square[i] & PieceType::color_mask) == PieceType::black)
        {
            black_square_score += ratePieceSquare(board, loc::Square64(i));
        }
    }

    return (white_square_score - black_square_score);
}

int Evaluate::ratePieceSquare(Board &board, loc::Square64 square)
{
    using namespace PieceSquareTable;
    int original_piece = board.Square[square] & PieceType::piece_mask;
    // if the piece is white than you need to flip it
    if ((board.Square[square] & PieceType::color_mask) == PieceType::white)
        square = loc::Square64(flip[square]);

    switch (original_piece)
    {
        case PieceType::king:
            if (isEndgame(board))
                return KingSquareTableEndgame[square];
            else 
                return KingSquareTable[square];
        case PieceType::pawn:
            return PawnSquareTable[square];
        case PieceType::knight:
            return KnightSquareTable[square];
        case PieceType::bishop:
            return BishopSquareTable[square];
        case PieceType::rook:
            return RookSquareTable[square];
        case PieceType::queen:
            return QueenSquareTable[square];
        default:
            return 0;
    }
}

bool Evaluate::isEndgame(Board &board)
{
    int total_piece_count = 0;
    for (int i = 0; i < 64; i++)
    {
        if (board.Square[i] != PieceType::none)
            total_piece_count++;
    }
    return ((total_piece_count <= 15) ? true : false);
}

// -------------------------------AI------------------------------- //

int ArtInt::negaMax(Board &board, int depth, int alpha, int beta)
{
    int color = ((board.rights & 1) + 1) * 8;

    game_outcomes outcome = _ERROR;
    Evaluate e;
    if (board.isGameOver(outcome) || depth <= 1)
        return e.evaluate(board, outcome, depth);

    std::list<move::MoveCell> moves_list;

    for (int i = 0; i < 64; i++)
    {
        if ((board.Square[i] & PieceType::color_mask) == color)
        {
            auto curr_list = board.calculateMoveList(loc::Square64(i));
            moves_list.splice(moves_list.end(), *curr_list);
        }
    }
    moves_list.sort(cmp); // O(Nlogn)

    std::list<move::MoveCell> temp_list;

    position_stack.push(board);

    auto it = moves_list.begin();
    while (it != moves_list.end())
    {
        if (it->type == move::promote)
        {
            // iterating from queen to knight
            for (int i = PieceType::queen; i >= PieceType::knight; i--)
            {
                board.movePiece(it->move, &temp_list, it->type, i);
                alpha = std::max(alpha, -negaMax(board, depth - 1, -beta, -alpha));
                board = position_stack.top();

                if (beta <= alpha)
                {
                    if(!position_stack.empty())
                        position_stack.pop();
                    return beta;
                }
            }
        }
        else
        {
            board.movePiece(it->move, &temp_list, it->type);
            alpha = std::max(alpha, -negaMax(board, depth - 1, -beta, -alpha));
            board = position_stack.top();
        }

        // board.printBoard();

        if (beta <= alpha)
        {
            position_stack.pop();
            return beta;
        }

        it++;
    }
    position_stack.pop();
    return alpha;
}

move::MoveCell ArtInt::generateComputerMove(Board board, int &promote_to, int depth)
{
    move::MoveCell best_move;
    int best_rating;

    int color = ((board.rights & 1) + 1) * 8;

    int alpha = -INFINITY;
    int beta = INFINITY;

    std::list<move::MoveCell> moves_list;

    for (int i = 0; i < 64; i++)
    {
        if ((board.Square[i] & PieceType::color_mask) == color)
        {
            auto curr_list = board.calculateMoveList(loc::Square64(i));
            moves_list.splice(moves_list.end(), *curr_list);
        }
    }
    moves_list.sort(cmp); // O(Nlogn)

    std::list<move::MoveCell> temp_list;

    // push the current board state into the stack
    position_stack.push(board);

    int eval;
    auto it = moves_list.begin();
    while (it != moves_list.end())
    {
        if (it->type == move::promote)
        {

            // iterating from queen to knight
            for (int i = PieceType::queen; i >= PieceType::knight; i--)
            {
                board.movePiece(it->move, &temp_list, it->type, i);
                temp_list.clear();
                eval = std::max(alpha, -negaMax(board, depth, -beta, -alpha));
                board = position_stack.top();

                if (eval > alpha)
                {
                    alpha = eval;
                    best_move = *it;
                    promote_to = i;
                    best_rating = eval;
                }
            }
        }
        else
        {
            board.movePiece(it->move, &temp_list, it->type);
            temp_list.clear();
            eval = std::max(alpha, -negaMax(board, depth, -beta, -alpha));
            board = position_stack.top();
        }

        if (eval > alpha)
        {
            alpha = eval;
            best_move = *it;
            best_rating = eval;
        }

        it++;
    }
    position_stack.pop();
    std::cout << "eval: " << float((color == PieceType::white ? 1 : -1) * best_rating) / 100 << std::endl;
    return best_move;
}
