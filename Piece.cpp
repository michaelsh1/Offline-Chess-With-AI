#include "Piece.hpp"

// will return true if the piece in the location square is pinned. \
   it will fill moves_list list with the available locations to to if pinned
bool behavior::PieceBehavior::isPinned(Board board, loc::Square64 square, std::list<move::MoveCell> *moves_list)
{
    moves_list->empty();

    // find king
    int number_of_attackers;
    int king_color = (board.Square[square] & PieceType::color_mask);
    board.Square[square] = PieceType::none;

    if (isKingInCheck(board, king_color, moves_list, number_of_attackers))
    {
        auto it = moves_list->begin();
        while (it != moves_list->end())
        {
            auto move = move::MoveCell::returnMoveVec(it->move);
            if (move.sq2 == square)
                return true;
            it++;
        }
        moves_list->empty();
        return false;
    }
    return false;
}

// this function will return true or false if the king of the selected color is in check. \
   it will also fill the moves_list list to contain the available moves to take or block the check.
bool behavior::PieceBehavior::isKingInCheck(Board &board, int king_color, std::list<move::MoveCell> *moves_list, int &number_of_attackers)
{
    // initialize
    moves_list->empty();
    number_of_attackers = 0;

    int king = PieceType::king | king_color;
    int attacking_color = (king_color == PieceType::white) ? PieceType::black : PieceType::white;
    // locate the king square
    loc::Square64 king_square;
    for (int i = 0; i < 64; i++)
    {
        if (board.Square[i] == king)
        {
            king_square = loc::Square64(i);
            break;
        }
    }

    move::MoveCell cell;
    const char(*attack_line)[5] = nullptr;

    if (isSquareAttacked(board, king_square, attacking_color))
    {
        const char(*normal_moves_arr)[5];
        const char(*slider_moves_arr)[8][5];

        for (int i = 0; i < 64; i++)
        {
            if ((board.Square[i] & PieceType::color_mask) == attacking_color)
            {
                switch (board.Square[i] & PieceType::piece_mask)
                {
                    // -----------------king----------------- //
                    case PieceType::king:
                        break;
                    // -----------------knight----------------- //
                    case PieceType::knight:
                    {
                        normal_moves_arr = knightPrecompiledMoves[i];
                        for (int k = 0; normal_moves_arr[k][0] != '\0'; k++)
                        {
                            auto move = move::MoveCell::returnMoveVec(normal_moves_arr[k]);
                            if (board.Square[move.sq2] == king)
                            {
                                if (++number_of_attackers >= 2)
                                {
                                    moves_list->empty();
                                    return true;
                                }
                                move::MoveCell::fillMoveStr({ move.sq2, move.sq1 }, cell.move);
                                cell.type = move::take;
                                moves_list->push_back(cell);
                                break;
                            }
                        }
                    }
                        break;
                    // -----------------pawn----------------- //
                    case PieceType::pawn:
                    {
                        normal_moves_arr = (attacking_color == PieceType::white) ? whitePawnPrecompiledMoves[i] : blackPawnPrecompiledMoves[i];
                        auto move = move::MoveCell::returnMoveVec(normal_moves_arr[0]);
                        if (((move.sq2 + 1) / 8) == (move.sq2 / 8))
                        {
                            if (board.Square[move.sq2 + 1] == king)
                            {
                                number_of_attackers++;
                                move::MoveCell::fillMoveStr({ move.sq2, move.sq1 }, cell.move);
                                cell.type = move::take;
                                moves_list->push_back(cell);
                                break;
                            }
                        }
                        if (((move.sq2 - 1) / 8) == (move.sq2 / 8))
                        {
                            if (board.Square[move.sq2 - 1] == king)
                            {
                                number_of_attackers++;
                                move::MoveCell::fillMoveStr({move.sq2, move.sq1}, cell.move);
                                cell.type = move::take;
                                moves_list->push_back(cell);
                                return true;
                            }
                        }
                    }
                        break;
                    // -----------------queen----------------- //
                    case PieceType::queen:
                    // -----------------rook----------------- //
                    case PieceType::rook:
                    {
                        slider_moves_arr = rookPrecompiledMoves[i];

                        for (int j = 0; j < 4; j++)
                        {
                            auto move_line = slider_moves_arr[j];
                            for (int k = 0; move_line[k][0] != '\0'; k++)
                            {
                                auto move = move::MoveCell::returnMoveVec(move_line[k]);
                                
                                if (board.Square[move.sq2] == king)
                                {
                                    if (++number_of_attackers >= 2)
                                    {
                                        moves_list->empty();
                                        return true;
                                    }
                                    else
                                    {
                                        attack_line = move_line;
                                        break;
                                    }
                                }

                                if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask) ||
                                    (((board.Square[move.sq2] & PieceType::color_mask) != (board.Square[move.sq1] & PieceType::color_mask)) &&
                                    (board.Square[move.sq2] != PieceType::none)))
                                    break;
                            }
                        }
                    }
                        if ((board.Square[i] & PieceType::piece_mask) != PieceType::queen)
                            break;
                    // -----------------bishop----------------- //
                    case PieceType::bishop:
                    {
                        slider_moves_arr = bishopPrecompiledMoves[i];

                        for (int j = 0; j < 4; j++)
                        {
                            auto move_line = slider_moves_arr[j];
                            for (int k = 0; move_line[k][0] != '\0'; k++)
                            {
                                auto move = move::MoveCell::returnMoveVec(move_line[k]);

                                if (board.Square[move.sq2] == king)
                                {
                                    if (++number_of_attackers >= 2)
                                    {
                                        moves_list->empty();
                                        return true;
                                    }
                                    else
                                    {
                                        attack_line = move_line;
                                        break;
                                    }
                                }

                                if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask) ||
                                    (((board.Square[move.sq2] & PieceType::color_mask) != (board.Square[move.sq1] & PieceType::color_mask)) &&
                                     (board.Square[move.sq2] != PieceType::none)))
                                    break;
                            }
                        }
                    }
                        break;
                }
            }
        }


        
        if (number_of_attackers < 2)
        {
            if (attack_line != nullptr)
            {
                for (int i = 0; attack_line[i][0] != '\0'; i++)
                {
                    auto move = move::MoveCell::returnMoveVec(attack_line[i]);

                    if (board.Square[move.sq2] == king)
                    {
                        move::MoveCell::fillMoveStr({ move.sq2, move.sq1 }, cell.move);
                        cell.type = move::take;
                        moves_list->push_back(cell);
                        break;
                    }

                    move::copyMove(attack_line[i], cell.move);
                    cell.type = move::normal;
                    moves_list->push_back(cell);
                }
                moves_list->sort(cmp);
            }
            return true;
        }
    }
    else
    {
        moves_list->empty();
        return true;
    }
}

