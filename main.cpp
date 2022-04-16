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
           if (isCheck(piece->White())) {
                cout << "White king is under attack.\n";
                // todo: дописати
            }
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
        Position kingPos(-1, -1);
        for (int x = 0; x < getRows(); x++) {
            for (int y = 0; y < getCols(); y++) {
                if (board[x][y]) {
                    if (board[x][y]->Type() == ChessPiece::KING && board[x][y]->White() != white) {
                        kingPos.x = x;
                        kingPos.y = y;
                    }
                }
            }
        }
        if (kingPos.x != -1 && kingPos.y != -1) {
            for (int x = 0; x < getRows(); x++) {
                for (int y = 0; y < getCols(); y++) {
                    if (board[x][y]) {
                        if (board[x][y]->Type() != ChessPiece::KING && board[x][y]->White() != board[kingPos.x][kingPos.y]->White()) { // != white as parameter
                            if (board[x][y]->CanMove(kingPos, this) && IsNothingBetween(Position(x, y), kingPos)) {
                                cout << "\nCheck by ";
                                cout << (board[x][y]->White() ? "white " : "black ");
                                cout << ChessPiece::ToStr(board[x][y]->Type()) << ".\n";
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool IsNothingBetween(Position from, Position to) {
        ChessPiecePtr piece = board[from.x][from.y];
        assert(piece->CanMove(to, this));
        if (piece->Type() == ChessPiece::KING || piece->Type() == ChessPiece::PAWN || piece->Type() == ChessPiece::KNIGHT) {
            return true;
        }

        int dist = max(abs(to.x - from.x), abs(to.y - from.y));
        Position shift((from.x - to.x) / dist, (from.y - to.y) / dist);

        for (int xs = from.x + shift.x, ys = from.y + shift.y; xs < to.x || ys < to.y; xs += shift.x, ys += shift.y) {
            if (board[xs][ys])
                return false;
        }
        return true;
    }

    void Draw() {
        cout << '\n';
        for (int i = 0; i < getCols(); i++) {
            cout << ' ' << i + 1 << "  ";
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
                        cout << '+';
                    } else {
                        cout << '-';
                    }
                    cout << ChessPiece::CharRepresentation(board[i][j]->Type());
                } else {
                    cout << "|___";
                }
            }
            cout << '|' << ' ' << i + 1 << '\n';
        }
        cout << "======================================================\n";
    }
private:
    vector <vector<ChessPiecePtr>> board;
};

int main() {
    ChessBoard board(4, 4);
    auto queen = std::make_shared <Queen>(Position(2, 0), true);
    board.AddPiece(queen);
    board.Draw();
    board.Move(queen, Position(0, 0));
    board.Draw();
    auto king = std::make_shared <King>(Position(2, 0), true);
    board.AddPiece(king);
    board.Draw();

    auto BlackQueen = std::make_shared<Queen>(Position(0, 1), false);
    board.AddPiece(BlackQueen);

    board.Move(BlackQueen, Position(0, 3));
    board.Draw();
    auto bishop = std::make_shared<Bishop>(Position(1, 3), false);
    board.AddPiece(bishop);
    board.Move(bishop, Position(0, 2));
    board.Draw();
    board.Move(bishop, Position(0, 2));
    board.Draw();
    return 0;
}


