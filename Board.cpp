#include "Board.hpp"

// --------------------------------------board-------------------------------------- //

Board::Board(const char *FEN)
{
    setFEN(FEN);
}

void Board::clearBoard()
{
    for (int i = 0; i < 64; i++)
        Square[i] = PieceType::none;

    rights = 0;
    fifty_move_rule = 0;
}

void Board::setFEN(const char *FEN)
{
    clearBoard();

    std::map<char, int> FENdict = {
        {'k', PieceType::king},
        {'p', PieceType::pawn},
        {'n', PieceType::knight},
        {'b', PieceType::bishop},
        {'r', PieceType::rook},
        {'q', PieceType::queen}};

    int file = loc::FILE_A, rank = loc::RANK_8;
    while (*FEN != ' ')
    {
        if (*FEN == '/')
        {
            rank--;
            file = 0;
        }
        else
        {
            if ('0' <= *FEN && *FEN <= '9')
                file += *FEN - '0' - 1;
            else
            {
                int pieceColor = ('A' <= *FEN && *FEN <= 'Z') ? PieceType::white : PieceType::black;
                int pieceType;
                if (pieceColor == PieceType::black)
                    pieceType = FENdict[*FEN];
                else
                    pieceType = FENdict[TO_LOWER(*FEN)];
                Square[rank * 8 + file] = pieceColor | pieceType;
            }
            file++;
        }
        FEN++;
    }
    while (*FEN)
    {
        switch (*FEN)
        {
        case 'w':
            rights |= 1;
            break;
        case 'b':
            rights &= ~1;
            break;
        case 'K':
            rights |= Rights::king_side_white;
            break;
        case 'Q':
            rights |= Rights::queen_side_white;
            break;
        case 'k':
            rights |= Rights::king_side_black;
            break;
        case 'q':
            rights |= Rights::queen_side_black;
            break;
        }
        if (*FEN >= 'a' && *FEN <= 'h')
            en_passant_square = move::returnSingleMove(FEN);
        
        FEN++;
    }
}