// will return a bool array where the 1s represent attacked squares by attacking_color \
   and 0s will represent non attacked squares
bool *behavior::PieceBehavior::calculateAttackedSquares(Board board, int attacking_color)
{
    bool *attacked_squares = new bool[64]{};

    // find king
    int king_color = (attacking_color == PieceType::white) ? PieceType::black : PieceType::white;
    int king = king_color | PieceType::king;
    // remove king
    for (int i = 0; i < 64; i++)
    {
        if (board.Square[i] == king)
        {
            board.Square[i] = PieceType::none;
            break;
        }
    }

    const char(*normal_moves_arr)[5];
    const char(*slider_moves_arr)[8][5];

    for (int i = 0; i < 64; i++)
    {
        if ((board.Square[i] & PieceType::color_mask) == attacking_color)
        {
            switch (board.Square[i] & PieceType::piece_mask)
            {
                // -----------------king----------------- //
                case PieceType::king:
                {
                    normal_moves_arr = kingPrecompiledMoves[i];
                    for (int k = 0; normal_moves_arr[k][0] != '\0'; k++)
                    {
                        auto move = move::MoveCell::returnMoveVec(normal_moves_arr[k]);
                        attacked_squares[move.sq2] = true;
                    }
                }
                    break;
                // -----------------knight----------------- //
                case PieceType::knight:
                {
                    normal_moves_arr = knightPrecompiledMoves[i];
                    for (int k = 0; normal_moves_arr[k][0] != '\0'; k++)
                    {
                        auto move = move::MoveCell::returnMoveVec(normal_moves_arr[k]);
                        attacked_squares[move.sq2] = true;
                    }
                }
                    break;
                // -----------------pawn----------------- //
                case PieceType::pawn:
                {
                    normal_moves_arr = (attacking_color == PieceType::white) ? whitePawnPrecompiledMoves[i] : blackPawnPrecompiledMoves[i];
                    auto move = move::MoveCell::returnMoveVec(normal_moves_arr[0]);
                    if (((move.sq2 + 1) / 8) == (move.sq2 / 8))
                        attacked_squares[move.sq2 + 1] = true;
                    if (((move.sq2 - 1) / 8) == (move.sq2 / 8))
                        attacked_squares[move.sq2 - 1] = true;
                }
                    break;
                // -----------------queen----------------- //
                case PieceType::queen:
                // -----------------rook----------------- //
                case PieceType::rook:
                {
                    slider_moves_arr = rookPrecompiledMoves[i];

                    for (int j = 0; j < 4; j++)
                    {
                        auto move_line = slider_moves_arr[j];
                        for (int k = 0; move_line[k][0] != '\0'; k++)
                        {
                            auto move = move::MoveCell::returnMoveVec(move_line[k]);

                            if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask) ||
                                (((board.Square[move.sq2] & PieceType::color_mask) != (board.Square[move.sq1] & PieceType::color_mask)) &&
                                (board.Square[move.sq2] != PieceType::none)))
                            {
                                attacked_squares[move.sq2] = true;
                                break;
                            }
                            else
                                attacked_squares[move.sq2] = true;
                        }
                    }
                }
                    if ((board.Square[i] & PieceType::piece_mask) != PieceType::queen)
                        break;
                // -----------------bishop----------------- //
                case PieceType::bishop:
                {
                    slider_moves_arr = bishopPrecompiledMoves[i];

                    for (int j = 0; j < 4; j++)
                    {
                        auto move_line = slider_moves_arr[j];
                        for (int k = 0; move_line[k][0] != '\0'; k++)
                        {
                            auto move = move::MoveCell::returnMoveVec(move_line[k]);

                            if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask) ||
                                (((board.Square[move.sq2] & PieceType::color_mask) != (board.Square[move.sq1] & PieceType::color_mask)) &&
                                 (board.Square[move.sq2] != PieceType::none)))
                            {
                                attacked_squares[move.sq2] = true;
                                break;
                            }
                            else
                                attacked_squares[move.sq2] = true;
                        }
                    }
                }
                    break;
            }
        }
    }

    return attacked_squares;
}

