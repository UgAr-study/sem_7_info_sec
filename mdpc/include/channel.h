#pragma once

#include <vector>

struct bpsk {
    static std::vector<int> modulate(std::vector<int> x);
    static std::vector<int> demodulate(std::vector<int> x);
};