// true if successful false if not
bool Board::movePiece(char move[5], std::list<move::MoveCell> *move_list, move::MoveType &move_type, int promote_to)
{
auto mve = move::MoveCell::returnMoveVec(move);

auto moves_list = calculateMoveList(mve.sq1);

int piece = Square[mve.sq1];

auto it = moves_list->begin();
while (it != moves_list->end())
{
    if (move::cmpMove(move, it->move) == true)
    {
        // increase fifty move rule
        fifty_move_rule++;
        if (it->type == move::take)
            fifty_move_rule = 0;
        // set move type (changeable)
        move_type = it->type;

        Square[mve.sq1] = PieceType::none;
        Square[mve.sq2] = piece;

        // change en passant square to nothing
        en_passant_square = loc::ER;

        // pawn stuff
        // set correct en passant square
        if ((piece & PieceType::piece_mask) == PieceType::pawn)
        {
            // if pawn moved than fifty move rule resets
            fifty_move_rule = 0;

            switch (piece & PieceType::color_mask)
            {
                case PieceType::black:
                    if ((mve.sq1 - 16) == mve.sq2)
                        en_passant_square = loc::Square64(mve.sq1 - 8);
                    break;
                case PieceType::white:
                    if ((mve.sq1 + 16) == mve.sq2)
                        en_passant_square = loc::Square64(mve.sq1 + 8);
                    break;
            }

            // remove pawn taken by en passant
            if (it->type == move::en_passant)
            {
                switch (piece & PieceType::color_mask)
                {
                    case PieceType::black:
                        Square[mve.sq2 + 8] = PieceType::none;
                        break;
                    case PieceType::white:
                        Square[mve.sq2 - 8] = PieceType::none;
                        break;
                }
            }

            // promotion
            // black pawn promotion
            if ((mve.sq2 / 8) == loc::RANK_1)
            {
                Square[mve.sq2] = (PieceType::black | promote_to);
                move_type = move::promote;
            }
            // white pawn promotion
            else if ((mve.sq2 / 8) == loc::RANK_8)
            {
                Square[mve.sq2] = (PieceType::white | promote_to);
                move_type = move::promote;
            }
        }
        // no reason to check for castlig related move if not king or rook
        else if ((piece & PieceType::piece_mask) == PieceType::king ||
                    (piece & PieceType::piece_mask) == PieceType::rook)
        {
            // castling stuff
            // deal with castling rights
            if ((rights & ~1) != 0) // if any castling rights left
            {
                if ((piece & PieceType::piece_mask) == PieceType::king)
                {
                    if ((piece & PieceType::color_mask) == PieceType::white)
                    {
                        rights &= ~Rights::king_side_white;
                        rights &= ~Rights::queen_side_white;
                    }
                    else
                    {
                        rights &= ~Rights::king_side_black;
                        rights &= ~Rights::queen_side_black;
                    }
                }
                else if ((piece & PieceType::piece_mask) == PieceType::rook)
                {
                    if ((piece & PieceType::color_mask) == PieceType::white)
                    {
                        if (mve.sq1 == loc::h1)
                            rights &= ~Rights::king_side_white;
                        else if (mve.sq1 == loc::a1)
                            rights &= ~Rights::queen_side_white;
                    }
                    else
                    {
                        if (mve.sq1 == loc::h8)
                            rights &= ~Rights::king_side_black;
                        else if (mve.sq1 == loc::a8)
                            rights &= ~Rights::queen_side_black;
                    }
                }
                if ((Square[mve.sq2] & PieceType::piece_mask) == PieceType::rook)
                {
                    if ((Square[mve.sq2] & PieceType::color_mask) == PieceType::white)
                    {
                        if (mve.sq1 == loc::h1)
                            rights &= ~Rights::king_side_white;
                        else if (mve.sq2 == loc::a1)
                            rights &= ~Rights::queen_side_white;
                    }
                    else
                    {
                        if (mve.sq2 == loc::h8)
                            rights &= ~Rights::king_side_black;
                        else if (mve.sq2 == loc::a8)
                            rights &= ~Rights::queen_side_black;
                    }
                }
            }
            
            // deal with a castles move
            // gotta make sure we are dealing with a king, not a rook
            if ((piece & PieceType::piece_mask) == PieceType::king)
            {
                // king side castles
                if (mve.sq1 + 2 == mve.sq2)
                {
                    Square[mve.sq1 + 1] = Square[mve.sq1 + 3];
                    Square[mve.sq1 + 3] = PieceType::none;
                }
                // queen side castles
                else if (mve.sq1 - 2 == mve.sq2)
                {
                    Square[mve.sq1 - 1] = Square[mve.sq1 - 4];
                    Square[mve.sq1 - 4] = PieceType::none;
                }
            }
        }
        
        // change turn
        rights = ((rights & 1) == 1) ? (rights - 1) : (rights + 1);
        move::MoveCell::fillMoveStr(move::MoveCell::returnMoveVec(move), last_move);
        return true;
    }
    it++;
}
return false;
}

std::list<move::MoveCell> *Board::calculateMoveList(loc::Square64 square)
{
    auto moves_list = new std::list<move::MoveCell>;

    behavior::PieceBehavior *behavior = new behavior::PawnBehavior;

    switch (Square[square] & PieceType::piece_mask)
    {
        case PieceType::king:
            behavior = new behavior::KingBehavior;
            break;
        case PieceType::pawn:
            behavior = new behavior::PawnBehavior;
            break;
        case PieceType::knight:
            behavior = new behavior::KnightBehavior;
            break;
        case PieceType::bishop:
            behavior = new behavior::BishopBehavior;
            break;
        case PieceType::rook:
            behavior = new behavior::RookBehavior;
            break;
        case PieceType::queen:
            behavior = new behavior::QueenBehavior;
            break;
    }

    // calculate the moves
    behavior->behavior(*this, square, moves_list);
    
    delete[] behavior;

    return moves_list;
}

