//
// Created by Kaveh Fayyazi on 8/12/25.
//

#include "catch.hpp"
#include "types.h"

static_assert(std::is_same_v<decltype(to_u(Piece::WP)), std::underlying_type_t<Piece>>);
static_assert(std::is_same_v<decltype(to_u(Castling::W_K)), std::underlying_type_t<Castling>>);
static_assert(std::is_same_v<decltype(to_u(Promo::Q)), std::underlying_type_t<Promo>>);

// Enumerator values (based on your enums)
static_assert(to_u(Piece::WP) == 0);
static_assert(to_u(Piece::WK) == 5);
static_assert(to_u(Piece::BP) == 6);
static_assert(to_u(Piece::BK) == 11);
static_assert(to_u(Piece::PIECE_N) == 12);
static_assert(to_u(Piece::None) == 0xFF);

static_assert(to_u(Promo::None) == 0);
static_assert(to_u(Promo::R) == 1);
static_assert(to_u(Promo::N) == 2);
static_assert(to_u(Promo::B) == 3);
static_assert(to_u(Promo::Q) == 4);

static_assert(to_u(Castling::None) == 0);
static_assert(to_u(Castling::W_K) == 0b0001);
static_assert(to_u(Castling::W_Q) == 0b0010);
static_assert(to_u(Castling::B_K) == 0b0100);
static_assert(to_u(Castling::B_Q) == 0b1000);

// constexpr use in a non-type template argument
template <size_t N>
struct ArrWrap { std::array<int, N> arr{}; };

static ArrWrap<to_u(Piece::PIECE_N)> _arrWrap{}; // should compile (constexpr)

//
// Runtime tests
//
TEST_CASE("to_u works for array indexing with Piece") {
    std::array<int, to_u(Piece::PIECE_N)> bb{}; // size 12
    bb[to_u(Piece::WN)] = 42;
    bb[to_u(Piece::BQ)] = 99;

    REQUIRE(bb[to_u(Piece::WN)] == 42);
    REQUIRE(bb[to_u(Piece::BQ)] == 99);
    REQUIRE(bb.size() == to_u(Piece::PIECE_N));
}

TEST_CASE("to_u makes castling bitmasks easy") {
    uint8_t mask = to_u(Castling::W_K) | to_u(Castling::W_Q)
                   | to_u(Castling::B_K) | to_u(Castling::B_Q);

    REQUIRE(mask == 0b1111);

    // test individual bits
    REQUIRE((mask & to_u(Castling::W_K)) != 0);
    REQUIRE((mask & to_u(Castling::W_Q)) != 0);
    REQUIRE((mask & to_u(Castling::B_K)) != 0);
    REQUIRE((mask & to_u(Castling::B_Q)) != 0);
}

TEST_CASE("to_u values are the expected underlying codes") {
    REQUIRE(to_u(Piece::WQ) == 4);
    REQUIRE(to_u(Piece::BK) == 11);
    REQUIRE(to_u(Promo::None) == 0);
    REQUIRE(to_u(Promo::Q) == 4);
    REQUIRE(to_u(Castling::None) == 0);
    REQUIRE(to_u(Castling::B_Q) == 8);
}

TEST_CASE("to_u is constexpr-friendly in local constexpr contexts") {
    constexpr auto wp = to_u(Piece::WP);
    constexpr auto wk = to_u(Piece::WK);
    constexpr auto cq = to_u(Castling::W_Q);
    static_assert(wp == 0, "WP should be 0");
    static_assert(wk == 5, "WK should be 5");
    static_assert(cq == 2, "W_Q should be 2");
    REQUIRE(wp + wk == 5);
    REQUIRE(cq == 2);
}