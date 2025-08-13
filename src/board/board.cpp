//
// Created by Kaveh Fayyazi on 8/9/25.
//

#include "board.h"
#include "types.h"


using enum Castling;

Board::Board() :
    bitboards{},
    whiteToMove(true),
    castling(uint8_t(to_u(W_K) | to_u(W_Q) | to_u(B_K) | to_u(B_Q))),
    epSquare(NUM_SQUARES),
    halfMoveClock(0),
    fullMoveTotal(1),
    hash(0),
    zobrist(Zobrist())
{
    constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;
    constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;

    {
        using enum Piece;
        // White pieces
        bitboards[to_u(WP)] = RANK_2;
        bitboards[to_u(WR)] = 0x0000000000000081ULL; // a1,h1
        bitboards[to_u(WN)] = 0x0000000000000042ULL; // b1,g1
        bitboards[to_u(WB)] = 0x0000000000000024ULL; // c1,f1
        bitboards[to_u(WQ)] = 0x0000000000000008ULL; // d1
        bitboards[to_u(WK)] = 0x0000000000000010ULL; // e1

        // Black pieces
        bitboards[to_u(BP)] = RANK_7;
        bitboards[to_u(BR)] = 0x8100000000000000ULL; // a8,h8
        bitboards[to_u(BN)] = 0x4200000000000000ULL; // b8,g8
        bitboards[to_u(BB)] = 0x2400000000000000ULL; // c8,f8
        bitboards[to_u(BQ)] = 0x0800000000000000ULL; // d8
        bitboards[to_u(BK)] = 0x1000000000000000ULL; // e8

        // Occupancy bitboards
        occWhite = bitboards[to_u(WP)] | bitboards[to_u(WR)] | bitboards[to_u(WN)] | bitboards[to_u(WB)] |
                   bitboards[to_u(WQ)] | bitboards[to_u(WK)];
        occBlack = bitboards[to_u(BP)] | bitboards[to_u(BR)] | bitboards[to_u(BN)] | bitboards[to_u(BB)] |
                   bitboards[to_u(BQ)] | bitboards[to_u(BK)];
        occAll = occWhite | occBlack;
    }
}