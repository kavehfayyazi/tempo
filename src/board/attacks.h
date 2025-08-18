//
// Created by Kaveh Fayyazi on 8/13/25.
//

#ifndef TEMPO_CHECK_H
#define TEMPO_CHECK_H

#include "types.h"
#include "utils.h"
#include <vector>
#include <span>

using enum Piece;
using Bitboards = std::array<uint64_t, 12>;

void getDeltasAndSteps(Piece piece, const int8_t*& DELTAS, uint8_t& NUM_DELTAS, uint8_t& maxSteps);

// Used for detecting rays of piece attacks on from
bool scanRays(const Bitboards& bb, int8_t from, Piece piece, uint64_t occAll);

// Used for returning occupancy of piece attacks on sq
uint64_t scanAttacks(const Bitboards& bb, int8_t sq, Piece piece, uint64_t occAll);

bool isSquareAttacked(Bitboards& bb, uint8_t sq, bool meWhite, uint64_t occAll);

uint64_t attackersTo(Bitboards& bb, uint8_t kingSq, bool meWhite, uint64_t occAll);

#endif //TEMPO_CHECK_H
