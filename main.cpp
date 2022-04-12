#include <iostream>
#include <utility>
#include <cassert>
#include <memory>
#include <vector>
using namespace std;

struct Position {
    int x;
    int y;
    Position(int x, int y) : x(x), y(y) {}
};

class ChessBoard;
class ChessPiece {
public:
    ChessPiece(Position pos, bool white) : position(pos), white(white) {};
    enum PieceType {
        QUEEN,
        KING,
        BISHOP,
        KNIGHT,
        ROOK,
        PAWN
    };
#define TO_STR(x) case x: return #x;
    static string ToStr(PieceType type) {
        switch (type) {
            TO_STR(QUEEN)
            TO_STR(KING)
            TO_STR(BISHOP)
            TO_STR(KNIGHT)
            TO_STR(ROOK)
            TO_STR(PAWN)
        }
    }
    static string CharRepresentation(PieceType type) {
        switch(type) {
            case QUEEN : return "Qn";
            case KING : return "Kg";
            case BISHOP : return "Bp";
            case KNIGHT : return "Kn";
            case ROOK : return "Rk";
            case PAWN : return "Pn";
        }
    }

    virtual PieceType Type() = 0;
    virtual bool CanMove(Position to, ChessBoard *board) = 0;
    Position getPosition() {
        return position;
    }
    void setPosition(int x, int y) {
        position.x = x;
        position.y = y;
    }
    bool White() {
        return white;
    }
protected:
    bool white = false;
    bool killed = false;
    Position position;
};

using ChessPiecePtr = shared_ptr<ChessPiece>;

class ChessBoard {
public:
    ChessBoard(int rows, int cols) {
        board = vector<vector<ChessPiecePtr>>(rows, vector <ChessPiecePtr>(cols, nullptr));
    }

    bool AddPiece(ChessPiecePtr piece) {
        auto pos = piece->getPosition();
        if (isOnBoard(pos)) {
            if (!Piece(pos)) {
                board[pos.x][pos.y] = piece;
                return true;
            }
        } else {
            cout << "Can't put a chess piece on cell with negative coordinates.\n";
            return false;
        }
    };

    bool isOnBoard(Position to) {
        return to.x >= 0 && to.x < getRows() && to.y >= 0 && to.y < getCols();
    }
    bool Move(ChessPiecePtr piece, Position to) {
        if (isOnBoard(to) && piece->CanMove(to, this)) {
            if (board[to.x][to.y]) {
                if (board[to.x][to.y]->White() == piece->White()) {
                    cout << "Can't take friendly piece.\n";
                    return false;
                } else {
                    cout << "The " << ChessPiece::ToStr(board[to.x][to.y]->Type()) << " was taken by " << ChessPiece::ToStr(piece->Type()) << ".\n";
                }
            }
            board[piece->getPosition().x][piece->getPosition().y] = nullptr;
            board[to.x][to.y] = piece;
            piece->setPosition(to.x, to.y);
            return true;
        } else {
            cout << ChessPiece::ToStr(piece->Type()) << " can't move like this.\n";
            return false;
        }
    }
    ChessPiecePtr Piece(Position pos) {
        return board[pos.x][pos.y];
    }

    int getRows() {
        return board.size();
    }
    int getCols() {
        return board.front().size();
    }

    bool isWhite(const ChessPiecePtr& piece) {
        return piece->White();
    }

    bool isCheck(bool white) {
        for (int i = 0; i < getRows(); i++) {
            for (int j = 0; j < getCols(); j++) {
                if (board[i][j]->Type() == ChessPiece::KING && board[i][j]->White() == white) {

                }
            }
        }
        for (int i = 0; i < getRows(); i++) {
            for (int j = 0; j < getCols(); j++) {
                if (board[i][j]->Type() != ChessPiece::KING && board[i][j]->White() != white) {
//                    if (board[i][j].)
                }
            }
        }
    }

