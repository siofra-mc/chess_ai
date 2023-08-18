#pragma once

#include "util.h"
#include "shader.h"
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

struct Piece {
	PieceType kind;
	Color color;
};

#define empty_sqr Piece(open, none);


class Square {
	glm::vec3 top_left_corner;
	bool is_dark;
	Piece piece;

public:
	Square(glm::vec3 tlc, bool dark, Piece p) : top_left_corner(tlc), is_dark(dark), piece(p) {}
	void draw(Shader*);
	void drawTexture(Shader*);
};


class Board { 
	// essential data
	Piece board[64];
	int promoting = -1;
 	// graphical components
	unsigned int gBoard = 0;
	sf::RenderTexture* texture;
	Shader* colorShader = nullptr;
	Shader* pieceShader = nullptr;

public:
	Board(sf::RenderTexture*); // Creates new starting board
	Board(Board*); // Copies board state
	~Board();
	bool makeMove(Piece, int, int);
	void promote(PieceType);
	std::string printBoardString();
	void printBoardImage();
	inline Piece getPiece(int s) { return board[s]; }
	void render();
};