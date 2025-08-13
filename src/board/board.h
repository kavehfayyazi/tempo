//
// Created by Kaveh Fayyazi on 8/9/25.
//

#ifndef TEMPO_BOARD_H
#define TEMPO_BOARD_H

#include "zobrist.h"
#include <cstdint>

// represents board state for pushing onto move stack
struct State {
    uint64_t zobrist;
    uint8_t  castling;
    int8_t   epSquare;
    uint16_t halfmoveClock;
    uint8_t  captured; // piece code or EMPTY
};

class Board {
public:
    // piece bitboards: WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK
    uint64_t bitboards[12];

    // occupancy bitboards
    uint64_t occWhite{}, occBlack{}, occAll;

    // state
    bool whiteToMove;
    uint8_t castling{}; // bitmask: 1 for WQ, 2 for WK, 3 for BQ, 4 for BK
    uint64_t epSquare; // En passant square
    uint64_t halfMoveClock;
    uint64_t fullMoveTotal;

    // hashing
    Zobrist zobrist;
    uint64_t hash;

public:
    Board();
};

#endif //TEMPO_BOARD_H
