//
// Created by Kaveh Fayyazi on 8/13/25.
//

#include "attacks.h"
#include "push.h"
#include "movegen.h"
#include <iostream>
#include "types.h"
#include <bit>
#include <vector>
#include <assert.h>

using MoveList = std::vector<uint32_t>;

inline Piece MoveGen::sideToMoveKing() const {
    return whiteToMove ? Piece::WK : Piece::BK;
}

// find piece at sq for color wantWhite
// returns Piece::None if not found
inline Piece MoveGen::findPieceAt(uint8_t square, bool wantWhite) const {
    const auto& set = wantWhite ? WHITE_PIECES : BLACK_PIECES;
    for (Piece piece : set) if (isPieceAtSquare(bb, piece, square)) return piece;
    return Piece::None;
}

inline uint64_t MoveGen::ourOcc() const { return whiteToMove ? occWhite : occBlack; }

inline uint64_t MoveGen::getOcc(bool all, bool isWhite, bool opposite) const {
    if (all) return occAll;
    if (isWhite) return opposite ? occBlack : occWhite;
    else return opposite ? occWhite : occBlack;
}

inline bool MoveGen::isCastleFlag(Castling pair) const { return castling & to_u(pair); }
inline bool MoveGen::kingAt(bool isWhite, uint8_t sq) const {
    Piece king = isWhite ? Piece::WK : Piece::BK;
    return bb[to_u(king)] & (1ULL << sq);
}

// works for both white and black pawns
// FWD is the incrementor for rank above current
void MoveGen::genPawnMovesFor(MoveList& out, Piece pawn) const {
    bool meWhite = isWhite(pawn);
    const uint8_t topRank = meWhite ? EIGHTH_RANK : FIRST_RANK;
    const uint8_t seventhRank = meWhite ? SEVENTH_RANK : SECOND_RANK;
    const uint8_t sixthRank = meWhite ? SIXTH_RANK : THIRD_RANK;
    const uint8_t secondRank = meWhite ? SECOND_RANK : SEVENTH_RANK;
    const uint8_t epRank = meWhite ? FIFTH_RANK : FOURTH_RANK;
    const uint8_t leftFile = meWhite ? A_FILE : H_FILE;
    const uint8_t rightFile = meWhite ? H_FILE : A_FILE;
    const int8_t leftSQdx = meWhite ? 1 : - 1;
    const int8_t rightSQdx = meWhite ? -1 : 1;
    const int8_t FWD = meWhite ? NUM_SQUARES_IN_ROW : (-(int8_t)NUM_SQUARES_IN_ROW);

    forEachSetBit(bb[to_u(pawn)], [&](uint8_t square) {
        if (squareInRank(square, topRank)) return; // Last rank

        // Quiet push promotion
        if (squareInRank(square, seventhRank) && !occupied(getOcc(true), square + FWD))
            pushPromo(out, square, square + FWD, pawn, false);

        // Capture to left
        if (fileOf(square) != leftFile &&
            occupied(getOcc(false, meWhite, true),square + FWD + leftSQdx))
            if (squareInRank(square, seventhRank)) // Promotion capture
                pushPromo(out, square, square + FWD + leftSQdx, pawn, true, findPieceAt(square + FWD + leftSQdx, !isWhite(pawn)));
             else // Normal capture
                 pushCapture(out, square, square + FWD + leftSQdx, pawn, enemyPieceAt(bb, square + FWD + leftSQdx, meWhite));

         // Capture to right
        if (fileOf(square) != rightFile&&
            occupied(getOcc(false, meWhite, true),square + FWD + rightSQdx))
            if (squareInRank(square, seventhRank)) // Promotion capture
                pushPromo(out, square, square + FWD + rightSQdx, pawn, true, findPieceAt(square + FWD + rightSQdx, !meWhite));
             else // Normal capture
                 pushCapture(out, square, square + FWD + rightSQdx, pawn, enemyPieceAt(bb, square + FWD + rightSQdx, meWhite));

        // En Passant
        if (squareInRank(square, epRank)) {
            Piece otherPawn = getOtherPiece(pawn);
            if (fileOf(square) != leftFile &&
                epSquare == square + FWD + leftSQdx) // Left
                pushCapture(out, square, square + FWD + leftSQdx, pawn, otherPawn, true);
            if (fileOf(square) != rightFile &&
                epSquare == square + FWD + rightSQdx) // Left
                pushCapture(out, square, square + FWD + rightSQdx, pawn, otherPawn, true);
        }

        // Any forward push (includes double pawn push)
        if (!occupied(occAll, square + FWD)) { // Single push
            pushQuiet(out, square, square + FWD, pawn);
            if (squareInRank(square, secondRank) && // Double pawn push
                !occupied(occAll, square + 2 * FWD))
                pushQuiet(out, square, square + 2 * FWD, pawn, true);
        }
    });
}

