//
// Created by Kaveh Fayyazi on 8/10/25.
//

#ifndef TEMPO_TYPES_H
#define TEMPO_TYPES_H

#include <array>
#include <cstdint>

// ---------- Enums ----------
template <class E>
constexpr auto to_u(E e) noexcept -> std::underlying_type_t<E> {
    static_assert(std::is_enum_v<E>, "to_u requires an enum type");
    return static_cast<std::underlying_type_t<E>>(e);}
enum class Piece    : uint8_t { WP, WR, WN, WB, WQ, WK, BP, BR, BN, BB, BQ, BK, PIECE_N, None=0xF};
enum class Promo    : uint8_t { R, N, B, Q, PROMO_N, None=0xF};
enum class Castling : uint8_t { None=0, W_K = 1<<0, W_Q = 1<<1, B_K = 1<<2, B_Q = 1<<3};

// ---------- Piece Groups ----------
inline constexpr std::array<Piece, 6> WHITE_PIECES { Piece::WP, Piece::WR, Piece::WN, Piece::WB, Piece::WQ, Piece::WK };
inline constexpr std::array<Piece, 6> BLACK_PIECES { Piece::BP, Piece::BR, Piece::BN, Piece::BB, Piece::BQ, Piece::BK };

inline constexpr std::array<Piece, 2> PAWNS   { Piece::WP, Piece::BP };
inline constexpr std::array<Piece, 2> ROOKS   { Piece::WR, Piece::BR };
inline constexpr std::array<Piece, 2> KNIGHTS { Piece::WN, Piece::BN };
inline constexpr std::array<Piece, 2> BISHOPS { Piece::WB, Piece::BB };
inline constexpr std::array<Piece, 2> QUEENS  { Piece::WQ, Piece::BQ };
inline constexpr std::array<Piece, 2> KINGS   { Piece::WK, Piece::BK };

// ---------- Piece Codes ----------
inline constexpr uint8_t WP_CODE = 0;
inline constexpr uint8_t WR_CODE = 1;
inline constexpr uint8_t WN_CODE = 2;
inline constexpr uint8_t WB_CODE = 3;
inline constexpr uint8_t WQ_CODE = 4;
inline constexpr uint8_t WK_CODE = 5;
inline constexpr uint8_t BP_CODE = 6;
inline constexpr uint8_t BR_CODE = 7;
inline constexpr uint8_t BN_CODE = 8;
inline constexpr uint8_t BB_CODE = 9;
inline constexpr uint8_t BQ_CODE = 10;
inline constexpr uint8_t BK_CODE = 11;

// ---------- Castling Flags ----------
inline constexpr uint8_t W_K_FLAG = to_u(Castling::W_K);
inline constexpr uint8_t W_Q_FLAG = to_u(Castling::W_Q);
inline constexpr uint8_t B_K_FLAG = to_u(Castling::B_K);
inline constexpr uint8_t B_Q_FLAG = to_u(Castling::B_Q);

// ---------- Board Constants ----------
inline constexpr uint64_t RANK_1 = 0x00000000000000FFULL;
inline constexpr uint64_t RANK_2 = 0x000000000000FF00ULL;
inline constexpr uint64_t RANK_6 = 0x0000FF0000000000ULL;
inline constexpr uint64_t RANK_7 = 0x00FF000000000000ULL;
inline constexpr uint64_t RANK_8 = 0xFF00000000000000ULL;

inline constexpr uint8_t EIGHTH_RANK = 7;
inline constexpr uint8_t SEVENTH_RANK = 6;
inline constexpr uint8_t SIXTH_RANK = 5;
inline constexpr uint8_t FIFTH_RANK = 4;
inline constexpr uint8_t FOURTH_RANK = 3;
inline constexpr uint8_t THIRD_RANK = 2;
inline constexpr uint8_t SECOND_RANK = 1;
inline constexpr uint8_t FIRST_RANK = 0;
inline constexpr uint8_t A_FILE = 7;
inline constexpr uint8_t H_FILE = 0;

inline constexpr uint8_t CASTLING_N = 4;
inline constexpr uint8_t NUM_SQUARES = 64;
inline constexpr uint8_t NUM_SQUARES_IN_ROW = 8;

// ---------- Move Generation Deltas ----------
inline constexpr std::array<int8_t, 2> WHITE_PAWN_DELTAS = { NUM_SQUARES_IN_ROW +1, NUM_SQUARES_IN_ROW -1 };
inline constexpr std::array<int8_t, 2> BLACK_PAWN_DELTAS = { -NUM_SQUARES_IN_ROW +1, -NUM_SQUARES_IN_ROW -1 };
inline constexpr std::array<int8_t, 4> ROOK_DELTAS = { +NUM_SQUARES_IN_ROW, -1, -NUM_SQUARES_IN_ROW, +1 };
inline constexpr std::array<int8_t, 8> KNIGHT_DELTAS = { +17, +10, -6, -15, -17, -10, +6, +15 };
inline constexpr std::array<int8_t, 4> BISHOP_DELTAS = { +9, +7, -9, -7 };
inline constexpr std::array<int8_t, 8> QUEEN_DELTAS = { +NUM_SQUARES_IN_ROW, -1, -NUM_SQUARES_IN_ROW, +1, +9, +7, -9, -7 };
inline constexpr std::array<int8_t, 8> KING_DELTAS = { +NUM_SQUARES_IN_ROW, -1, -NUM_SQUARES_IN_ROW, +1, +9, +7, -9, -7 };

// ---------- Delta Sizes ----------
inline constexpr uint8_t NUM_WHITE_PAWN_DELTAS = WHITE_PAWN_DELTAS.size();
inline constexpr uint8_t NUM_BLACK_PAWN_DELTAS = BLACK_PAWN_DELTAS.size();
inline constexpr uint8_t NUM_ROOK_DELTAS = ROOK_DELTAS.size();
inline constexpr uint8_t NUM_KNIGHT_DELTAS = KNIGHT_DELTAS.size();
inline constexpr uint8_t NUM_BISHOP_DELTAS = BISHOP_DELTAS.size();
inline constexpr uint8_t NUM_QUEEN_DELTAS = QUEEN_DELTAS.size();
inline constexpr uint8_t NUM_KING_DELTAS = KING_DELTAS.size();

#endif //TEMPO_TYPES_H