void Board::printBoard()
{
    std::map<int, char> PieceDict = {
        {PieceType::white | PieceType::king, 'K'},
        {PieceType::white | PieceType::pawn, 'P'},
        {PieceType::white | PieceType::knight, 'N'},
        {PieceType::white | PieceType::bishop, 'B'},
        {PieceType::white | PieceType::rook, 'R'},
        {PieceType::white | PieceType::queen, 'Q'},

        {PieceType::black | PieceType::king, 'k'},
        {PieceType::black | PieceType::pawn, 'p'},
        {PieceType::black | PieceType::knight, 'n'},
        {PieceType::black | PieceType::bishop, 'b'},
        {PieceType::black | PieceType::rook, 'r'},
        {PieceType::black | PieceType::queen, 'q'},

        {PieceType::none, '-'}};

    for (int i = 0; i < 64; i++)
    {
        if (i % 8 == 0)
            std::cout << '\n';
        std::cout << PieceDict[Square[i]] << " ";
    }
    std::cout << std::endl;

    std::cout << std::bitset<8>(rights) << std::endl;
}

bool Board::isGameOver(game_outcomes &outcome)
{
    // the fifty move rule determines that if a capture or pawn move didnt occur in 50 or more moves the game is a draw
    if (fifty_move_rule >= 50)
    {
        outcome = game_outcomes::DRAW;
        return true;
    }

    int side_to_check = (((rights & 1) + 1) * 8);
    outcome = game_outcomes::_ERROR;
    std::list<move::MoveCell> block_list;
    behavior::KingBehavior behave;
    int number_of_attackers;


    // check for checkmate
    if (behave.isKingInCheck(*this, side_to_check, &block_list, number_of_attackers))
    {
        std::list<move::MoveCell> king_availabe_list;

        int i;
        // find king
        auto king = (PieceType::king | side_to_check);

        for (i = 0; i < 64; i++)
            if (Square[i] == king)
                break;

        behave.behavior(*this, loc::Square64(i), &king_availabe_list);


        // is no available squares for the king to escape to
        if (king_availabe_list.empty())
        {
            for (i = 0; i < 64; i++)
            {
                if ((Square[i] & PieceType::color_mask) == side_to_check && 
                    (Square[i] & PieceType::piece_mask) != PieceType::king)
                {
                    if (calculateMoveList(loc::Square64(i))->empty() != true)
                        return false;
                }
            }

            // if king in check AND king cant move AND attacker cant be taken or blocked, \
               than its a checkmate and the opposite color to the checked is the winner
            outcome = (side_to_check == PieceType::white) ? BLACK : WHITE;
            return true;
        }
        else
            return false;
    }
    // check for stalemate or insufficient material
    else
    {
        // check for stalemate
        int i;
        for (i = 0; i < 64; i++)
        {
            if ((Square[i] & PieceType::color_mask) == side_to_check)
            {
                if (calculateMoveList(loc::Square64(i))->empty() != true)
                    break;
            }
        }
        // if all the pieces cant make a legal move and also the king isnt in check, \
           it means its a draw by stalemate
        if (i == 64)
        {
            outcome = game_outcomes::DRAW;
            return true;
        }
        // if a piece can move maby there is insufficient material \
           which means not enough material to checkmate
        else
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

            int total_material_count = 0;

            // count the pieces
            for (int i = 0; i < 64; i++)
            {
                if ((Square[i] != PieceType::none) && 
                    ((Square[i] & PieceType::piece_mask) != PieceType::king))
                {
                    material_map[Square[i]]++;
                    total_material_count++;
                }
                if ((Square[i] & PieceType::piece_mask) == PieceType::bishop)
                {
                    switch (i % 2)
                    {
                        case 0:
                            switch (Square[i] & PieceType::color_mask)
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
                            switch (Square[i] & PieceType::color_mask)
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
            
            // if there are only kings on the board its insufficient material
            if (total_material_count == 0)
            {
                outcome = game_outcomes::DRAW;
                return true;
            }
            // if there are any pawns thats not insufficient
            else if (material_map[PieceType::white | PieceType::pawn] != 0 || 
                     material_map[PieceType::black | PieceType::pawn] != 0)
                return false;
            // if there are no pawns but there are heavy pieces (rooks or queens) its also sufficient
            else if (material_map[PieceType::black | PieceType::rook] != 0 ||
                     material_map[PieceType::black | PieceType::queen] != 0 ||
                     material_map[PieceType::white | PieceType::rook] != 0 ||
                     material_map[PieceType::white | PieceType::queen] != 0)
                return false;
            // if there are no heavy pieces and no pawns that means that only minor pieces are left
            else
            {
                // if white or black has the bishop pair than its sufficient material to checkmate
                if (((white_ls >= 1) && (white_ds >= 1)) || 
                    ((black_ls >= 1) && (black_ds >= 1)))
                    return false;
                // if any side has 3 or more knights its sufficient material to checkmate
                else if ((material_map[PieceType::black | PieceType::knight] >= 3) ||
                         (material_map[PieceType::white | PieceType::knight] >= 3))
                    return false;
                // a bishop and knight are also sufficient to checkmate
                else if (((material_map[PieceType::black | PieceType::knight] >= 1) &&
                          (material_map[PieceType::black | PieceType::bishop] >= 1)) ||
                         ((material_map[PieceType::white | PieceType::knight] >= 1) &&
                          (material_map[PieceType::white | PieceType::bishop] >= 1)))
                    return false;
                // if a lone knight or less or a lone bishop or less, than its not sufficient material to checkmate, \
                   therefore its a drawn position
                outcome = game_outcomes::DRAW;
                return true;
            }
        }
    }

    return false;
}

// --------------------------------------visual board-------------------------------------- //

std::map<int, sf::Texture *> VisualBoard::textures;

void VisualBoard::loadTextures()
{
    std::map<int, const char *> texture_strings = {
        { PieceType::black | PieceType::king,    "res\\black\\king.png"   },
        { PieceType::black | PieceType::pawn,    "res\\black\\pawn.png"   },
        { PieceType::black | PieceType::knight,  "res\\black\\knight.png" },
        { PieceType::black | PieceType::bishop,  "res\\black\\bishop.png" },
        { PieceType::black | PieceType::rook,    "res\\black\\rook.png"   },
        { PieceType::black | PieceType::queen,   "res\\black\\queen.png"  },
        { PieceType::white | PieceType::king,    "res\\white\\king.png"   },
        { PieceType::white | PieceType::pawn,    "res\\white\\pawn.png"   },
        { PieceType::white | PieceType::knight,  "res\\white\\knight.png" },
        { PieceType::white | PieceType::bishop,  "res\\white\\bishop.png" },
        { PieceType::white | PieceType::rook,    "res\\white\\rook.png"   },
        { PieceType::white | PieceType::queen,   "res\\white\\queen.png"  } };

    for (int i = 1; i <= 2; i++)
    {
        for (int j = 1; j <= 6; j++)
        {
            textures.insert(std::make_pair(((i * 8) | j), new sf::Texture));
            textures[((i * 8) | j)]->loadFromFile(texture_strings[((i * 8) | j)]);
            textures[((i * 8) | j)]->setSmooth(true);
        }
    }
}

void VisualBoard::loadSprites(int board[64])
{
    for (int i = 0; i < 64; i++)
    {
        if (board[i] != PieceType::none)
        {
            pieces[7 - (i % 8) + 8 * (i / 8)] = new sf::Sprite;
            pieces[7 - (i % 8) + 8 * (i / 8)]->setTexture(*textures[board[i]]);
            pieces[7 - (i % 8) + 8 * (i / 8)]->setOrigin({ 50, 50 });
            pieces[7 - (i % 8) + 8 * (i / 8)]->setPosition({ float(int((7 - (i % 8)) * 100) + 50), float(int(((i / 8)) * 100) + 50) });
        }
        else
            pieces[7 - (i % 8) + 8 * (i / 8)] = nullptr;
    }
}

void VisualBoard::drawEmptyBoard(sf::RenderWindow &window,
                                 sf::Color color_dark,
                                 sf::Color color_light)
{
    sf::Color color;
    sf::RectangleShape rect;
    rect = sf::RectangleShape(sf::Vector2f(window.getSize().x / 8, window.getSize().y / 8));
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i % 2 == 0)
            {
                if (j % 2 == 0)
                    color = color_light;
                else
                    color = color_dark;
            }
            else
            {
                if (j % 2 == 0)
                    color = color_dark;
                else
                    color = color_light;
            }
            rect.setFillColor(color);
            rect.setPosition(sf::Vector2f(i * window.getSize().x / 8, j * window.getSize().y / 8));
            window.draw(rect);
        }
    }
}

