//
// Created by Kaveh Fayyazi on 8/9/25.
//

#include "attacks.h"
#include "board.h"
#include "utils.h"
#include "movegen.h"
#include "types.h"
#include <iostream>
#include <queue>

// file and rank is [0,7]

using enum Piece;

void Board::calcOcc() {
    occWhite = bb[to_u(WP)] | bb[to_u(WR)] | bb[to_u(WN)] | bb[to_u(WB)] |
               bb[to_u(WQ)] | bb[to_u(WK)];
    occBlack = bb[to_u(BP)] | bb[to_u(BR)] | bb[to_u(BN)] | bb[to_u(BB)] |
               bb[to_u(BQ)] | bb[to_u(BK)];
    occAll = occWhite | occBlack;
}

inline void Board::removeCastlingFlag (uint8_t flag) {
    if (!(castling & flag)) return;
    uint8_t idx =
        (flag == W_K_FLAG) ? 0 :
        (flag == W_Q_FLAG) ? 1 :
        (flag == B_K_FLAG) ? 2 : 3; // flag == B_Q_FLAG
    key ^= zobrist.castling[idx];
    castling &= ~flag;
}

void Board::move(uint32_t move) {
    const auto from = Move::from(move);
    const auto to = Move::to(move);
    const auto movedCode = Move::movedCode(move);
    const auto isCapture = Move::isCapture(move);
    const auto isCastle = Move::isCastle(move);
    const auto isDPP = Move::isDPP(move);
    const auto isEP = Move::isEP(move);
    const auto promoCode = Move::promo(move);
    const auto capturedCode = Move::capturedCode(move);

    State st{key, castling, epSquare, halfMoveClock, capturedCode};
    gameRecord.push(st);

    // 1) If En Passant square is set, clear it
    if (epSquare != NUM_SQUARES) {
        key ^= zobrist.epFile[fileOf(epSquare)];  // assuming ep hash is file-based
        epSquare = NUM_SQUARES;
    }

    // 2) Move current piece
    key ^= zobrist.pieces[movedCode][from];
    key ^= zobrist.pieces[movedCode][to];
    bb[movedCode] ^= (1ULL << from);
    bb[movedCode] ^= (1ULL << to);

    // 3) Capture
    if (isCapture) {
        uint8_t captureSq = to;
        if (isEP)
            captureSq = (movedCode == to_u(WP))
                    ? (to - NUM_SQUARES_IN_ROW)
                    : (to + NUM_SQUARES_IN_ROW);
        key ^= zobrist.pieces[capturedCode][captureSq];
        bb[capturedCode] ^= (1ULL << captureSq);
    }

    // 4) Castling, move the rook
    if (isCastle) {
        const uint8_t r = rankOf(from);
        const uint8_t fFrom = fileOf(from);
        const uint8_t fTo = fileOf(to);
        bool kingSide = (fTo < fFrom);

        const uint8_t rookFromSq = sq(kingSide ? (fTo - 1) : (fTo + 2), r);
        const uint8_t rookToSq   = sq(kingSide ? (fTo + 1) : (fTo - 1), r);
        const auto rookCode = isWhite(static_cast<Piece>(movedCode)) ? to_u(WR) : to_u(BR);

        key ^= zobrist.pieces[rookCode][rookFromSq];
        key ^= zobrist.pieces[rookCode][rookToSq];
        bb[rookCode] ^= (1ULL << rookFromSq);
        bb[rookCode] ^= (1ULL << rookToSq);
    }

    // 5) Double pawn push, set en passant square
    if (isDPP) {
        epSquare = (movedCode == to_u(WP))
                ? (to - NUM_SQUARES_IN_ROW)
                : (to + NUM_SQUARES_IN_ROW);
        key ^= zobrist.epFile[fileOf(epSquare)];
    }

    // 6) Promotion
    if (promoCode != 0x7) {
        key ^= zobrist.pieces[movedCode][to];
        key ^= zobrist.pieces[promoCode][to];
        bb[movedCode] ^= (1ULL << to);
        bb[promoCode] ^= (1ULL << to);
    }

    // 7) Castling flag logic

    // If a king moved
    if (movedCode == to_u(WK)) { removeCastlingFlag(W_K_FLAG); removeCastlingFlag(W_Q_FLAG); }
    if (movedCode == to_u(BK)) { removeCastlingFlag(B_K_FLAG); removeCastlingFlag(B_Q_FLAG); }

    // If a rook moved from its original square
    if (movedCode == to_u(WR)) {
        if (from == h1) removeCastlingFlag(W_K_FLAG);
        if (from == a1) removeCastlingFlag(W_Q_FLAG);
    }
    if (movedCode == to_u(BR)) {
        if (from == h8) removeCastlingFlag(B_K_FLAG);
        if (from == a8) removeCastlingFlag(B_Q_FLAG);
    }

    // If a rook was captured on its original square
    if (isCapture) {
        if (capturedCode == to_u(WR)) {
            if (to == h1) removeCastlingFlag(W_K_FLAG);
            if (to == a1) removeCastlingFlag(W_Q_FLAG);
        } else if (capturedCode == to_u(BR)) {
            if (to == h8) removeCastlingFlag(B_K_FLAG);
            if (to == a8) removeCastlingFlag(B_Q_FLAG);
        }
    }

    // 8) Side to move
    whiteToMove = !whiteToMove;
    key ^= zobrist.blackToMove;

    // 9) Update occupancies
    calcOcc();
}