// returns true if the square is attacked, false otherwise
bool behavior::PieceBehavior::isSquareAttacked(Board &board, loc::Square64 square, int attacking_color)
{
    return (calculateAttackedSquares(board, attacking_color))[square];
}

// king \
   returns the linked list of available squares for the king
void behavior::KingBehavior::behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list)
{
    moves_list->empty();
    if ((board.Square[square] & PieceType::piece_mask) != PieceType::king)
    { std::cout << "not a king" << std::endl; return; }

    auto attacked_squares = calculateAttackedSquares(board, (board.Square[square] & PieceType::color_mask) == PieceType::white ? 
                                                            PieceType::black : 
                                                            PieceType::white);

    move::MoveCell cell;

    auto moves_arr = kingPrecompiledMoves[square];
    for (int i = 0; moves_arr[i][0] != '\0'; i++)
    {
        auto move = move::MoveCell::returnMoveVec(moves_arr[i]);

        if (attacked_squares[move.sq2] == true)
            continue;
        else
        {
            if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask))
                continue;
            else
            {
                move::copyMove(moves_arr[i], cell.move);
                if (board.Square[move.sq2] == PieceType::none)
                    cell.type = move::normal;
                else
                    cell.type = move::take;

                moves_list->push_back(cell);
            }
        }
    }

    if (!attacked_squares[square]) // if the king is in check you are not allowed to castle
    {
        switch ((board.Square[square] & PieceType::color_mask))
        {
            case PieceType::white:
                if ((board.rights & Rights::king_side_white) == Rights::king_side_white)
                {
                    if (!attacked_squares[square + 1] && !attacked_squares[square + 2] &&
                        board.Square[square + 1] == PieceType::none && board.Square[square + 2] == PieceType::none)
                    {
                        move::MoveCell::fillMoveStr({ square, loc::Square64(square + 2) }, cell.move);
                        cell.type = move::normal;
                        moves_list->push_back(cell);
                    }
                }
                if ((board.rights & Rights::queen_side_white) == Rights::queen_side_white)
                {
                    if (!attacked_squares[square - 1] && !attacked_squares[square - 2] &&
                        board.Square[square - 1] == PieceType::none && board.Square[square - 2] == PieceType::none &&
                        board.Square[square - 3] == PieceType::none)
                    {
                        move::MoveCell::fillMoveStr({ square, loc::Square64(square - 2) }, cell.move);
                        cell.type = move::normal;
                        moves_list->push_back(cell);
                    }
                }
                break;
            case PieceType::black:
                if ((board.rights & Rights::king_side_black) == Rights::king_side_black)
                {
                    if (!attacked_squares[square + 1] && !attacked_squares[square + 2] &&
                        board.Square[square + 1] == PieceType::none && board.Square[square + 2] == PieceType::none)
                    {
                        move::MoveCell::fillMoveStr({ square, loc::Square64(square + 2) }, cell.move);
                        cell.type = move::normal;
                        moves_list->push_back(cell);
                    }
                }
                if ((board.rights & Rights::queen_side_black) == Rights::queen_side_black)
                {
                    if (!attacked_squares[square - 1] && !attacked_squares[square - 2] &&
                        board.Square[square - 1] == PieceType::none && board.Square[square - 2] == PieceType::none &&
                        board.Square[square - 3] == PieceType::none)
                    {
                        move::MoveCell::fillMoveStr({ square, loc::Square64(square - 2) }, cell.move);
                        cell.type = move::normal;
                        moves_list->push_back(cell);
                    }
                }
                break;
        }
    }

    delete[] attacked_squares;

    moves_list->sort(cmp);
}

