#include "game.h"
#include "piece.h"
#include "imgui.h"

uint8_t moveCount = 0;


std::string moveAlgebraicForm(Color c, glm::ivec2 src, glm::ivec2 dest, PieceType type) {
	std::string moveString;

	switch (type) {
	case king: moveString += "K"; break;
	case queen: moveString += "Q"; break;
	case bishop: moveString += "B"; break;
	case knight: moveString += "N"; break;
	case rook: moveString += "R"; break;
	default: moveString += " "; break;
	}

	moveString += dest.x + 'a';
	moveString += dest.y + '1';
	
	return moveString;
}




Game::Game(unsigned int fbo = 0) {
	board = new Board(fbo);
}

Game::Game(Game* base) {
	board = new Board(base->board);
}

Game::~Game() {
	delete board;
}


/*-------------------------------------------------------------------------------------------------------------*\
* Game::pawnSights(std::vector<int>*, int, int, Color, bool?)
* 
* Parameters: moves - Pointer to vector where legal moves will be appended
*             p - Which piece we are checking the moves for
*             file - File where the piece is located (numbered 0-7)
*             rank - Rank where the piece is located (numbered 0-7)
*             threat - Optional, defaults to false. Are we checking threat maps or not?
* Description: Looks at the possible moves for a pawn and appends them to 'moves'
\*-------------------------------------------------------------------------------------------------------------*/


   
/*-------------------------------------------------------------------------------------------------------------*\
* Game::render()
* 
* Description: Sets up the ImGUI context of our game, handles user entered moves, and handles pawn promotion
\*-------------------------------------------------------------------------------------------------------------*/
void Game::render() {
	ImGui::SetNextWindowPos(ImVec2(0, 0)); 
	ImGui::Begin("Play window", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	board->render();

	ImGui::Begin("Gameview");
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !board->isHolding()) {
		ImVec2 wPos  = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos  = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file    = mPos.x / (wSize.x / 8);
		char rank    = 8 - mPos.y / (wSize.y / 8);
		Piece* p     = board->getPiece(glm::ivec2(file, rank));
		if (p && p->getColor() == board->whoseTurn()) {
			board->grab(p);
		}
	} 
	else if (ImGui::IsMouseReleased(0) && board->isHolding()) {
		ImVec2 wPos = ImGui::GetWindowPos();
		ImVec2 wSize = ImGui::GetWindowSize();
		ImVec2 mPos = { io.MousePos.x - wPos.x, io.MousePos.y - wPos.y };
		char file = mPos.x / (wSize.x / 8);
		char rank = 8 - mPos.y / (wSize.y / 8);
		Piece* targetPiece = board->getPiece(glm::ivec2(file, rank));
		Piece* movedPiece = board->drop();
		if (movedPiece) {
			vec2s* legals = movedPiece->legalMoves(false);
			glm::ivec2 attempt = { file,rank };
			if (std::find(legals->begin(), legals->end(), attempt) != legals->end()) {
				moveCount += 1;
				moves.push_back(moveAlgebraicForm(movedPiece->getColor(), movedPiece->getPosition(), attempt, movedPiece->getType()));
				board->makeLegalMove(movedPiece, attempt);
			}
			delete legals;
		}
	}
	ImGui::End();

	ImGui::Begin("history");
	ImGui::ArrowButton("back", ImGuiDir_Left);
	ImGui::SameLine();
	ImGui::ArrowButton("next", ImGuiDir_Right);
	for (int i = 0; i < moves.size(); i++) {
		std::string moveStr = "";
		if (i % 2 == 0) {
			moveStr += i / 2 + '1';
			moveStr += ". ";
		}
		else {
			ImGui::SameLine();
		}
		moveStr += moves.at(i).c_str();
		ImGui::Text(moveStr.c_str());
	}
	ImGui::End();

	/*// Handle user entered moves
	char move[16] = "";
	bool move_entered = ImGui::InputText("Make Move", move, 16, ImGuiInputTextFlags_EnterReturnsTrue);

	// Block moves until promotion has been dealt with
	if (move_entered && !wait_for_promote) makeUserMove(move);
	if (wait_for_promote) {
		PieceType promote_to = open;
		if (ImGui::Button("Queen")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
		if (ImGui::Button("Knight")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
		if (ImGui::Button("Rook")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
		if (ImGui::Button("Bishop")) {
			board->promote<Queen>();
			wait_for_promote = 0;
			ImGui::SameLine();
		}
	}*/
	ImGui::End();
}