//
// Created by Kaveh Fayyazi on 8/10/25.
//

#ifndef TEMPO_TYPES_H
#define TEMPO_TYPES_H

#include "typeHelpers.h"
#include <cstdint>

// types.h (example)
enum class Piece : uint8_t { WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, PIECE_N, None=0xFF};
enum class Promo : uint8_t { None=0, R, N, B, Q}; // Promoted
enum class Castling : uint8_t { None=0, W_K = 1<<0, W_Q = 1<<1, B_K = 1<<2, B_Q = 1<<3};

inline constexpr uint8_t CASTLING_N = 4;
inline constexpr uint64_t NUM_SQUARES = 64;
inline constexpr uint64_t NUM_SQUARES_IN_ROW = 8;

#endif //TEMPO_TYPES_H
