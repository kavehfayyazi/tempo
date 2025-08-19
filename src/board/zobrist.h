//
// Created by Kaveh Fayyazi on 8/9/25.
//

#ifndef TEMPO_ZOBRIST_H
#define TEMPO_ZOBRIST_H

#include "move.h"
#include "utils.h"
#include <cstdint>
#include <array>

struct Zobrist {
    const std::array<std::array<uint64_t, NUM_SQUARES>, size_t(Piece::PIECE_N)> pieces;
    const uint64_t blackToMove;
    const std::array<uint64_t, CASTLING_N> castling;
    const std::array<uint64_t, NUM_SQUARES_IN_ROW> epFile;

    uint64_t key;
public:
    Zobrist() :
        pieces([] {
            std::array<std::array<uint64_t, NUM_SQUARES>, (size_t)Piece::PIECE_N> tmp{};
            for(size_t piece = 0; piece < (size_t)Piece::PIECE_N; ++piece)
                for (size_t square = 0; square < NUM_SQUARES; ++square)
                    tmp[piece][square] = random_u64();
            return tmp;
        }()),
        blackToMove(random_u64()),
        castling([] {
            std::array<uint64_t, CASTLING_N> tmp{};
            for(size_t piece = 0; piece < CASTLING_N; ++piece)
                tmp[piece] = random_u64();
            return tmp;
        }()),
        epFile([] {
            std::array<uint64_t, NUM_SQUARES_IN_ROW> tmp{};
            for(size_t piece = 0; piece < NUM_SQUARES_IN_ROW; ++piece)
                tmp[piece] = random_u64();
            return tmp;
        }()),
        key([&] {
            uint64_t result = 0;
            // XORing pieces
            for(size_t piece = 0; piece < (size_t)Piece::PIECE_N; ++piece)
                for (size_t square = 0; square < NUM_SQUARES; ++square)
                    result ^= pieces[piece][square];
            // XORing castling
            for(size_t piece = 0; piece < CASTLING_N; ++piece)
                result ^= castling[piece];
            return result;
        }())
    {}

    uint64_t getKey() const { return key; };
};

#endif //TEMPO_ZOBRIST_H
