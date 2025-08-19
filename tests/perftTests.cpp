//
// Created by Kaveh Fayyazi on 8/18/25.
//

#include "catch.hpp"
#include "board.h"
#include "perft.h"

TEST_CASE("Perft leaves board state unchanged") {
    Board b = Board();
    uint64_t initKey = b.getKey();
    Perft(b, 1);
    Perft(b, 2);
    Perft(b, 3);
    Perft(b, 4);
    uint64_t finalKey = b.getKey();
    REQUIRE(initKey == finalKey);
}

TEST_CASE("Perft initial position node counts") {
    Board b = Board();
    REQUIRE(Perft(b, 1) == 20);
    REQUIRE(Perft(b, 2) == 400);
    REQUIRE(Perft(b, 3) == 8902);
    REQUIRE(Perft(b, 4) == 197281);
    REQUIRE(Perft(b, 5) == 4865609);
    REQUIRE(Perft(b, 6) == 119060324);
}

//TEST_CASE("Perft Kiwipete position node counts") {
//    Board b = Board();
//    b.setFromFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
//    REQUIRE(Perft(b, 1) == 48);
//    REQUIRE(Perft(b, 2) == 2039);
//    REQUIRE(Perft(b, 3) == 97862);
//    REQUIRE(Perft(b, 4) == 4085603);
//    REQUIRE(Perft(b, 5) == 193690690);
//    REQUIRE(Perft(b, 6) == 8031647685);
//}