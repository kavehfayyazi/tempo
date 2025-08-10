//
// Created by Kaveh Fayyazi on 8/9/25.
//

#include "board.h"

Board::Board() : bitboards{}, whiteToMove(true), castling(uint8_t(W_K | W_Q | B_K | B_Q)), epSquare(NO_SQUARES), halfMoveClock(0), fullMoveTotal(1){
    constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;
    constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;

    // White pieces
    bitboards[WP] = RANK_2;
    bitboards[WR] = 0x0000000000000081ULL; // a1,h1
    bitboards[WN] = 0x0000000000000042ULL; // b1,g1
    bitboards[WB] = 0x0000000000000024ULL; // c1,f1
    bitboards[WQ] = 0x0000000000000008ULL; // d1
    bitboards[WK] = 0x0000000000000010ULL; // e1

    // Black pieces
    bitboards[BP] = RANK_7;
    bitboards[BR] = 0x8100000000000000ULL; // a8,h8
    bitboards[BN] = 0x4200000000000000ULL; // b8,g8
    bitboards[BB] = 0x2400000000000000ULL; // c8,f8
    bitboards[BQ] = 0x0800000000000000ULL; // d8
    bitboards[BK] = 0x1000000000000000ULL; // e8

    // Occupancy bitboards
    occWhite = bitboards[WP] | bitboards[WR] | bitboards[WN] | bitboards[WB] | bitboards[WQ] | bitboards[WK];
    occBlack = bitboards[BP] | bitboards[BR] | bitboards[BN] | bitboards[BB] | bitboards[BQ] | bitboards[BK];
    occAll = occWhite | occBlack;

    // Hashing
    hash = computeZobrist();
}