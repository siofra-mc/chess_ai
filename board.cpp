#include "board.h"
#include "imgui.h"
#include "gl/glew.h"
#include <cctype>
#include "VAO.h"
#include "EBO.h"

#include <Windows.h>

// Colors for the squares of the board
#define LIGHT_SQUARE_COLOR glm::vec3(1.f)
#define DARK_SQUARE_COLOR glm::vec3(.1f, 0.f, .2f)

// Offsets to the corners of the board squares
#define SQUARE_SIZE .25f
glm::vec3 square_top_right_offset = glm::vec3(SQUARE_SIZE, 0.f, 0.f);
glm::vec3 square_bottom_left_offset = glm::vec3(0.f,  -1 * SQUARE_SIZE, 0.f);
glm::vec3 square_bottom_right_offset = glm::vec3(SQUARE_SIZE, -1 * SQUARE_SIZE, 0.f);

const char piece_chars[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };

/*-------------------------------------------------------------------------------------------------------------*\
* Square::draw(Shader*)
* 
* Parameters: shader - Pointer to shader used to draw our square.
* Description: Draws a solid colored square
\*-------------------------------------------------------------------------------------------------------------*/
void Square::draw(Shader* shader) {
	// Vertex data for this square
	glm::vec3 top_right_corner = top_left_corner + square_top_right_offset;
	glm::vec3 bottom_left_corner = top_left_corner + square_bottom_left_offset;
	glm::vec3 bottom_right_corner = top_left_corner + square_bottom_right_offset;
	glm::vec3 color = (is_dark) ? DARK_SQUARE_COLOR : LIGHT_SQUARE_COLOR;

	// Vertex and index buffer data
	float vertices[] = {
		top_left_corner.x, top_left_corner.y, top_left_corner.z, color.x, color.y, color.z,
		top_right_corner.x, top_right_corner.y, top_right_corner.z, color.x, color.y, color.z,
		bottom_left_corner.x, bottom_left_corner.y, bottom_left_corner.z, color.x, color.y, color.z,
		bottom_right_corner.x, bottom_right_corner.y, bottom_right_corner.z, color.x, color.y, color.z
	};
	unsigned int indices[] = {0, 1, 2, 1, 2, 3};

	// Create opengl objects
	VAO* vertex_array = new VAO();
	vertex_array->bind();
	VBO* vertex_buffer = new VBO(vertices, sizeof(vertices));
	EBO* element_buffer = new EBO(indices, sizeof(indices));

	// Link the position and color attributes
	vertex_array->linkAttribute(vertex_buffer, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	vertex_array->linkAttribute(vertex_buffer, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// Draw
	shader->activate();
	vertex_array->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Cleanup
	shader->deactivate();
	vertex_array->unbind();
	vertex_buffer->unbind();
	element_buffer->unbind();
	delete vertex_array;
	delete vertex_buffer;
	delete element_buffer;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Square::drawTexture(Shader*)
* 
* Parameters: shader - Pointer to shader used to draw the piece texture within this square
* Description: Draws a chess piece over this square
\*-------------------------------------------------------------------------------------------------------------*/
void Square::drawTexture(Shader* shader) {
	// Vertex data for this square
	glm::vec3 top_right_corner = top_left_corner + square_top_right_offset;
	glm::vec3 bottom_left_corner = top_left_corner + square_bottom_left_offset;
	glm::vec3 bottom_right_corner = top_left_corner + square_bottom_right_offset;

	// Vertex and index buffer data 
	GLfloat vertices[] = {
		top_left_corner.x, top_left_corner.y, top_left_corner.z - .1f, 0.f, 1.f,
		top_right_corner.x, top_right_corner.y, top_right_corner.z - .1f, 1.f, 1.f,
		bottom_left_corner.x, bottom_left_corner.y, bottom_left_corner.z - .1f, 0.f, 0.f,
		bottom_right_corner.x, bottom_right_corner.y, bottom_right_corner.z - .1f, 1.f, 0.f
	};
	GLuint indices[] = { 0, 1, 2, 1, 2, 3 };

	// Create opengl elements
	VAO* vertex_array = new VAO();
	vertex_array->bind();
	VBO* vertex_buffer = new VBO(vertices, sizeof(vertices));
	EBO* element_buffer = new EBO(indices, sizeof(indices));

	// Link position and texture coordinate attributes
	vertex_array->linkAttribute(vertex_buffer, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	vertex_array->linkAttribute(vertex_buffer, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// setup opengl texture object 
	int width, height, nChannels;
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Get texture from file through stb image
	std::string texture_path = "../../../res/textures/";
	texture_path.append((piece.color == black) ? "black_" : "white_");
	texture_path += piece_chars[piece.kind];
	texture_path.append(".png");
	unsigned char* data = stbi_load(texture_path.c_str(), &width, &height, &nChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::cerr << "Fail!: " << texture_path << "\n";
		std::cerr << "Working Directory: " << std::string(buffer) << std::endl;
	}
	stbi_image_free(data);


	// Draw the piece
	shader->activate();
	glEnable(GL_BLEND);
	glUniform1i(glGetUniformLocation(shader->ID, "ourTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	vertex_array->bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Cleanup
	shader->deactivate();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	vertex_array->unbind();
	vertex_buffer->unbind();
	element_buffer->unbind();
	glDeleteTextures(1, &texture);
	delete vertex_array;
	delete vertex_buffer;
	delete element_buffer;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(GLuint)
* 
* Parameters: fbo - ID of the FrameBuffer Object that the board will be drawn onto
* Description: Creates a new Board with the classical starting position
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(sf::RenderTexture* texture) : texture(texture) {
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


	/*unsigned int render_buffer_object;
	glGenRenderbuffers(1, &render_buffer_object);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	*/
	// attaching render buffer 
	/*glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBoard, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object);
	*/

	/*if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame buffer failed.\n" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	//colorShader = new Shader("../../../res/shaders/default.vert", "../../../res/shaders/default.frag");
	//pieceShader = new Shader("../../../res/shaders/piece.vert", "../../../res/shaders/piece.frag");
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::Board(Board*)
* 
* Parameters: base - Pointer to the board that will be copied
* Description: Makes a new board which is a copy of 'base'. Used to test moves
\*-------------------------------------------------------------------------------------------------------------*/
Board::Board(Board* base) {
	for (int i = 0; i < 64; i++) board[i] = base->board[i];
}

// Performs cleanup before deleting the board
Board::~Board() {
	if (pieceShader) delete pieceShader;
	if (colorShader) delete colorShader;
	glDeleteTextures(1, &gBoard);
}

/*-------------------------------------------------------------------------------------------------------------*\
* print_threatmap(uint64_t)
* 
* Parameters: map - 64 bit representation of a threatmap
* Description: Prints out an 8x8 ASCII representation of a threatmap to the console
\*-------------------------------------------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------------------------------------------*\
* Board::makeMove(Piece, int, int)
* 
* Parameters: p - Piece that is being moved
*             s - Index of the source square
*             d - Index of the destination square
* Description: Handles moving a piece on the board and checks if we need to promote a pawn
* Return Value: True if we need to promote a pawn, false otherwise
\*-------------------------------------------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------------------------------------------*\
* Board::promote(PieceType)
* 
* Parameters: type - Kind of piece the pawn is promoting to
* Description: Handles pawn promotion
\*-------------------------------------------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------------------------------------------*\
* Board::printBoard(std::string&)
* 
* Description: Outputs a string represention of the current state of the board
* Return: String representation of the board to be printed
\*-------------------------------------------------------------------------------------------------------------*/
std::string Board::printBoardString() {
	Piece p;
	char piece_c;
	std::string s = HORZ_LINE;
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
	return s;
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::printBoardImage(Shader*)
* 
* Description Prints out the graphical representation of the board.
\*-------------------------------------------------------------------------------------------------------------*/
void Board::printBoardImage() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 
	if (ImGui::Begin("Gameview", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar)) {		//ImGui::SetCursorPos({0, 0})
		for (int i = 0; i < 64 ; i++) {
			float left = (i % 8 - 4) * .25f;
			float top = (i / -8 + 4) * .25f;
			glm::vec3 top_left = glm::vec3(left, top, 0.f);
			bool isBlack = (i % 2) ^ (i / 8 % 2);
			Piece p = board[i];
			Square* s = new Square(top_left, isBlack, p);

			if (p.kind != open) {
				//s->draw(colorShader);
				//s->drawTexture(pieceShader);
			}
			delete s;
		}
	
		/*glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		*/ 
		ImGui::Image((void*)(intptr_t)gBoard, ImGui::GetContentRegionAvail());
	}
	ImGui::End(); 
}

/*-------------------------------------------------------------------------------------------------------------*\
* Board::render()
* 
* Description: Handles how the board will be rendered. Calls for both the graphical and text based renderings.
\*-------------------------------------------------------------------------------------------------------------*/
void Board::render() {
	std::string board_string = printBoardString();
	ImGui::Begin("Play window");
	ImGui::Text(board_string.c_str());
	ImGui::End();
	
 
	//printBoardImage(); 
}