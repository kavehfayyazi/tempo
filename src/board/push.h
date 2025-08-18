//
// Created by Kaveh Fayyazi on 8/13/25.
//

#ifndef TEMPO_PUSH_H
#define TEMPO_PUSH_H

#include "move.h"
#include "types.h"
#include "utils.h"
#include <vector>

using Bitboards = std::array<uint64_t, 12>;

inline void pushQuiet (std::vector<uint32_t> &out, uint8_t from, uint8_t to, Piece moved, bool isDPP=false, bool isCastle=false) {
    out.emplace_back(Move::make(from, to, moved, false, isCastle, isDPP, false));
}

inline void pushCapture (std::vector<uint32_t> &out, uint8_t from, uint8_t to, Piece moved, Piece captured) {
    out.emplace_back(Move::make(from, to, moved, true, false, false, false, captured));
}

// capture = true
bool pushQuietOrCapture (std::vector<uint32_t>& out, Bitboards& bb, uint8_t from, uint8_t to, Piece moved, uint64_t occAll, bool isRook=false) {
    if (!occupied(occAll, to)) { // space empty
        pushQuiet(out, from, to, moved);
        return false;
    }
    Piece enemy = enemyPieceAt(bb, to, isWhite(moved));
    if (enemy != Piece::None) // enemy piece occupies the square
        pushCapture(out, from, to, moved, enemy);
    else return false; // friendly piece occupies the square
    return true;
}

// pawn push promotion (includes captures)
inline void pushPromo (std::vector<uint32_t> &out, uint8_t from, uint8_t to, Piece moved, bool isCapture, Piece captured=Piece::None) {
    for (size_t i = 0; i < to_u(Promo::PROMO_N); ++i)
        out.emplace_back(Move::make(from, to, moved, isCapture, false, false, false, captured, static_cast<Promo>(i)));
}

void slide (std::vector<uint32_t> &out, Bitboards& bb, Piece piece, uint8_t from, int8_t delta, uint64_t occAll) {
    int8_t prev = from, next = from;
    while (true) {
        next += delta;
        if (!squareInBoard(next) || wrapped(prev, next, delta)) break; // out of bounds

        if (!occupied(occAll, next)) { // empty next space
            pushQuiet(out, from, next, piece);
            prev = next;
            continue;
        }

        Piece enemy = enemyPieceAt(bb, next, isWhite(piece));
        if (enemy != Piece::None) // enemy piece occupies the square
            pushCapture(out, from, next, piece, enemy);
        break;
    }
}

#endif //TEMPO_PUSH_H
