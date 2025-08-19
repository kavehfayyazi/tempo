//
// Created by Kaveh Fayyazi on 8/18/25.
//

#include "attacks.h"

using enum Piece;
using Bitboards = std::array<uint64_t, 12>;

void getDeltasAndSteps(Piece piece, const int8_t*& DELTAS, uint8_t& NUM_DELTAS, uint8_t& maxSteps) {
    switch (piece) {
        case Piece::WP:
            DELTAS = WHITE_PAWN_DELTAS.data();
            NUM_DELTAS = NUM_WHITE_PAWN_DELTAS;
            maxSteps = 1;
            break;
        case Piece::BP:
            DELTAS = BLACK_PAWN_DELTAS.data();
            NUM_DELTAS = NUM_BLACK_PAWN_DELTAS;
            maxSteps = 1;
            break;
        case Piece::WN: case Piece::BN:
            DELTAS = KNIGHT_DELTAS.data();
            NUM_DELTAS = NUM_KNIGHT_DELTAS;
            maxSteps = 1;
            break;
        case Piece::WR: case Piece::BR:
            DELTAS = ROOK_DELTAS.data();
            NUM_DELTAS = NUM_ROOK_DELTAS;
            maxSteps = 8;
            break;
        case Piece::WB: case Piece::BB:
            DELTAS = BISHOP_DELTAS.data();
            NUM_DELTAS = NUM_BISHOP_DELTAS;
            maxSteps = 8;
            break;
        case Piece::WQ: case Piece::BQ:
            DELTAS = QUEEN_DELTAS.data();
            NUM_DELTAS = NUM_QUEEN_DELTAS;
            maxSteps = 8;
            break;
        case Piece::WK: case Piece::BK:
            DELTAS = KING_DELTAS.data();
            NUM_DELTAS = NUM_KING_DELTAS;
            maxSteps = 1;
            break;
        default: break;
    }
}

// Used for detecting rays of piece attacks on from
bool scanRays(const Bitboards& bb, int8_t from, Piece piece, uint64_t occAll) {
    const int8_t* DELTAS = nullptr; uint8_t NUM_DELTAS; uint8_t maxSteps = 0;
    getDeltasAndSteps(piece, DELTAS, NUM_DELTAS, maxSteps);

    for (size_t i = 0; i < NUM_DELTAS; ++i) {
        int8_t next = from; int8_t dsq = DELTAS[i];
        if (isPawn(piece)) {
            next -= dsq; // reverse direction
            if (!squareInBoard(next) || !diagHopValid(from, next)) continue;
            if (bb[to_u(piece)] & (1ULL << next)) return true;
            continue;
        }
        if (isKnight(piece)) {
            next += dsq;
            if (!squareInBoard(next) || !knightHopValid(from, next)) continue;
            if (bb[to_u(piece)] & (1ULL << next)) return true;
            continue;
        }

        int8_t prev = from;
        for (size_t step = 0; step < maxSteps; ++step) {
            next += dsq;
            if (!squareInBoard(next) || wrapped(prev, next, dsq)) break; // out of bounds
            if (bb[to_u(piece)] & (1ULL << next)) // piece occupies this square
                return true;
            if (occupied(occAll, next)) // blocked by any piece
                break;
            prev += dsq;
        }
    }
    return false;
}

// Used for returning occupancy of piece attacks on sq
uint64_t scanAttacks(const Bitboards& bb, int8_t sq, Piece piece, uint64_t occAll) {
    const int8_t* DELTAS = nullptr; uint8_t NUM_DELTAS; uint8_t maxSteps = 0;
    getDeltasAndSteps(piece, DELTAS, NUM_DELTAS, maxSteps);

    uint64_t attacksByPiece = 0;

    // Knights
    if (isKnight(piece)) {
        for (size_t i = 0; i < NUM_DELTAS; ++i) {
            int8_t to = sq + DELTAS[i];
            if (squareInBoard(to) && knightHopValid(sq, to) &&
                (bb[to_u(piece)] & (1ULL << to)))
                attacksByPiece |= (1ULL << to);
            }
        return attacksByPiece;
    }
    // Pawns, reverse direction (same in scanRays)
    if (isPawn(piece)) {
        for (size_t i = 0; i < NUM_DELTAS; ++i) {
            int8_t to = sq - DELTAS[i];
            if (squareInBoard(to) && diagHopValid(sq, to) &&
                (bb[to_u(piece)] & (1ULL << to)))
                attacksByPiece |= (1ULL << to);
            }
        return attacksByPiece;
    }

    for (size_t i = 0; i < NUM_DELTAS; ++i) {
        int8_t dx = dxStepFromDelta(DELTAS[i]);
        int8_t prevSq = sq;
        int8_t nextSq = sq;
        for (size_t step = 0; step < maxSteps; ++step) {
            nextSq += DELTAS[i];
            if (!squareInBoard(nextSq) || wrapped(prevSq, nextSq, DELTAS[i])) break; // out of bounds
            if (bb[to_u(piece)] & (1ULL << nextSq)) {// piece occupies this square
                attacksByPiece |= (1ULL << nextSq);
                break;
            }
            if (occupied(occAll, nextSq)) // blocked by any piece
                break;
            prevSq += DELTAS[i];
        }
    }
    return attacksByPiece;
}

bool isSquareAttacked(Bitboards& bb, uint8_t sq, bool meWhite, uint64_t occAll) {
    return attackersTo(bb, sq, meWhite, occAll) != 0;
}

uint64_t attackersTo(Bitboards& bb, uint8_t kingSq, bool meWhite, uint64_t occAll) {
    return scanAttacks(bb, kingSq, getEnemyPawn(meWhite),   occAll)
         | scanAttacks(bb, kingSq, getEnemyRook(meWhite),   occAll)
         | scanAttacks(bb, kingSq, getEnemyKnight(meWhite), occAll)
         | scanAttacks(bb, kingSq, getEnemyBishop(meWhite), occAll)
         | scanAttacks(bb, kingSq, getEnemyQueen(meWhite),  occAll)
         | scanAttacks(bb, kingSq, getEnemyKing(meWhite),   occAll);
}
