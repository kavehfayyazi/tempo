//
// Created by Kaveh Fayyazi on 8/14/25.
//

#ifndef TEMPO_UTILS_H
#define TEMPO_UTILS_H

#include "types.h"
#include "utils.h"
#include <random>
#include <type_traits>
#include <stdexcept>

static uint64_t random_u64() {
    static std::mt19937_64 rng(std::random_device{}());
    return rng();
}


// delta ∈ {+1,-1,+8,-8,+9,-9,+7,-7}
inline constexpr bool stepFromDelta(int8_t delta, int8_t& dx, int8_t& dy) {
    switch (delta) {
        case +1: dx = +1; dy = 0;  return true;
        case -1: dx = -1; dy = 0;  return true;
        case +8: dx = 0;  dy = +1; return true;
        case -8: dx = 0;  dy = -1; return true;
        case +9: dx = +1; dy = +1; return true;
        case -9: dx = -1; dy = -1; return true;
        case +7: dx = -1; dy = +1; return true;
        case -7: dx = +1; dy = -1; return true;
        default: dx = 0;  dy = 0;  return false; // invalid
    }
}

// delta ∈ {+1,-1,+8,-8,+9,-9,+7,-7}
inline constexpr int8_t dxStepFromDelta(int8_t delta) {
    switch (delta) {
        case +1: return +1;
        case -1: return -1;
        case +8: return 0;
        case -8: return 0;
        case +9: return +1;
        case -9: return -1;
        case +7: return -1;
        case -7: return +1;
        default: throw std::invalid_argument("Invalid delta.");
    }
}

using Bitboards = std::array<uint64_t, 12>;

// bit helpers
inline int bitscanForward(uint64_t bitboard) { return __builtin_ctzll(bitboard); }
inline uint64_t lsbReset(uint64_t number) { return number & (number - 1); }

// Iterate all set bits with a lambda f
template <typename F>
inline void forEachSetBit(uint64_t bb, F f) {
    while (bb) { uint8_t sq = bitscanForward(bb); f(sq); bb = lsbReset(bb); }
}

// board helpers
inline uint8_t rankOf(uint8_t square) { return square / NUM_SQUARES_IN_ROW; }
inline uint8_t fileOf(uint8_t square) { return square % NUM_SQUARES_IN_ROW; }
inline bool squareInBoard(int8_t square) { return 0 <= square && square < NUM_SQUARES; }
inline bool rankInBoard(int8_t rank) { return 0 <= rank && rank < NUM_SQUARES_IN_ROW; }
inline bool fileInBoard(int8_t file) { return 0 <= file && file < NUM_SQUARES_IN_ROW; }
inline bool notOuterTwoRanks(uint8_t square) { return 2 * NUM_SQUARES_IN_ROW <= square && square < NUM_SQUARES - 2 * NUM_SQUARES_IN_ROW; }
inline bool squareInRank(uint8_t square, uint8_t rank) { return rankOf(square) == rank; }
inline bool occupied(uint64_t occ, int8_t square) { return (0 <= square && square < NUM_SQUARES) && ((occ >> square) & 1); }
inline bool firstTwoRanks(uint8_t square, bool meWhite) {
    if (meWhite) return square < NUM_SQUARES_IN_ROW * 2;
    else return square >= NUM_SQUARES - NUM_SQUARES_IN_ROW * 2;
}

// for file checking
inline bool wrapped(uint8_t prev, uint8_t next, int8_t delta) { return std::abs(int(fileOf(next)) - int(fileOf(prev))) != std::abs(dxStepFromDelta(delta)); }

inline bool isWhite(Piece piece) { return to_u(piece) <= to_u(Piece::WK); }
inline bool isBlack(Piece piece) { return to_u(piece) >= to_u(Piece::BP) && to_u(piece) <= to_u(Piece::BK); }
inline const std::array<Piece, 6>& otherSidePieces(Piece p) { return isWhite(p) ? BLACK_PIECES : WHITE_PIECES; }

inline bool isPawn(Piece piece) { return piece == Piece::WP || piece == Piece::BP; }
inline bool isRook(Piece piece) { return piece == Piece::WR || piece == Piece::BR; }
inline bool isKnight(Piece piece) { return piece == Piece::WN || piece == Piece::BN; }
inline bool isBishop(Piece piece) { return piece == Piece::WB || piece == Piece::BB; }
inline bool isQueen(Piece piece) { return piece == Piece::WQ || piece == Piece::BQ; }
inline bool isRookBishopQueen(Piece piece) { return isRook(piece) || isBishop(piece) || isQueen(piece); }
inline bool isPawnKnight(Piece piece) { return isPawn(piece) || isKnight(piece); }

inline Piece getOtherPiece(Piece piece) { return static_cast<Piece>((to_u(piece) + 6) % to_u(Piece::PIECE_N)); }
inline uint8_t getOtherPieceCode(Piece piece) { return (to_u(piece) + 6) % to_u(Piece::PIECE_N); }

