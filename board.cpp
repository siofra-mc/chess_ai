#include "board.h"
#include "imgui.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include <cctype>

const char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };


Board::Board(unsigned int fbo) : fbo(fbo) {
	for (int i = 0; i < 64; i++) {
		float left = (i % 8 - 4) * .25f;
		float right = (i % 8 - 3) * .25f;
		float top = (i / 8 - 3) * .25f;
		float bottom = (i / 8 - 4) * .25f;
		
		glm::vec3 color = ((i % 2) ^ (i / 8 % 2)) ? glm::vec3(0.1f, 0.0f, 0.0f) : glm::vec3(1.0f, 1.0f, 1.0f);
		vec3s colors = { color, color, color };
		vec3s top_vertices = { glm::vec3(left, top, 0.0f), glm::vec3(right, top, 0.0f), glm::vec3(left, bottom, 0.0f) };
		vec3s bottom_vertices = { glm::vec3(right, top, 0.0f), glm::vec3(left, bottom, 0.0f), glm::vec3(right, bottom, 0.0f) };
		Square* sqr = new Square(top_vertices, bottom_vertices, colors);
		
		gSquares.push_back(sqr);
	}

 	memset(&board, 0, sizeof(Piece) * 64);
	// Pawns
	for (int file = 0; file < 8; file++) {
		board[8 + file] = Piece(pawn, white);
		board[48 + file] = Piece(pawn, black);
	}

	// Rooks
	board[0] = Piece(rook, white);
	board[7] = Piece(rook, white);
	board[56] = Piece(rook, black);
	board[63] = Piece(rook, black);

	// Knights
	board[1] = Piece(knight, white);
	board[6] = Piece(knight, white);
	board[57] = Piece(knight, black);
	board[62] = Piece(knight, black);

	// Bishop
	board[2] = Piece(bishop, white);
	board[5] = Piece(bishop, white);
	board[58] = Piece(bishop, black);
	board[61] = Piece(bishop, black);

	// Queens
	board[3] = Piece(queen, white);
	board[59] = Piece(queen, black);

	// Kings
	board[4] = Piece(king, white);
	board[60] = Piece(king, black);

	glGenTextures(1, &gBoard);
	glBindTexture(GL_TEXTURE_2D, gBoard);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);


	unsigned int render_buffer_object;
	glGenRenderbuffers(1, &render_buffer_object);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attaching render buffer 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBoard, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer failed.\n" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Board::Board(Board* base) {
	for (int i = 0; i < 64; i++) board[i] = base->board[i];
}

void print_threatmap(uint64_t map) {
	std::cout << HORZ_LINE;
	int sqr;
	for (int rank = 7; rank >= 0; rank--) {
		std::cout << "| ";
		for (int file = 0; file < 8; file++) {
			sqr = BIDX(file, rank);
			std::cout << (char)(XTRC_BIT(map, sqr) + '0');
			std::cout << " | ";
		}
		std::cout << "\b\n" << HORZ_LINE;
	}
	std::cout << "\n";
}

bool Board::makeMove(Piece p, int s, int d) {
	// Make the move
	board[d] = p;
	board[s] = empty_sqr;

	// Extra move on castle
	if (p.kind == king && abs((s % 8) - (d % 8)) == 2) {
		if (p.color == white) {
			if (d % 8 == 6) {
				board[BIDX(5, 0)] = board[BIDX(7, 0)];
				board[BIDX(7, 0)] = empty_sqr;
			}
			if (d % 8 == 2) {
				board[BIDX(3, 0)] = board[BIDX(0, 0)];
				board[BIDX(0, 0)] = empty_sqr;
			}
		}
		if (p.color == black) {
			if (d % 8 == 6) {
				board[BIDX(5, 7)] = board[BIDX(7, 7)];
				board[BIDX(7, 7)] = empty_sqr;
			}
			if (d % 8 == 2) {
				board[BIDX(3, 7)] = board[BIDX(0, 7)];
				board[BIDX(0, 7)] = empty_sqr;
			}
		}
	}

	// Check for promotion
	int promotion_sqr = (p.color == white) ? 7 : 0;
	if ((p.kind == pawn) && (d / 8 == promotion_sqr)) {
		promoting = d;
		return 1;
	}
	return 0;
}

void Board::promote(PieceType type) {
	if (promoting == -1) {
		std::cerr << "No piece able to promote!";
		return;
	}
	if (type == open || type == pawn) {
		std::cerr << "Invalid promotion type!";
		return;
	}

	board[promoting].kind = type;
	promoting = -1;
}

void Board::printBoard(std::string& s) {
	Piece p;
	char piece_c;
	s += HORZ_LINE;
	for (int rank = 7; rank >= 0; rank--) {
		s += "| ";
		for (int file = 0; file < 8; file++) {
			p = board[BIDX(file, rank)];
			piece_c = (p.color == white) ? tolower(piece_chars[p.kind]) : piece_chars[p.kind];
			s += piece_c;
			s += " | ";
		}
		s += "\n";
		s += HORZ_LINE;
	}
}

void Board::render(unsigned int shaderProgram) {
	std::string board_string;
	printBoard(board_string);
	ImGui::Begin("Play window");
	ImGui::Text(board_string.c_str());
	ImGui::End();

	
	if (ImGui::Begin("Gameview", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {		//ImGui::SetCursorPos({0, 0})
		GLuint vertex_buffer, vertex_array;
		glGenVertexArrays(1, &vertex_array);
		glGenBuffers(1, &vertex_buffer);

		std::vector<float> vertex_buffer_data;
		for (int i = 0; i < 64; i++) {
			Square* sqr = gSquares[i];
			sqr->top_triangle.draw(glm::mat4(1), shaderProgram);
			sqr->bottom_triangle.draw(glm::mat4(1), shaderProgram);
			/*float top_left[] = {sqr->top_left.x, sqr->top_left.y, sqr->top_left.z, sqr->color.x, sqr->color.y, sqr->color.z};
			float top_right[] = { sqr->top_right.x, sqr->top_right.y, sqr->top_right.z, sqr->color.x, sqr->color.y, sqr->color.z };
			float bottom_left[] = { sqr->bottom_left.x, sqr->bottom_left.y, sqr->bottom_left.z, sqr->color.x, sqr->color.y, sqr->color.z };
			float bottom_right[] = { sqr->bottom_right.x, sqr->bottom_right.y, sqr->bottom_right.z, sqr->color.x, sqr->color.y, sqr->color.z };
			// top
			for (float j : top_left) vertex_buffer_data.push_back(j);
			for (float j : top_right) vertex_buffer_data.push_back(j);
			for (float j : bottom_left) vertex_buffer_data.push_back(j);

			// bottom
			for (float j : top_right) vertex_buffer_data.push_back(j);
			for (float j : bottom_left) vertex_buffer_data.push_back(j);
			for (float j : bottom_right) vertex_buffer_data.push_back(j);*/
		}
		/*glBindVertexArray(vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_data.size() / 2);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);*/
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Image((void*)(intptr_t)gBoard, ImGui::GetContentRegionAvail());
		glDeleteVertexArrays(1, &vertex_array);
		glDeleteBuffers(1, &vertex_buffer);
	}
	ImGui::End(); 
}