// bishop \
   returns the linked list of available squares for the bishop \
   takes care of checks
void behavior::BishopBehavior::behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list)
{
    moves_list->empty();
    if ((board.Square[square] & PieceType::piece_mask) != PieceType::bishop)
    {
        std::cout << "not a bishop" << std::endl;
        return;
    }

    auto moves_arr = bishopPrecompiledMoves[square];

    for (int i = 0; i < 4; i++)
    {
        auto move_line = moves_arr[i];
        for (int k = 0; move_line[k][0] != '\0'; k++)
        {
            auto move = move::MoveCell::returnMoveVec(move_line[k]);

            if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask))
                break;
            else
            {
                move::MoveCell cell;
                move::copyMove(move_line[k], cell.move);
                if (board.Square[move.sq2] == PieceType::none)
                    cell.type = move::normal;
                else
                {
                    cell.type = move::take;
                    moves_list->push_back(cell);
                    break;
                }

                moves_list->push_back(cell);
            }
        }
    }

    std::list<move::MoveCell> available_list;
    int number_of_attackers;
    bool flag;
    if (isPinned(board, square, &available_list))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }
    else if (isKingInCheck(board, (board.Square[square] & PieceType::color_mask), &available_list, number_of_attackers))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }

    moves_list->sort(cmp);
}

// knight \
   returns the linked list of available squares for the knight