// works for both white and black pawns
// FWD is the incrementor for rank above current
void MoveGen::genRookMovesFor(MoveList& out, Piece rook) const {
    forEachSetBit(bb[to_u(rook)], [&](uint8_t square) {
        for(size_t i = 0; i < ROOK_DELTAS.size(); ++i)
            slide(out, bb, rook, square, ROOK_DELTAS[i], occAll);
    });
}

void MoveGen::genKnightMovesFor(MoveList& out, Piece knight) const {
    forEachSetBit(bb[to_u(knight)], [&](uint8_t from) {
        for(size_t i = 0; i < KNIGHT_DELTAS.size(); ++i) {
            int8_t to = from + KNIGHT_DELTAS[i];
            if (!squareInBoard(to) || !knightHopValid(from, to)) continue;
            if (getOcc(false, isWhite(knight)) & (1ULL << to)) continue;
            pushQuietOrCapture(out, bb, from, to, knight, occAll);
        }
    });
}

void MoveGen::genBishopMovesFor(MoveList& out, Piece bishop) const {
    forEachSetBit(bb[to_u(bishop)], [&](uint8_t square) {
        for(size_t i = 0; i < BISHOP_DELTAS.size(); ++i)
            slide(out, bb, bishop, square, BISHOP_DELTAS[i], occAll);
    });
}

void MoveGen::genQueenMovesFor(MoveList& out, Piece queen) const {
    forEachSetBit(bb[to_u(queen)], [&](uint8_t square) {
        for(size_t i = 0; i < QUEEN_DELTAS.size(); ++i)
            slide(out, bb, queen, square, QUEEN_DELTAS[i], occAll);
    });
}

void MoveGen::genKingMovesFor(MoveList& out, Piece king) const {
    forEachSetBit(bb[to_u(king)], [&](uint8_t from) {
        for(size_t i = 0; i < KING_DELTAS.size(); ++i) {
            int8_t to = from + KING_DELTAS[i];
            if (!squareInBoard(to) || wrapped(from, to, KING_DELTAS[i])) continue;
            pushQuietOrCapture(out, bb, from, to, king, occAll);
        }
    });

    // Castling
    if (king == Piece::WK) {
        // King-side (e1 -> g1), rook at h1
        if ((castling & to_u(Castling::W_K)) && kingAt(true, e1)) {
            // empty between king and rook, not in/through check, rook exists
            if (!occupied(occAll, f1) && !occupied(occAll, g1) &&
                !isSquareAttacked(bb, e1, true, occAll) &&
                !isSquareAttacked(bb, f1, true, occAll) &&
                !isSquareAttacked(bb, g1, true, occAll) &&
                    isPieceAtSquare(bb, Piece::WR, h1)) {
                pushQuiet(out, e1, g1, king, /*isDPP=*/false, /*isCastle=*/true);
            }
        }
        // Queen-side (e1 -> c1), rook at a1
        if ((castling & to_u(Castling::W_Q)) && kingAt(true, e1)) {
            // empty between (d1,c1,b1), not in/through check on e1,d1,c1, rook exists
            if (!occupied(occAll, d1) && !occupied(occAll, c1) && !occupied(occAll, b1) &&
                !isSquareAttacked(bb, c1, true, occAll) &&
                !isSquareAttacked(bb, d1, true, occAll) &&
                !isSquareAttacked(bb, e1, true, occAll) &&
                    isPieceAtSquare(bb, Piece::WR, a1)) {
                pushQuiet(out, e1, c1, king, /*isDPP=*/false, /*isCastle=*/true);
            }
        }
    } else {
        // King-side (e8 -> g8), rook at h8
        if ((castling & to_u(Castling::B_K)) && kingAt(false, e8)) {
            if (!occupied(occAll, f8) && !occupied(occAll, g8) &&
                !isSquareAttacked(bb, e8, false, occAll) &&
                !isSquareAttacked(bb, f8, false, occAll) &&
                !isSquareAttacked(bb, g8, false, occAll) &&
                    isPieceAtSquare(bb, Piece::BR, h8)) {
                pushQuiet(out, e8, g8, king, /*isDPP=*/false, /*isCastle=*/true);
            }
        }
        // Queen-side (e8 -> c8), rook at a8
        if ((castling & to_u(Castling::B_Q)) && kingAt(false, e8)) {
            if (!occupied(occAll, d8) && !occupied(occAll, c8) && !occupied(occAll, b8) &&
                !isSquareAttacked(bb, c8, false, occAll) &&
                !isSquareAttacked(bb, d8, false, occAll) &&
                !isSquareAttacked(bb, e8, false, occAll) &&
                    isPieceAtSquare(bb, Piece::BR, a8)) {
                pushQuiet(out, e8, c8, king, /*isDPP=*/false, /*isCastle=*/true);
            }
        }
    }
}

