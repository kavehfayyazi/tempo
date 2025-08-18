//
// Created by Kaveh Fayyazi on 8/9/25.
//

#ifndef TEMPO_BOARD_H
#define TEMPO_BOARD_H

#include "movegen.h"
#include "move.h"
#include "zobrist.h"
#include <cstdint>
#include <array>
#include <vector>
#include <stack>

using Bitboards = std::array<uint64_t, 12>;
using MoveList = std::vector<uint32_t>;

// represents board state for pushing onto move stack
struct State {
    uint64_t zobrist;
    uint8_t  castling;
    uint8_t   epSquare;
    uint16_t halfmoveClock;
    uint8_t  captured; // piece code or 0xF for None
};

class Board {
public:
    // piece bb: WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK
    Bitboards bb;

    // occupancy bb
    void calcOcc();
    uint64_t occWhite, occBlack, occAll;

    // helper
    inline void removeCastlingFlag(uint8_t flag);

    // state
    bool whiteToMove;
    bool hasCastled;
    uint8_t castling; // bitmask: 1 for WQ, 2 for WK, 3 for BQ, 4 for BK
    uint8_t epSquare; // En passant square
    uint8_t halfMoveClock;
    uint8_t fullMoveTotal;

    // hashing
    Zobrist zobrist;
    uint64_t key;

    // move generator
    MoveGen movegen;

    // keep track of move
    std::stack<State> gameRecord;

public:
    void move(uint32_t move);
    void undoMove(uint32_t move);
    MoveList genLegalMoves();
    Board();
};

#endif //TEMPO_BOARD_H