void behavior::KnightBehavior::behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list)
{
    moves_list->empty();
    if ((board.Square[square] & PieceType::piece_mask) != PieceType::knight)
        { std::cout << "not a knight" << std::endl; return; }

    std::list<move::MoveCell> available_list;
    if (isPinned(board, square, &available_list))
        return;

    auto moves_arr = knightPrecompiledMoves[square];

    for (int i = 0; moves_arr[i][0] != '\0'; i++)
    {
        auto move = move::MoveCell::returnMoveVec(moves_arr[i]);
        // if the target square piece is the same color as the knight
        if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask))
            continue;
        else
        {
            move::MoveCell cell;
            move::copyMove(moves_arr[i], cell.move);
            if (board.Square[move.sq2] == PieceType::none)
                cell.type = move::normal;
            else
                cell.type = move::take;

            moves_list->push_back(cell);
        }
    }

    int number_of_attackers;
    bool flag;

    if (isKingInCheck(board, (board.Square[square] & PieceType::color_mask), &available_list, number_of_attackers))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }

    moves_list->sort(cmp);
}

//  pawn \
   returns the linked list of available squares for the pawn
void behavior::PawnBehavior::behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list)
{
    moves_list->empty();
    if ((board.Square[square] & PieceType::piece_mask) != PieceType::pawn)
        {
            std::cout << "not a pawn. square: " << square << std::endl;
            return;
        }

    const char (*moves_arr)[5];
    int pawn_color;

    // calculate pawn type
    switch (board.Square[square] & PieceType::color_mask)
    {
        case PieceType::white:
            moves_arr = whitePawnPrecompiledMoves[square];
            pawn_color = PieceType::white;
            break;
        case PieceType::black:
            moves_arr = blackPawnPrecompiledMoves[square];
            pawn_color = PieceType::black;
            break;
    }

    move::MoveCell cell;
    // calculate forward move
    for (int i = 0; moves_arr[i][0] != '\0'; i++)
    {
        auto move = move::MoveCell::returnMoveVec(moves_arr[i]);

        if (board.Square[move.sq2] != PieceType::none)
            break;

        move::copyMove(moves_arr[i], cell.move);
        cell.type = move::normal;
        moves_list->push_back(cell);
    }

    // calculate captures
    auto move = move::MoveCell::returnMoveVec(moves_arr[0]);
    if (((move.sq2 + 1) / 8) == (move.sq2 / 8))
    {
        if (((board.Square[move.sq2 + 1] & PieceType::color_mask) != pawn_color) &&
            ((board.Square[move.sq2 + 1] & PieceType::color_mask) != PieceType::none))
        {
            move::MoveCell::fillMoveStr({ square, loc::Square64(move.sq2 + 1) }, cell.move);
            cell.type = move::take;
            moves_list->push_back(cell);
        }
        // en passant capture
        else if (board.en_passant_square == (move.sq2 + 1))
        {
            auto temp = board.Square[square + 1];
            board.Square[square + 1] = PieceType::none;
            std::list<move::MoveCell> available_list;

            if (!isPinned(board, square, &available_list))
            {
                move::MoveCell::fillMoveStr({ square, loc::Square64(move.sq2 + 1) }, cell.move);
                cell.type = move::en_passant;
                moves_list->push_back(cell);
            }
            board.Square[square + 1] = temp;
        }
    }

    if (((move.sq2 - 1) / 8) == (move.sq2 / 8))
    {
        if (((board.Square[move.sq2 - 1] & PieceType::color_mask) != pawn_color) &&
            ((board.Square[move.sq2 - 1] & PieceType::color_mask) != PieceType::none))
        {
            move::MoveCell::fillMoveStr({ square, loc::Square64(move.sq2 - 1) }, cell.move);
            cell.type = move::take;
            moves_list->push_back(cell);
        }
        // en passant capture
        else if (board.en_passant_square == (move.sq2 - 1))
        {
            auto temp = board.Square[square - 1];
            board.Square[square - 1] = PieceType::none;
            std::list<move::MoveCell> available_list;

            if (!isPinned(board, square, &available_list))
            {
                move::MoveCell::fillMoveStr({ square, loc::Square64(move.sq2 - 1) }, cell.move);
                cell.type = move::en_passant;
                moves_list->push_back(cell);
            }
            board.Square[square - 1] = temp;
        }
    }

    // calculate pinned moves
    std::list<move::MoveCell> available_list;
    int number_of_attackers;
    bool flag;

    if (isPinned(board, square, &available_list))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }
    else if (isKingInCheck(board, (board.Square[square] & PieceType::color_mask), &available_list, number_of_attackers))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }

    moves_list->sort(cmp);
}

