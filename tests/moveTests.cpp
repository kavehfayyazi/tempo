//
// Created by Kaveh Fayyazi on 8/12/25.
//

#include "catch.hpp"
#include "types.h"
#include "move.h"

TEST_CASE("Knight move") {
    const uint8_t from = 12, to = 28;
    auto m = Move::make(from, to, Piece::WN, false, false, false, false);

    REQUIRE(Move::from(m) == from);
    REQUIRE(Move::to(m) == to);
    REQUIRE_FALSE(Move::isCapture(m));
    REQUIRE_FALSE(Move::isCastle(m));
    REQUIRE_FALSE(Move::isDPP(m));
    REQUIRE_FALSE(Move::isEP(m));
    REQUIRE(Move::promo(m) == 0x07);
    REQUIRE(Move::movedCode(m) == to_u(Piece::WN));
    REQUIRE(Move::capturedCode(m) == to_u(Piece::None));

    // only 27 bits used
    REQUIRE((m >> 27) == 0);
}

TEST_CASE("Capture move encodes piece and flag") {
    // White pawn captures a black knight
    const uint8_t from = 28, to = 35;
    auto m = Move::make(from, to, Piece::WP, true, false, false, false, Piece::BN);

    REQUIRE(Move::from(m) == from);
    REQUIRE(Move::to(m) == to);
    REQUIRE(Move::isCapture(m));
    REQUIRE_FALSE(Move::isCastle(m));
    REQUIRE_FALSE(Move::isDPP(m));
    REQUIRE_FALSE(Move::isEP(m));
    REQUIRE(Move::promo(m) == 0x07);
    REQUIRE(Move::movedCode(m) == to_u(Piece::WP));
    REQUIRE(Move::capturedCode(m) == to_u(Piece::BN));
}

TEST_CASE("Double pawn push sets DPP only") {
    // e2->e4 double pawn push
    auto m = Move::make(12, 28, Piece::WP, false, false, true, false);

    REQUIRE(Move::from(m) == 12);
    REQUIRE(Move::to(m) == 28);
    REQUIRE_FALSE(Move::isCapture(m));
    REQUIRE_FALSE(Move::isCastle(m));
    REQUIRE(Move::isDPP(m));
    REQUIRE_FALSE(Move::isEP(m));
    REQUIRE(Move::promo(m) == 0x07); // Promo::None is represented by 0x07
}

TEST_CASE("En passant capture sets EP and capture, captured piece is pawn") {
    // e5xd6 ep
    auto m = Move::make(36, 43, Piece::WP, true, false, false, true, Piece::BP);

    REQUIRE(Move::isCapture(m));
    REQUIRE(Move::isEP(m));
    REQUIRE_FALSE(Move::isCastle(m));
    REQUIRE_FALSE(Move::isDPP(m));
    REQUIRE(Move::capturedCode(m) == to_u(Piece::BP));
}

TEST_CASE("Promotion without capture") {
    // a7->a8=Q : from=48, to=56
    auto m = Move::make(48, 56, Piece::WP, false, false, false, false, Piece::None, Promo::Q);

    REQUIRE(Move::from(m) == 48);
    REQUIRE(Move::to(m) == 56);
    REQUIRE_FALSE(Move::isCapture(m));
    REQUIRE(Move::promo(m) == to_u(Promo::Q));
    REQUIRE(Move::movedCode(m) == to_u(Piece::WP));
    REQUIRE(Move::capturedCode(m) == to_u(Piece::None));
}

TEST_CASE("Promotion with capture") {
    // b7xa8=N
    auto m = Move::make(49, 56, Piece::WP, true, false, false, false, Piece::BR, Promo::N);

    REQUIRE(Move::isCapture(m));
    REQUIRE(Move::promo(m) == to_u(Promo::N));
    REQUIRE(Move::capturedCode(m) == to_u(Piece::BR));
}

TEST_CASE("Castle flag set correctly") {
    // White short castle: e1->g1, from=4, to=6
    auto m = Move::make(4, 6, Piece::WK, false, true, false, false);

    REQUIRE_FALSE(Move::isCapture(m));
    REQUIRE(Move::isCastle(m));
    REQUIRE_FALSE(Move::isDPP(m));
    REQUIRE_FALSE(Move::isEP(m));
    REQUIRE(Move::movedCode(m) == to_u(Piece::WK));
}

TEST_CASE("No captured piece decodes to None") {
    auto m = Move::make(10, 18, Piece::WB, false, false, false, false);

    REQUIRE_FALSE(Move::isCapture(m));
    REQUIRE(Move::capturedCode(m) == to_u(Piece::None));
}

TEST_CASE("Edge squares") {
    // from max square, to zero
    auto m = Move::make(63, 0, Piece::BK, false, false, false, false);

    REQUIRE(Move::from(m) == 63);
    REQUIRE(Move::to(m) == 0);
    REQUIRE(Move::movedCode(m) == to_u(Piece::BK));
    REQUIRE_FALSE(Move::isCapture(m));
    REQUIRE(Move::promo(m) == 0x07);
}