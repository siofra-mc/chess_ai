#pragma once

#include "util.h"
#include "triangle.h"
#include <string>
#include <iostream>

struct Piece {
	PieceType kind;
	Color color;
};

#define empty_sqr Piece(open, none);


// TODO: move into geometries
struct Square {
	glm::vec3 top_left;
	glm::vec3 top_right;
	glm::vec3 bottom_left;
	glm::vec3 bottom_right;
	glm::vec3 color;
	Triangle top_triangle;
	Triangle bottom_triangle;

	Square(vec3s top, vec3s bot, vec3s colors) {
		top_triangle = Triangle(top, colors);
		bottom_triangle = Triangle(bot, colors);
	}
};


struct Board { 
private:
	// essential data
	Piece board[64];
	int promoting = -1;

	// graphical components
	std::vector<Square*> gSquares;
	unsigned int gBoard;
	unsigned int fbo;

public:
	Board(unsigned int); // Creates new starting board
	Board(Board*); // Copies board state
	bool makeMove(Piece, int, int);
	void promote(PieceType);
	void printBoard(std::string&);
	inline Piece getPiece(int s) { return board[s]; }
	void render(unsigned int);
};