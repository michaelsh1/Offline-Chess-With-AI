#pragma once
#include "Defs.hpp"

#ifndef BOARD
#define BOARD
#include "Piece.hpp"

class Board
{
public:
    // main board array
    int Square[64] = {0};

    // castling rights and turn
    int rights = 0;
    int fifty_move_rule = 0;

    loc::Square64 en_passant_square;

    Board(const char *FEN = START_FEN);

    void clearBoard();

    void setFEN(const char *FEN);

    void printBoard();

    // true if successful false if not
    bool movePiece(char move[5], std::list<move::MoveCell> *move_list, move::MoveType &move_type, int promote_to = PieceType::queen);

    std::list<move::MoveCell> *calculateMoveList(loc::Square64 square);

    bool isGameOver(game_outcomes &outcome);
};


class VisualBoard
{
public:
    // 0 = black, 1 = white
    static std::map<int, sf::Texture*> textures;
    sf::Sprite *pieces[64];

    // std::unique_ptr<sf::RectangleShape> from_highlighter = nullptr, 
    //                                     to_highlighter = nullptr;

    bool white_up = true;

    void loadTextures();

    void loadSprites(int board[64]);

    static void drawEmptyBoard(sf::RenderWindow &window,
                        sf::Color color_dark = sf::Color(209, 139, 71),
                        sf::Color color_light = sf::Color(255, 206, 158));

    void flipBoard(bool _white_up = true);

    void displayBoard(sf::RenderWindow &window,
                      sf::Color color_dark = sf::Color(209, 139, 71),
                      sf::Color color_light = sf::Color(255, 206, 158),
                      std::list<move::MoveCell> *highlight_moves = nullptr,
                      sf::Color highlight_color = sf::Color(201, 20, 20, 130));

    // returns true if the move was successful and legal, false if not
    bool convertMouseToMove(sf::Vector2i mouse_position_press,
                            sf::Vector2i mouse_position_release,
                            char move[5]);

    void movePiece(Board &board, char move[5], move::MoveType move_type, int promote_to = PieceType::queen);
};

#endif // !BOARD