// rook \
   returns the linked list of available squares for the rook
void behavior::RookBehavior::behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list)
{
    moves_list->empty();
    if ((board.Square[square] & PieceType::piece_mask) != PieceType::rook)
        { std::cout << "not a rook" << std::endl; return; }

    auto moves_arr = rookPrecompiledMoves[square];

    for (int i = 0; i < 4; i++)
    {
        auto move_line = moves_arr[i];
        for (int k = 0; move_line[k][0] != '\0'; k++)
        {
            auto move = move::MoveCell::returnMoveVec(move_line[k]);

            if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask))
                break;
            else
            {
                move::MoveCell cell;
                move::copyMove(move_line[k], cell.move);
                if (board.Square[move.sq2] == PieceType::none)
                    cell.type = move::normal;
                else
                {
                    cell.type = move::take;
                    moves_list->push_back(cell);
                    break;
                }

                moves_list->push_back(cell);
            }
        }
    }

    std::list<move::MoveCell> available_list;
    int number_of_attackers;
    bool flag;
    if (isPinned(board, square, &available_list))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }
    else if (isKingInCheck(board, (board.Square[square] & PieceType::color_mask), &available_list, number_of_attackers))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }

    moves_list->sort(cmp);
}

// queen \
   returns the linked list of available squares for the queen
void behavior::QueenBehavior::behavior(Board &board, loc::Square64 square, std::list<move::MoveCell> *moves_list)
{
    moves_list->empty();
    if ((board.Square[square] & PieceType::piece_mask) != PieceType::queen)
    { std::cout << "not a queen" << std::endl; return; }

    // bishop moves
    auto moves_arr = bishopPrecompiledMoves[square];

    for (int i = 0; i < 4; i++)
    {
        auto move_line = moves_arr[i];
        for (int k = 0; move_line[k][0] != '\0'; k++)
        {
            auto move = move::MoveCell::returnMoveVec(move_line[k]);

            if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask))
                break;
            else
            {
                move::MoveCell cell;
                move::copyMove(move_line[k], cell.move);
                if (board.Square[move.sq2] == PieceType::none)
                    cell.type = move::normal;
                else
                {
                    cell.type = move::take;
                    moves_list->push_back(cell);
                    break;
                }

                moves_list->push_back(cell);
            }
        }
    }

    // rook moves
    moves_arr = rookPrecompiledMoves[square];

    for (int i = 0; i < 4; i++)
    {
        auto move_line = moves_arr[i];
        for (int k = 0; move_line[k][0] != '\0'; k++)
        {
            auto move = move::MoveCell::returnMoveVec(move_line[k]);

            if ((board.Square[move.sq2] & PieceType::color_mask) == (board.Square[move.sq1] & PieceType::color_mask))
                break;
            else
            {
                move::MoveCell cell;
                move::copyMove(move_line[k], cell.move);
                if (board.Square[move.sq2] == PieceType::none)
                    cell.type = move::normal;
                else
                {
                    cell.type = move::take;
                    moves_list->push_back(cell);
                    break;
                }

                moves_list->push_back(cell);
            }
        }
    }

    std::list<move::MoveCell> available_list;
    int number_of_attackers;
    bool flag;

    if (isPinned(board, square, &available_list))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }
    else if (isKingInCheck(board, (board.Square[square] & PieceType::color_mask), &available_list, number_of_attackers))
    {
        auto it1 = moves_list->begin();
        while (it1 != moves_list->end())
        {
            flag = false;
            auto move1 = move::MoveCell::returnMoveVec(it1->move);
            auto it2 = available_list.begin();
            while (it2 != available_list.end())
            {
                auto move2 = move::MoveCell::returnMoveVec(it2->move);
                if (move1.sq2 == move2.sq2)
                {
                    flag = true;
                    break;
                }
                it2++;
            }
            if (flag == false)
                moves_list->erase(it1);
            it1++;
        }
    }

    moves_list->sort(cmp);
}
