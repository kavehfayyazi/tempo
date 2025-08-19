#include <iostream>

#include "perft.h"
#include "board.h"
#include "movegen.h"

using MoveList = std::vector<uint32_t>;

int main() {
    Board board = Board();
    for (size_t i = 0; i < 9; i++) {
        uint64_t depth = Perft(board, i);
        std::cout << "Depth of " << i << ": " << depth << std::endl;
    }
    return 0;
}