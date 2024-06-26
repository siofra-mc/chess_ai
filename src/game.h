#pragma once
#include "board.h"
#include "Texture.h"
#include <vector>


// Masks for game status
constexpr uint16_t WHITE_SHORT_CASTLE	= 0x001;
constexpr uint16_t WHITE_LONG_CASTLE	= 0x002;
constexpr uint16_t BLACK_SHORT_CASTLE	= 0x004;
constexpr uint16_t BLACK_LONG_CASTLE	= 0x008;
constexpr uint16_t WHITE_CHECK			= 0x010;
constexpr uint16_t BLACK_CHECK			= 0x020;
constexpr uint16_t PROMOTING			= 0x040;
constexpr uint16_t WHOSE_TURN			= 0x080;
constexpr uint16_t PLAY_STATUS			= 0x300;

// PLAY_STATUS values
constexpr uint16_t PLAYING		= 0x000;
constexpr uint16_t WHITE_WIN	= 0x100;
constexpr uint16_t BLACK_WIN	= 0x200;
constexpr uint16_t STALEMATE	= 0x300;


class Game {
protected:
	Board* board = nullptr;
	std::vector<std::string> moveList;

	int promotionSubject = -1;

	uint16_t gameStatus = 0xF;
	// Where are the kings
	uint8_t whiteKing;
	uint8_t blackKing;
	uint64_t white_threat_map = 0ULL;
	uint64_t black_threat_map = 0ULL;
	int8_t enPassantSquare = -1;

	bool isWaitingOnPromotion() const { return gameStatus & PROMOTING; }
	bool canCastle(Castling) const;
	Color whoseTurn() const;
	bool makeLegalMove(Piece*, uint8_t);
	bool isInCheck(Color) const;
	void updateThreatMaps();
	void updateChecks();
	bool move(Piece*, uint8_t);
	void handlePromotion();
	bool hasLegalMove(Color);
	std::vector<uint8_t>* getLegalPieceMoves(Piece*, bool);
	bool check4check(Piece*, uint8_t, bool = false);
	void checkIfGameEnded();


	template<class T>
	void promote() {
		if (promotionSubject == -1) {
			std::cerr << "No piece able to promote!";
			return;
		}

		Piece* toDelete = board->getPiece(promotionSubject);
		Piece* promotedPiece = createPiece<T>(toDelete->getColor(), toDelete->getPosition(), board);
		board->placePiece(promotedPiece, promotionSubject);
		promotionSubject = -1;
		gameStatus &= ~PROMOTING;
		delete toDelete;

		std::string promotionMove = moveList[moveList.size() - 1];
		moveList.pop_back();
		moveList[moveList.size() - 1] = promotionMove + "=" + promotedPiece->textboardSymbol();
		gameStatus ^= WHOSE_TURN;
	}
public:
	Game();
	Game(Game*);
	~Game();
};

class GraphicalGame : public Game {
	unsigned int fbo = 0;
	Shader* colorShader = nullptr;
	Shader* pieceShader = nullptr;

	GLuint boardGraphic = 0;
	Texture* queenPromotion = 0;
	Texture* rookPromotion = 0;
	Texture* knightPromotion = 0;
	Texture* bishopPromotion = 0;
	Piece* held = nullptr;


	bool isHolding() { return (held != nullptr); }
	void printBoardImage();
	void printMoveList();
	void grab(Piece*);
	Piece* drop();
	void handlePromotion();
public:
	GraphicalGame(unsigned int);
	~GraphicalGame();
	void createPromotionTextures();
	void deletePromotionTextures();
	void render();

	template<class T>
	void promote() {
		Game::promote<T>();
		deletePromotionTextures();
	}
};