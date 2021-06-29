#pragma once

#define _WIN32_WINNT 0x0A00

#ifndef DEFS
#define DEFS

#include "Includes.hpp"
#define SFML_NO_DEPRECATED_WARNINGS

// --------------------------pieces & rights-------------------
enum PieceType
{
	none = 0,
	king = 1,
	pawn = 2,
	knight = 3,
	bishop = 4,
	rook = 5,
	queen = 6,

	white = 16,
	black = 8,

	
	color_mask = 24,
	piece_mask = 7
};

enum Rights
{
	// \
    & with the not of these to disable. \
    add / subtract 1 to change the turn.
	king_side_white = 2,
	queen_side_white = 4,
	king_side_black = 8,
	queen_side_black = 16
};

// --------------------------screen dimensions-----------------

enum game_modes { Offline, Online, AI };
enum game_outcomes { BLACK, DRAW, WHITE, _ERROR };

namespace loc
{
	enum Square64
	{
		a1, b1, c1, d1, e1, f1, g1, h1,
		a2, b2, c2, d2, e2, f2, g2, h2,
		a3, b3, c3, d3, e3, f3, g3, h3,
		a4, b4, c4, d4, e4, f4, g4, h4,
		a5, b5, c5, d5, e5, f5, g5, h5,
		a6, b6, c6, d6, e6, f6, g6, h6,
		a7, b7, c7, d7, e7, f7, g7, h7,
		a8, b8, c8, d8, e8, f8, g8, h8, ER = 99
	};

	enum Files { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
	enum Ranks { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };
}

namespace move
{
	enum MoveType
	{
		normal,
		en_passant,
		take,
		promote
	};

	template<typename T>
	struct TwoSqrVec { T sq1, sq2; };

	struct MoveCell
	{
		char move[5];
		MoveType type;

		static void fillMoveStr(TwoSqrVec<loc::Square64> sq, char move[5])
		{
			move[0] = (char)('a' + (int)(sq.sq1 % 8));
			move[1] = (char)((int)(sq.sq1 / 8) + '1');

			move[2] = (char)('a' + (int)(sq.sq2 % 8));
			move[3] = (char)((int)(sq.sq2 / 8) + '1');

			move[4] = '\0';
		}

		static TwoSqrVec<loc::Square64> returnMoveVec(const char move[5])
		{
			return {loc::Square64((move[0] - 'a') + (move[1] - '1') * 8),
					loc::Square64((move[2] - 'a') + (move[3] - '1') * 8)};
		}
	};

	static TwoSqrVec<loc::Square64> returnMoveVecflip(const char move[5])
	{
		return { loc::Square64(7 - (move[0] - 'a') + (move[1] - '1') * 8),
				 loc::Square64(7 - (move[2] - 'a') + (move[3] - '1') * 8) };
	}
	
	static loc::Square64 convertSquareToFlip(loc::Square64 square)
	{
		return loc::Square64((square / 8) * 8 + 7 - square % 8);
	}

	static void copyMove(const char from[5], char to[5])
	{
		to[0] = from[0];
		to[1] = from[1];
		to[2] = from[2];
		to[3] = from[3];
		to[4] = from[4];
	}

	static bool cmpMove(const char move1[5], const char move2[5])
	{
		if (
			move1[0] == move2[0] &&
			move1[1] == move2[1] &&
			move1[2] == move2[2] &&
			move1[3] == move2[3]
		)
			return true;

		return false;
	}

	static void makeSingleMove(loc::Square64 sq, char move[3])
	{
		move[0] = (char)('h' - (int)(sq % 8));
		move[1] = (char)((int)(sq / 8) + '1');

		move[2] = '\0';
	}

	static loc::Square64 returnSingleMove(const char move[3])
	{
		return loc::Square64((move[0] - 'a') + (move[1] - '1') * 8);
	}
}

#define TO_LOWER(char) ((char) + 32)

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

//---------------------------utility functions-----------------
static sf::Vector2f pos2XY(int pos, int board_width)
{
	return sf::Vector2f(pos % board_width, pos / board_width);
}
#define POS_XY(pos) pos2XY(pos, 8)

static bool isMouseOver(sf::Sprite *sprite, sf::RenderWindow &window)
{
	if (sprite->getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
		return true;
	return false;
}

static bool loadFont(sf::Font &font, const char *filePath)
{
	if (!font.loadFromFile(filePath))
	{
		MessageBoxA(NULL, "Unable to load font", "Font loading error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

// ---------------------------macros--------------------------- //
#define INFINITY std::numeric_limits<int>::max()

// ---------------------------externs--------------------------- //
static sf::Font VarelaRound, SourceSans;
static sf::Cursor cursor;

static void changeCursor(sf::Cursor::Type type, sf::RenderWindow &window) 
{
	if (cursor.loadFromSystem(type))
		window.setMouseCursor(cursor);
}

#endif // !DEFS

// -------------------------heat maps------------------------- //
const int bishopHeatMap[64] =
{
	 5, 0,  0,  -5, -5, 0,  0,  5,
	 0, 10, 5,  0,  0,  5,  10, 0,
	 0, 5,  15, 10, 10, 15, 5,  0,
	-5, 0,  10, 20, 20, 10, 0, -5,
	-5, 0,  10, 20, 20, 10, 0, -5,
	 0, 5,  15, 10, 10, 15, 5,  0,
	 0, 10, 5,  0,  0,  5,  10, 0,
	 5, 0,  0,  -5, -5, 0,  0,  5,
};