void Board::undoMove(uint32_t move) {
    const auto from = Move::from(move);
    const auto to = Move::to(move);
    const auto movedCode = Move::movedCode(move);
    const auto isCapture = Move::isCapture(move);
    const auto isCastle = Move::isCastle(move);
    const auto isDPP = Move::isDPP(move);
    const auto isEP = Move::isEP(move);
    const auto promoCode = Move::promo(move);
    const auto capturedCode = Move::capturedCode(move);

    // flip turn back
    whiteToMove = !whiteToMove;

    // get hashing, castling, en passant square, and halfMoveClock from state
    State st = gameRecord.top();
    gameRecord.pop();
    key = st.zobrist;
    castling = st.castling;
    epSquare = st.epSquare;
    halfMoveClock = st.halfmoveClock;

    // Undo special cases
    // 1) Promotion
    if (promoCode != 0x7) { // if promotion existed
        bb[promoCode] ^= (1ULL << to);
        bb[movedCode] ^= (1ULL << to);
    }

    // 2) Undo capture (including EP): restore captured piece on its square
    if (isCapture) {
        uint8_t captureSq = to;
        if (isEP) {
            captureSq = (movedCode == to_u(WP))
                        ? (to - NUM_SQUARES_IN_ROW)
                        : (to + NUM_SQUARES_IN_ROW);
        }
        bb[capturedCode] ^= (1ULL << captureSq);
    }

    // 3) Undo rook move from castling
    if (isCastle) {
        const uint8_t r = rankOf(from);
        const uint8_t fFrom = fileOf(from);
        const uint8_t fTo   = fileOf(to);
        bool kingSide = (fTo < fFrom);

        const uint8_t rookFromSq = sq(kingSide ? (fTo - 1) : (fTo + 2), r);
        const uint8_t rookToSq   = sq(kingSide ? (fTo + 1) : (fTo - 1), r);
        const auto rookCode = (movedCode <= to_u(WK)) ? to_u(WR) : to_u(BR);
        bb[rookCode] ^= (1ULL << rookFromSq);
        bb[rookCode] ^= (1ULL << rookToSq);
    }

    // 4) Undo the piece move itself
    bb[movedCode] ^= (1ULL << to);
    bb[movedCode] ^= (1ULL << from);

    // 5) Recompute occupancies
    calcOcc();
}

MoveList Board::genLegalMoves() {
    MoveList moves;
    moves.reserve(218); // Maximum number of pseudolegal moves for a single turn in chess

    uint8_t kingSq = kingSquare(bb, whiteToMove);
    if (isSquareAttacked(bb, kingSq, whiteToMove, occAll))
        movegen.genEvasions(moves, kingSq);
    else movegen.genPseudoMoves(moves);

    MoveList out;
    out.reserve(moves.size());

    for (auto m : moves) {
        move(m);
        const bool prevUs = !whiteToMove; // Side to move was changed
        const uint8_t ksqAfter = kingSquare(bb, prevUs);
        if (!isSquareAttacked(bb, ksqAfter, prevUs, occAll))
            out.push_back(m);
        undoMove(m);
    }
    return out;
}

Board::Board() :
        bb{},
        whiteToMove(true),
        hasCastled(false),
        castling(uint8_t(to_u(Castling::W_K) | to_u(Castling::W_Q) | to_u(Castling::B_K) | to_u(Castling::B_Q))),
        epSquare(NUM_SQUARES),
        halfMoveClock(0),
        fullMoveTotal(1),
        zobrist(Zobrist()),
        key(zobrist.getKey()),
        movegen(MoveGen(bb, whiteToMove, occWhite, occBlack, occAll, castling, epSquare))
{
    // White pieces
    bb[to_u(WP)] = RANK_2;
    bb[to_u(WR)] = 0x0000000000000081ULL; // a1,h1
    bb[to_u(WN)] = 0x0000000000000042ULL; // b1,g1
    bb[to_u(WB)] = 0x0000000000000024ULL; // c1,f1
    bb[to_u(WQ)] = 0x0000000000000008ULL; // d1
    bb[to_u(WK)] = 0x0000000000000010ULL; // e1

    // Black pieces
    bb[to_u(BP)] = RANK_7;
    bb[to_u(BR)] = 0x8100000000000000ULL; // a8,h8
    bb[to_u(BN)] = 0x4200000000000000ULL; // b8,g8
    bb[to_u(BB)] = 0x2400000000000000ULL; // c8,f8
    bb[to_u(BQ)] = 0x0800000000000000ULL; // d8
    bb[to_u(BK)] = 0x1000000000000000ULL; // e8

    // Occupancy bb
    occWhite = bb[to_u(WP)] | bb[to_u(WR)] | bb[to_u(WN)] | bb[to_u(WB)] |
               bb[to_u(WQ)] | bb[to_u(WK)];
    occBlack = bb[to_u(BP)] | bb[to_u(BR)] | bb[to_u(BN)] | bb[to_u(BB)] |
               bb[to_u(BQ)] | bb[to_u(BK)];
    occAll = occWhite | occBlack;
}