/*
    flips the visual board to be white up or down ( default up )
    in: white up or not (boolean)
    out: (nothing)
*/
void VisualBoard::flipBoard(bool _white_up)
{
    if (white_up == _white_up)
        return;
    else
    {
        white_up = _white_up;

        for (int i = 0; i < 64; i++)
        {
            if (pieces[i] == nullptr)
                continue;
            int x, y;
            x = pieces[i]->getPosition().x;
            y = pieces[i]->getPosition().y;
            pieces[i]->setPosition({ float(800 - x), float(800 - y) });
        }
    }
}

void markSquare(loc::Square64 flip_square, sf::Color color, sf::RectangleShape highlighter)
{

}

void VisualBoard::displayBoard(sf::RenderWindow &window, Board &board,
                               sf::Color color_dark,
                               sf::Color color_light,
                               std::list<move::MoveCell> *highlight_moves,
                               sf::Color highlight_color)
{
    auto markSquare = [this, &window](loc::Square64 flip_square, sf::Color color, sf::RectangleShape &highlighter)
    {
        highlighter.setFillColor(color);

        auto square = POS_XY(flip_square);
        square = {(float)((int)square.x * 100), (float)((int)square.y * 100)};

        if (!white_up)
            square = { 700 - square.x, 700 - square.y };

        highlighter.setPosition(square);
        window.draw(highlighter);
    };

    drawEmptyBoard(window, color_dark, color_light);
    sf::RectangleShape highlighter;
    highlighter.setSize({ 100, 100 });
    if (highlight_moves != nullptr)
    {
        auto it = highlight_moves->begin();
        while (it != highlight_moves->end())
        {
            markSquare(move::returnMoveVecflip(it->move).sq2, highlight_color, highlighter);
            it++;
        }
    }
    if (board.last_move != "\0")
        {
            markSquare(move::returnMoveVecflip(board.last_move).sq1, 
                       { 255 - highlight_color.r, 255 - highlight_color.g, 255 - highlight_color.b, highlight_color.a }, 
                       highlighter);
            markSquare(move::returnMoveVecflip(board.last_move).sq2, 
                       { 255 - highlight_color.r, 255 - highlight_color.g, 255 - highlight_color.b, highlight_color.a }, 
                       highlighter);
        }

    for (int i = 0; i < 64; i++)
    {
        if(pieces[i] == nullptr)
            continue;
        else
            window.draw(*pieces[i]);
    }
}

