//
// Created by Kaveh Fayyazi on 8/18/25.
//

#ifndef TEMPO_PERFT_H
#define TEMPO_PERFT_H

#include "Board.h"

// Used to verify the total number of legal positions (nodes) reachable
// from a starting position to a specified depth (debugging/testing)
uint64_t Perft(Board& board, uint8_t depth) {
    if (depth == 0) return 1;

    MoveList moves;
    board.genLegalMoves(moves);
    uint64_t nMoves = moves.size();
    uint64_t nodes = 0;

    if (depth == 1) return nMoves;
    for (size_t i = 0; i < nMoves; ++i) {
        board.move(moves[i]);
        nodes += Perft(board, depth - 1);
        board.undoMove(moves[i]);
    }
    return nodes;
}

#endif //TEMPO_PERFT_H
