#include <iostream>

#include "board.h"
#include "movegen.h"

using MoveList = std::vector<uint32_t>;

int main() {
    Board b = Board();
    MoveList moves = b.genLegalMoves();
    return 0;
}