bool VisualBoard::convertMouseToMove(sf::Vector2i mouse_position_press,
                                      sf::Vector2i mouse_position_release,
                                      char move[5])
{
    mouse_position_press    = { mouse_position_press.x - (mouse_position_press.x % 100),
                                mouse_position_press.y - (mouse_position_press.y % 100) };
    mouse_position_release  = { mouse_position_release.x - (mouse_position_release.x % 100),
                                mouse_position_release.y - (mouse_position_release.y % 100) };

    move::TwoSqrVec<loc::Square64> move_vec;
    if (white_up)
    {
        move_vec = {loc::Square64(7 - (mouse_position_press.x / 100) + 8 * (mouse_position_press.y / 100)),
                    loc::Square64(7 - (mouse_position_release.x / 100) + 8 * (mouse_position_release.y / 100)) };
    }
    else
    {
        mouse_position_press   = { 700 - mouse_position_press.x,   700 - mouse_position_press.y };
        mouse_position_release = { 700 - mouse_position_release.x, 700 - mouse_position_release.y };


        move_vec = {loc::Square64((7 - (mouse_position_press.x / 100) + 8 * (mouse_position_press.y / 100))),
                    loc::Square64((7 - (mouse_position_release.x / 100) + 8 * (mouse_position_release.y / 100)))};
    }

    move::MoveCell::fillMoveStr(move_vec, move);
}

