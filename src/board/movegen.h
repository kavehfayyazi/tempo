//
// Created by Kaveh Fayyazi on 8/13/25.
//

#ifndef TEMPO_MOVEGEN_H
#define TEMPO_MOVEGEN_H

#include "types.h"
#include <array>
#include <vector>

class MoveGen {
    using Bitboards = std::array<uint64_t, 12>;
    using MoveList = std::vector<uint32_t>;
private:
    inline Piece sideToMoveKing() const;
    inline Piece findPieceAt(uint8_t square, bool wantWhite) const;
    inline uint64_t ourOcc() const;
    inline uint64_t getOcc(bool all, bool isWhite=false, bool opposite=false) const;
    inline bool isCastleFlag(Castling pair) const;
    inline bool kingAt(bool isWhite, uint8_t sq) const;

    void genPawnMovesFor(MoveList& out, Piece pawn) const;
    void genRookMovesFor(MoveList& out, Piece rook) const;
    void genKnightMovesFor(MoveList& from, Piece knight) const;
    void genBishopMovesFor(MoveList& out, Piece bishop) const;
    void genQueenMovesFor(MoveList& out, Piece queen) const;
    void genKingMovesFor(MoveList& out, Piece king) const;

    // Gen evasions helpers
    void genSafeKingMoves(MoveList& out, uint8_t kingSq) const;
    void genPieceCapturesToSquare(MoveList& out, uint8_t sq, Piece piece, Piece captured) const;
    void genCapturesToSquare(MoveList& out, uint8_t attackSq) const;
    void genPawnBlocksToMask(MoveList& out, uint64_t mask) const;
    void genPieceBlocksToMask(MoveList& out, uint64_t mask, Piece piece) const;
    void genQuietBlocks(MoveList& out, uint64_t mask) const;
    void genEPBlock(MoveList& out, uint64_t mask) const;

public:
    void genPseudoMoves(MoveList& out) const;
    void genEvasions(MoveList& out, uint8_t kingSq) const;
    MoveGen(Bitboards& bb, bool& whiteToMove, uint64_t& occWhite, uint64_t& occBlack, uint64_t& occAll, uint8_t& castling, uint8_t& epSquare);

private:
    Bitboards& bb;
    bool& whiteToMove;
    uint64_t& occWhite;
    uint64_t& occBlack;
    uint64_t& occAll;
    uint8_t& castling;
    uint8_t& epSquare;
};
#endif //TEMPO_MOVEGEN_H
