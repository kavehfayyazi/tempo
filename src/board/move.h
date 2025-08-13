//
// Created by Kaveh Fayyazi on 8/11/25.
//

#ifndef TEMPO_MOVE_H
#define TEMPO_MOVE_H

#include "types.h"
#include <cstdint>

struct Move {
    // Layout (LSB -> MSB):
    // 0-5:     from square [0,63]
    // 6-11:    to square [0,63]
    // 12-15:   moved piece [0,11], 0xF means "not stored"
    // 16:      capture flag
    // 17:      castle flag
    // 18:      double pawn push flag
    // 19:      en passant capture flag
    // 20-22:   promo [0,4], 0 means None
    // 23-26:   captured piece [0,11], 0xF means None

    static constexpr uint8_t SQUARE_MASK = 0x3F;
    static constexpr uint8_t PIECE_MASK = 0xF;
    static constexpr uint8_t PROMO_MASK = 0x7;

    static constexpr uint32_t FROM_SHIFT = 0;
    static constexpr uint32_t TO_SHIFT = 6;
    static constexpr uint32_t MOVED_SHIFT = 12;
    static constexpr uint32_t PROMO_SHIFT = 20;
    static constexpr uint32_t CAPTURE_SHIFT = 23;

    static constexpr uint32_t CAPTURE_FLAG_SHIFT = 16;
    static constexpr uint32_t CASTLE_FLAG_SHIFT = 17;
    static constexpr uint32_t PAWN_DPP_FLAG_SHIFT = 18;
    static constexpr uint32_t EP_FLAG_SHIFT = 19;

    static constexpr uint32_t CAPTURED_FLAG = 1ULL << CAPTURE_FLAG_SHIFT;
    static constexpr uint32_t CASTLE_FLAG = 1ULL << CASTLE_FLAG_SHIFT;
    static constexpr uint32_t PAWN_DOUBLE_MOVE_FLAG = 1ULL << PAWN_DPP_FLAG_SHIFT;
    static constexpr uint32_t EN_PASSANT_FLAG = 1ULL << EP_FLAG_SHIFT;


    // isDPP = is double pawn push
    // isEP = is en passant
    static inline uint32_t make(uint8_t from, uint8_t to, Piece moved,
                         bool isCaptured, bool isCastled, bool isDPP, bool isEP,
                         Piece capturedPiece =  Piece::None, Promo promoted = Promo::None)
    {
        const uint32_t movedCode = static_cast<uint32_t>(moved) & PIECE_MASK;
        const uint32_t capturedCode = static_cast<uint32_t>(capturedPiece) & PIECE_MASK;
        const uint32_t promoCode = static_cast<uint32_t>(promoted) & PROMO_MASK;

        uint64_t move{0};
        move = ((uint32_t(from) & SQUARE_MASK) << FROM_SHIFT)
             | ((uint32_t(to) & SQUARE_MASK) << TO_SHIFT)
             | (movedCode << MOVED_SHIFT)
             | (promoCode << PROMO_SHIFT)
             | (capturedCode << CAPTURE_SHIFT);

        if (isCaptured) move |= CAPTURED_FLAG;
        if (isCastled) move |= CASTLE_FLAG;
        if (isDPP) move |= PAWN_DOUBLE_MOVE_FLAG;
        if (isEP) move |= EN_PASSANT_FLAG;

        return move;
    }

    static inline uint8_t from(uint64_t move)               { return (move >> FROM_SHIFT) & SQUARE_MASK; }
    static inline uint8_t to(uint64_t move)                 { return (move >> TO_SHIFT) & SQUARE_MASK; }
    static inline bool    isCapture(uint64_t move)          { return move & CAPTURED_FLAG; }
    static inline bool    isCastle(uint64_t move)           { return move & CASTLE_FLAG; }
    static inline bool    isDoublePawnPush(uint64_t move)   { return move & PAWN_DOUBLE_MOVE_FLAG; }
    static inline bool    isEnPassant(uint64_t move)        { return move & EN_PASSANT_FLAG; }
    static inline Promo   promo(uint64_t move)              { return static_cast<Promo>((move >> PROMO_SHIFT) & PROMO_MASK); }

    // 0xF -> Piece::None
    static inline Piece   movedPiece(uint32_t move) {
        uint8_t code = (move >> MOVED_SHIFT) & PIECE_MASK;
        return code == 0xF ? Piece::None : static_cast<Piece>(code);
    }
    static inline Piece   capturedPiece(uint32_t move) {
        uint8_t code = (move >> CAPTURE_SHIFT) & PIECE_MASK;
        return code == 0xF ? Piece::None : static_cast<Piece>(code);
    }
};

#endif //TEMPO_MOVE_H
