//
// Created by Kaveh Fayyazi on 8/9/25.
//

#include "catch.hpp"
#include "board.h"

// Change the fields in Board to public.

TEST_CASE("Occupancy Bitboards") {
    Board b = Board();
    REQUIRE(b.occWhite == 0x000000000000FFFFULL);
    REQUIRE(b.occBlack == 0xFFFF000000000000ULL);
    REQUIRE(b.occAll == 0xFFFF00000000FFFFULL);
}

TEST_CASE("Castling") {
    Board b = Board();
    REQUIRE(b.castling == 0x0F);
}