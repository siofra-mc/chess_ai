#pragma once
#include "board.h"
#include "shader.h"
#include <vector>


class Game {
private:
	Board* board = nullptr;
	std::vector<std::string> moves;

public:
	Game(unsigned int);
	Game(Game*);
	~Game();
	void render();
};