// consider that the board recieved is the board AFTER the move was done
void VisualBoard::movePiece(Board &board, char move[5], move::MoveType move_type, int promote_to)
{
    auto pos = move::returnMoveVecflip(move);

    if (!white_up)
        pieces[pos.sq1]->setPosition( { (float)(((7 - (pos.sq2 % 8)) * 100) + 50),
                                        (float)(((7 - (pos.sq2 / 8)) * 100) + 50) });
    else
        pieces[pos.sq1]->setPosition({ (float)(((pos.sq2 % 8) * 100) + 50),
                                       (float)(((pos.sq2 / 8) * 100) + 50) });
    // if target square is not empty
    if (pieces[pos.sq2] != nullptr)
        delete pieces[pos.sq2];

    pieces[pos.sq2] = pieces[pos.sq1];
    pieces[pos.sq1] = nullptr;

    // normal board move (unflipped)
    auto mve = move::MoveCell::returnMoveVec(move);

    // visual en passant move
    if (move_type == move::en_passant)
    {
        switch (board.Square[mve.sq2] & PieceType::color_mask)
        {
            case PieceType::black:
                delete pieces[move::convertSquareToFlip(loc::Square64(mve.sq2 + 8))];
                pieces[move::convertSquareToFlip(loc::Square64(mve.sq2 + 8))] = nullptr;
                break;
            case PieceType::white:
                delete pieces[move::convertSquareToFlip(loc::Square64(mve.sq2 - 8))];
                pieces[move::convertSquareToFlip(loc::Square64(mve.sq2 - 8))] = nullptr;
                break;
        }
    }

    if (move_type == move::promote)
    {
        pieces[(pos.sq2)]->setTexture(*textures[(board.Square[mve.sq2] & PieceType::color_mask) | promote_to]);
    }

    auto piece = board.Square[mve.sq2];
    // visual castles move
    if ((piece & PieceType::piece_mask) == PieceType::king)
    {
        // king side castles
        if (mve.sq1 + 2 == mve.sq2)
        {
            auto rook_loc = move::convertSquareToFlip(loc::Square64(mve.sq2 - 1));

            pieces[rook_loc] = pieces[move::convertSquareToFlip(loc::Square64(mve.sq1 + 3))];
            pieces[move::convertSquareToFlip(loc::Square64(mve.sq1 + 3))] = nullptr;

            if (white_up)
                pieces[rook_loc]->setPosition({ (float)((rook_loc % 8) * 100 + 50), (float)((rook_loc / 8) * 100 + 50) });
            else
                pieces[rook_loc]->setPosition({ (float)(700 - (rook_loc % 8) * 100 + 50), (float)(700 - (rook_loc / 8) * 100 + 50) });
        }
        // queen side castles
        else if (mve.sq1 - 2 == mve.sq2)
        {
            auto rook_loc = move::convertSquareToFlip(loc::Square64(mve.sq2 + 1));

            pieces[rook_loc] = pieces[move::convertSquareToFlip(loc::Square64(mve.sq1 - 4))];
            pieces[move::convertSquareToFlip(loc::Square64(mve.sq1 - 4))] = nullptr;

            if (white_up)
                pieces[rook_loc]->setPosition({ (float)((rook_loc % 8) * 100 + 50), (float)((rook_loc / 8) * 100 + 50) });
            else
                pieces[rook_loc]->setPosition({ (float)(700 - (rook_loc % 8) * 100 + 50), (float)(700 - (rook_loc / 8) * 100 + 50) });
        }
    }
}