inline Piece getOurPawn(bool meWhite) { return meWhite ? Piece::WP : Piece::BP; }
inline Piece getOurRook(bool meWhite) { return meWhite ? Piece::WR : Piece::BR; }
inline Piece getOurKnight(bool meWhite) { return meWhite ? Piece::WN : Piece::BN; }
inline Piece getOurBishop(bool meWhite) { return meWhite ? Piece::WB : Piece::BB; }
inline Piece getOurQueen(bool meWhite) { return meWhite ? Piece::WQ : Piece::BQ; }
inline Piece getOurKing(bool meWhite) { return meWhite ? Piece::WK : Piece::BK; }

inline Piece getEnemyPawn(bool meWhite) { return meWhite ? Piece::BP : Piece::WP; }
inline Piece getEnemyRook(bool meWhite) { return meWhite ? Piece::BR : Piece::WR; }
inline Piece getEnemyKnight(bool meWhite) { return meWhite ? Piece::BN : Piece::WN; }
inline Piece getEnemyBishop(bool meWhite) { return meWhite ? Piece::BB : Piece::WB; }
inline Piece getEnemyQueen(bool meWhite) { return meWhite ? Piece::BQ : Piece::WQ; }
inline Piece getEnemyKing(bool meWhite) { return meWhite ? Piece::BK : Piece::WK; }

inline bool pawnMoveFromPromotion(uint8_t sq, bool meWhite) {
    if (meWhite) return rankOf(sq + NUM_SQUARES_IN_ROW) == EIGHTH_RANK;
    else return rankOf(int8_t(sq) - (int8_t)NUM_SQUARES_IN_ROW) == FIRST_RANK;
}

inline uint64_t getKingMoves(uint8_t kingSq) {
    uint64_t moves = 0;
    for (uint8_t i = 0; i < NUM_KING_DELTAS; ++i) {
        int8_t d = KING_DELTAS[i];
        int8_t to = int8_t(kingSq) + d;
        if (!squareInBoard(to) || wrapped(kingSq, to, d)) continue;
        moves |= (1ULL << to);
    }
    return moves;
}

constexpr auto sq = [](int f, int r) -> uint8_t { return r * NUM_SQUARES_IN_ROW + f; };

const uint8_t
        a1 = sq(7, 0), a8 = sq(7, 7),
        b1 = sq(6, 0), b8 = sq(6, 7),
        c1 = sq(5, 0), c8 = sq(5, 7),
        d1 = sq(4, 0), d8 = sq(4, 7),
        e1 = sq(3, 0), e8 = sq(3, 7),
        f1 = sq(2, 0), f8 = sq(2, 7),
        g1 = sq(1, 0), g8 = sq(1, 7),
        h1 = sq(0, 0), h8 = sq(0, 7);

// rooks, bishops, and queens
// excluding sq1 and sq2, squares in between
inline constexpr uint64_t rayBetween(int8_t sq1, int8_t sq2) {
    int8_t ri = rankOf(sq1), rf = rankOf(sq2);
    int8_t fi = fileOf(sq1), ff = fileOf(sq2);
    int8_t dr = rf - ri;
    int8_t df = ff - fi;

    // not on a vertical/horizontal or diagonal line
    if (!(dr == 0 || df == 0 || abs(dr) == abs(df))) return 0ULL;

    // steps (−1, 0, or +1)
    int rStep = (dr > 0) - (dr < 0);
    int fStep = (df > 0) - (df < 0);

    // creating mask
    uint64_t mask = 0ULL;
    uint8_t r = ri + rStep, f = fi + fStep;
    while (r != rf || f != ff) {
        mask |= (1ULL << sq(f, r));
        r += rStep;
        f += fStep;
    }
    return mask;
}

inline bool knightHopValid(uint8_t from, uint8_t to) {
    auto df = std::abs(int(fileOf(to))  - int(fileOf(from)));
    auto dr = std::abs(int(rankOf(to))  - int(rankOf(from)));
    return (df == 1 && dr == 2) || (df == 2 && dr == 1);
}

inline bool diagHopValid(uint8_t from, uint8_t to) {
    int df = std::abs(int(fileOf(to)) - int(fileOf(from)));
    int dr = std::abs(int(rankOf(to)) - int(rankOf(from)));
    return df == 1 && dr == 1;
}



inline bool pieceInBoard (uint8_t square, uint64_t occ) {return (1ULL << square) & occ; }
inline bool isPieceAtSquare(Bitboards& bb, Piece piece, uint8_t square) { return (bb[to_u(piece)] >> square) & 1; }

inline uint8_t kingSquare(Bitboards& bb, bool isWhite) { return isWhite ? bitscanForward(bb[WK_CODE]) : bitscanForward(bb[BK_CODE]); }

inline bool friendlyAt(Bitboards& bb, uint8_t square, bool meWhite) {
    const auto& set = meWhite ? WHITE_PIECES : BLACK_PIECES;
    for (auto p : set) if (isPieceAtSquare(bb, p, square)) return true;
    return false;
}
inline Piece enemyPieceAt(Bitboards& bb, uint8_t square, bool meWhite) {
    const auto& set = meWhite ? BLACK_PIECES : WHITE_PIECES;
    for (auto p : set) if (isPieceAtSquare(bb, p, square)) return p;
    return Piece::None;
}

#endif //TEMPO_UTILS_H