void MoveGen::genSafeKingMoves(MoveList& out, uint8_t kingSq) const {
    Piece ourKing = getOurKing(whiteToMove);
    uint64_t kingMoves = getKingMoves(kingSq) & ~ourOcc();
    forEachSetBit(kingMoves, [&](uint8_t to) {
        uint64_t occKing = (occAll ^ (1ULL << kingSq)) | (1ULL << to);
        if(!isSquareAttacked(bb, to, whiteToMove, occKing))
        pushQuietOrCapture(out, bb, kingSq, to, getOurKing(whiteToMove), occAll);
    });
}

void MoveGen::genPieceCapturesToSquare(MoveList& out, uint8_t sq, Piece piece, Piece captured) const {
    uint64_t ourAttackingPieces = scanAttacks(bb, sq, piece, occAll);
    forEachSetBit(ourAttackingPieces, [&](uint8_t from) {
        pushCapture(out, from, sq, piece, captured);
    });
}

void MoveGen::genCapturesToSquare(MoveList& out, uint8_t attackSq) const {
    bool meWhite = whiteToMove;
    Piece captured = findPieceAt(attackSq, !meWhite);
    assert(captured != Piece::None); // must be an enemy piece
    Piece ourKing = getOurKing(whiteToMove);

    // Pawns
    Piece ourPawn = getOurPawn(whiteToMove);
    uint64_t ourAttackingPawns = scanAttacks(bb, attackSq, ourPawn, occAll);
    forEachSetBit(ourAttackingPawns, [&](uint8_t from) {
        if (pawnMoveFromPromotion(from, whiteToMove))
            pushPromo(out, from, attackSq, ourPawn, true, captured);
        else
            pushCapture(out, from, attackSq, ourPawn, captured);
    });

    // Rooks, Knights, Bishops, Queens
    genPieceCapturesToSquare(out, attackSq, getOurRook(whiteToMove), captured);
    genPieceCapturesToSquare(out, attackSq, getOurKnight(whiteToMove), captured);
    genPieceCapturesToSquare(out, attackSq, getOurBishop(whiteToMove), captured);
    genPieceCapturesToSquare(out, attackSq, getOurQueen(whiteToMove), captured);
}

