#pragma once
#include "board.h"
#include "shader.h"
#include <vector>
#include <SFML/Graphics.hpp>


class Game {
private: 
	Board* board = nullptr;
	// Who's in check
	bool white_check = 0;
	bool black_check = 0;
	// Where are the kings
	int white_king;
	int black_king;
	Color whose_turn = none;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	// Which file has an en passant opportunity
	int white_en_passant = -1;
	int black_en_passant = -1;
	// Castling availabilities
	bool white_short_castle = 1;
	bool black_short_castle = 1;
	bool white_long_castle = 1;
	bool black_long_castle = 1;
	// Are we waiting for the player to decide how to promote their pawn
	bool wait_for_promote = 0;

public:
	Game(sf::RenderTexture*);
	Game(Game*);
	~Game();
	void render();

private:
	void makeUserMove(std::string);
	void makeLegalMove(Piece, int, int);
	void allLegalMoves(std::vector<int>*, Color);
	void legalPieceMoves(std::vector<int>*, Piece, int, int);
	void pawnSights(std::vector<int>*, int, int, Color, bool threat = false);
	void knightSights(std::vector<int>*, int, int, Color, bool threat = false);
	void bishopSights(std::vector<int>*, int, int, Color, bool threat = false);
	void rookSights(std::vector<int>*, int, int, Color, bool threat = false);
	void kingSights(std::vector<int>*, int, int, Color, bool threat = false);
	void updateThreatMaps();
};