    void Draw() {
        cout << '\n';
        for (int i = 0; i < getCols(); i++) {
            cout << ' ' << i + 1 << " ";
        }
        cout << '\n';
        for (int i = 0; i < getCols(); i++) {
            cout << " ___";
        }
        cout << '\n';
        for (int i = 0; i < getRows(); i++) {
            for (int j = 0; j < getCols(); j++) {
                if (Piece({i, j})) {
                    cout << '|';
                    if (Piece({i, j})->White()) {
                        cout << '.';
                    }
                    cout << ChessPiece::CharRepresentation(board[i][j]->Type());
                } else {
                    cout << "|___";
                }
            }
            cout << '|' << ' ' << i + 1 << '\n';
        }
    }
private:
    vector <vector<ChessPiecePtr>> board;
};

class Queen : public ChessPiece {
public:
    Queen(Position pos, bool white) : ChessPiece(pos, white) {};
    PieceType Type() override {
        return QUEEN;
    }
    bool CanMove(Position to, ChessBoard *board) override {
        bool diagonal = abs(to.x - position.x) == abs(to.y - position.y);
        bool left_right = to.y - position.y == 0;
        bool up_down = to.x - position.x == 0;
        if (diagonal || left_right || up_down) {
            //while
            return true;
        }
        return false;
    }
};

class King : public ChessPiece {
public:
    King(Position pos, bool white) : ChessPiece(pos, white) {};
    PieceType Type() override {
        return KING;
    }
    bool CanMove(Position to,  ChessBoard *board) override {
        int p_x = abs(to.x - position.x);
        int p_y = abs(to.y - position.y);
        bool diagonal = (p_x == p_y) && (p_x == 1) && (p_y == 1);
        bool left_right = (p_y == 0) && (p_x == 1);
        bool up_down = (p_x == 0) && (p_y == 1);
        if (diagonal || left_right || up_down) {
            return true;
        }
        return false;
    }
private:
    bool hadCastle = false;
};

class Bishop : public ChessPiece {
public:
    Bishop(Position pos, bool white) : ChessPiece(pos, white) {};
    PieceType Type() override {
        return BISHOP;
    }
    bool CanMove(Position to,  ChessBoard *board) override {
        bool diagonal = abs(to.x - position.x) == abs(to.y - position.y);
        if (diagonal) {
            return true;
        }
        return false;
    }
};

class Knight : public ChessPiece {
    Knight(Position pos, bool white) : ChessPiece(pos, white) {};
    PieceType Type() override {
        return KNIGHT;
    }
    bool CanMove(Position to, ChessBoard *board) override {
        int p_x = abs(to.x - position.x);
        int p_y = abs(to.y - position.y);
        bool first_move = p_x == 1 && p_y == 2;
        bool second_move = p_x == 2 && p_y == 1;
        if (first_move || second_move) {
            return true;
        }
        return false;
    } //подив чи працює
};

class Pawn : public ChessPiece {
public:
    Pawn(Position pos, bool white, bool isFirstMove) : ChessPiece(pos, white), isFirstMove(isFirstMove) {};
    PieceType Type() override {
        return PAWN;
    }
    bool CanMove(Position to,  ChessBoard *board) override {
        int p_x = abs(to.x - position.x);
        int p_y = abs(to.y - position.y);
        if (isFirstMove) {
            if ((p_y == 2 || p_y == 1) && (p_x == 0)) {
                return true;
            }
        } else {
            if (p_y == 1 && p_x == 0) {
                return true;
            }
        }
        return false;
    }
//    bool CanTake(Position to) override {
//        int p_x = to.x - position.x;
//        int p_y = to.y - position.y;
//        if (p_x == 1 && p_y == 1) {
//            return true;
//        }
//        return false;
//    }
private:
    bool isFirstMove = true;
};

int main() {
    ChessBoard board(4, 4);
    auto queen = std::make_shared <Queen>(Position(3, 1), true);
    board.AddPiece(queen);
    board.Draw();
    board.Move(queen, Position(1, 1));
    auto king = std::make_shared <King>(Position(3, 1), true);
    board.AddPiece(king);
    board.Draw();
    board.Move(king, Position(2,0));
    board.Draw();
    board.Move(queen, Position(2, 0));
    board.Draw();
    return 0;
}