void MoveGen::genPawnBlocksToMask(MoveList& out, uint64_t mask) const {
    Piece ourPawn = getOurPawn(whiteToMove);
    int8_t FWD = whiteToMove ? NUM_SQUARES_IN_ROW : -(int8_t)NUM_SQUARES_IN_ROW;
    uint8_t secondRank = whiteToMove ? SECOND_RANK : SEVENTH_RANK;

    forEachSetBit(bb[to_u(ourPawn)], [&](uint8_t from){
        // Single push
        uint8_t to1 = int8_t(from) + FWD;
        if (!occupied(occAll, to1) && (mask & (1ULL << to1))) {
            pushQuiet(out, from, to1, ourPawn);
        }
        // Double push
        if (rankOf(from) == secondRank) {
            uint8_t mid = uint8_t(int8_t(from) + FWD);
            uint8_t to2 = uint8_t(int8_t(from) + 2*FWD);
            if (!occupied(occAll, mid) &&
                !occupied(occAll, to2) &&
                (mask & (1ULL << to2))) {
                pushQuiet(out, from, to2, ourPawn, /*isDPP=*/true);
            }
        }
    });
}

void MoveGen::genPieceBlocksToMask(MoveList& out, uint64_t mask, Piece piece) const {
    forEachSetBit(mask, [&](uint8_t to) {
        uint64_t froms = scanAttacks(bb, to, piece, occAll);
        forEachSetBit(froms, [&](uint8_t from) {
            pushQuiet(out, from, to, piece);
        });
    });
}

void MoveGen::genQuietBlocks(MoveList& out, uint64_t mask) const {
    genPawnBlocksToMask(out, mask);
    genPieceBlocksToMask(out, mask, getOurRook(whiteToMove));
    genPieceBlocksToMask(out, mask, getOurKnight(whiteToMove));
    genPieceBlocksToMask(out, mask, getOurBishop(whiteToMove));
    genPieceBlocksToMask(out, mask, getOurQueen(whiteToMove));
}

void MoveGen::genEPBlock(MoveList& out, uint64_t mask) const {
    if (epSquare == NUM_SQUARES) return;
    Piece ourPawn = getOurPawn(whiteToMove);
    uint8_t epSq = epSquare; // landing square of pawn
    uint64_t ourEPpawns = scanAttacks(bb, epSq, ourPawn, occAll);
    forEachSetBit(ourEPpawns, [&](uint8_t from) {
        if ((1ULL << epSq) & mask) pushCapture(out, from, epSq, ourPawn, getOtherPiece(ourPawn), true);
    });
}

void MoveGen::genPseudoMoves(MoveList& out) const {
    using enum Piece;
    if (whiteToMove) {
        genPawnMovesFor(out, WP);
        genRookMovesFor(out, WR);
        genKnightMovesFor(out, WN);
        genBishopMovesFor(out, WB);
        genQueenMovesFor(out, WQ);
        genKingMovesFor(out, WK);
    } else {
        genPawnMovesFor(out, BP);
        genRookMovesFor(out, BR);
        genKnightMovesFor(out, BN);
        genBishopMovesFor(out, BB);
        genQueenMovesFor(out, BQ);
        genKingMovesFor(out, BK);
    }
}

void MoveGen::genEvasions(MoveList& out, uint8_t kingSq) const {
    uint64_t checkers = attackersTo(bb, kingSq, whiteToMove, occAll);
    assert(checkers != 0); // King must be in check
    genSafeKingMoves(out, kingSq); // King moves
    if (std::popcount(checkers) >= 2) return; // Double check can only be escaped by king moves.

    // Single checks (only one piece):
    // Options are either capture the checker or block if its a sliding piece (rook, bishop, queen)
    uint8_t attackSq = bitscanForward(checkers); // get square of attacker
    bool sliding = isRookBishopQueen(enemyPieceAt(bb, attackSq, whiteToMove));
    uint64_t blockSq = sliding ? rayBetween(kingSq, attackSq) : 0ULL;

    // Capture the checker
    genCapturesToSquare(out, attackSq);

    // Block the checker
    if (blockSq) genQuietBlocks(out, blockSq);

    // En passant block
    if (epSquare != NUM_SQUARES) genEPBlock(out, blockSq);
}

MoveGen::MoveGen(Bitboards& bb, bool& whiteToMove, uint64_t& occWhite, uint64_t& occBlack, uint64_t& occAll, uint8_t& castling, uint8_t& epSquare) :
    bb(bb),
    whiteToMove(whiteToMove),
    occWhite(occWhite),
    occBlack(occBlack),
    occAll(occAll),
    castling(castling),
    epSquare(epSquare)
{}