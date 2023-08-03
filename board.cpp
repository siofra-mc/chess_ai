#include "board.h"
#include "imgui.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include <cctype>
#include "VAO.h"

#include <Windows.h>

#define WHITE_SQUARE_COLOR glm::vec3(1.f)
#define BLACK_SQUARE_COLOR glm::vec3(.1f, 0.f, .2f)
#define SQUARE_SIZE .25f

glm::vec3 square_top_right_offset = glm::vec3(SQUARE_SIZE, 0.f, 0.f);
glm::vec3 square_bottom_left_offset = glm::vec3(0.f,  -1 * SQUARE_SIZE, 0.f);
glm::vec3 square_bottom_right_offset = glm::vec3(SQUARE_SIZE, -1 * SQUARE_SIZE, 0.f);

const char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };

void Square::draw(unsigned int shaderProgram, unsigned int fbo, bool debug = false) {
	unsigned int stride = 6;
	bool has_texture = false;
	
	GLuint vertex_buffer, vertex_array, element_buffer;
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);

	unsigned int indices[] = {0, 1, 2, 1, 2, 3};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	std::vector<float> vertex_buffer_data;

	glm::vec3 top_right_corner = top_left_corner + square_top_right_offset;
	glm::vec3 bottom_left_corner = top_left_corner + square_bottom_left_offset;
	glm::vec3 bottom_right_corner = top_left_corner + square_bottom_right_offset;
	glm::vec3 color = (is_black) ? BLACK_SQUARE_COLOR : WHITE_SQUARE_COLOR;

	if (debug) {
		std::cerr << "Top left corner: ";
		print_vec3(top_left_corner);
		std::cerr << "Top right corner: ";
		print_vec3(top_right_corner);
		std::cerr << "Bottom left corner: ";
		print_vec3(bottom_left_corner);
		std::cerr << "Bottom right corner: ";
		print_vec3(bottom_right_corner);
	}

	/*std::vector<glm::vec3*> vertices = {&top_left_corner, &top_right_corner, &bottom_left_corner, &top_right_corner, &bottom_left_corner, &bottom_right_corner};
	for (auto vertex : vertices) {
		vertex_buffer_data.push_back(vertex->x);
		vertex_buffer_data.push_back(vertex->y);
		vertex_buffer_data.push_back(vertex->z);
		vertex_buffer_data.push_back(color.x);
		vertex_buffer_data.push_back(color.y);
		vertex_buffer_data.push_back(color.z);
	}*/

	std::vector<float> top_left_array = { top_left_corner.x, top_left_corner.y, top_left_corner.z, color.x, color.y, color.z };
	std::vector<float> top_right_array = { top_right_corner.x, top_right_corner.y, top_right_corner.z, color.x, color.y, color.z };
	std::vector<float> bottom_left_array = { bottom_left_corner.x, bottom_left_corner.y, bottom_left_corner.z, color.x, color.y, color.z };
	std::vector<float> bottom_right_array = { bottom_right_corner.x, bottom_left_corner.y, bottom_left_corner.z, color.x, color.y, color.z };
	  
	int width, height, nChannels;
	if (piece.kind != open) {
		// setup opengl texture object
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::string texture_path = "../../../res/";
		texture_path.append((piece.color == black) ? "black_" : "white_");
		texture_path += piece_chars[piece.kind];
		texture_path.append(".png");
		unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			TCHAR buffer[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, buffer, MAX_PATH);
			std::cerr << "Fail!: " << texture_path << "\n";
			std::cerr << "Working Directory: " << std::string(buffer) << std::endl;
		}
		stbi_image_free(data);

		top_left_array.push_back(0.f); top_left_array.push_back(1.f);
		top_right_array.push_back(1.f); top_right_array.push_back(1.f);
		bottom_left_array.push_back(0.f); bottom_left_array.push_back(0.f);
		bottom_right_array.push_back(1.f); bottom_right_array.push_back(0.f);
		stride = 8;
	}
	
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	if (has_texture) {
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	glUseProgram(shaderProgram);
	glBindVertexArray(vertex_array);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer); 
}

Board::Board(unsigned int fbo) : fbo(fbo) {
	for (int i = 0; i < 64; i++) {
		float left = (i % 8 - 4) * .25f;
		float right = (i % 8 - 3) * .25f;
		float top = (i / 8 - 3) * .25f;
		float bottom = (i / 8 - 4) * .25f;
		
		/*Square* sqr = new Square();
		sqr->top_left = glm::vec3(left, top, 0.0f);
		sqr->top_right = glm::vec3(right, top, 0.0f);
		sqr->bottom_left = glm::vec3(left, bottom, 0.0f);
		sqr->bottom_right = glm::vec3(right, bottom, 0.0f);
		sqr->color = ((i % 2) ^ (i / 8 % 2)) ? glm::vec3(0.1f, 0.0f, 0.0f) : glm::vec3(1.0f, 1.0f, 1.0f);
		gSquares.push_back(sqr);*/
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

void Board::printBoardImage(Shader shader) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	if (ImGui::Begin("Gameview", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {		//ImGui::SetCursorPos({0, 0})
		std::cerr << "Printing new Board...................\n";
		for (int i = 0; i < 64; i++) {
			float left = (i % 8 - 4) * .25f;
			float top = (i / 8 - 3) * .25f;
			glm::vec3 top_left = glm::vec3(left, top, 0.f);
			Square s = Square(top_left, (i % 2) ^ (i / 8 % 2), board[i]);
			s.draw(shaderProgram, fbo);
		}
		ImGui::Image((void*)(intptr_t)gBoard, ImGui::GetContentRegionAvail());
	
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::Image((void*)(intptr_t)gBoard, ImGui::GetContentRegionAvail());
		std::cerr << "===================================================================\n";
	}
}

void Board::render(unsigned int shaderProgram) {
	std::string board_string;
	printBoard(board_string);
	ImGui::Begin("Play window");
	ImGui::Text(board_string.c_str());
	ImGui::End();

	
	printBoardImage(shaderProgram); 
	ImGui::End(); 
}