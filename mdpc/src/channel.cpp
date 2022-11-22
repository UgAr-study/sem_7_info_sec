#include "../include/channel.h"

std::vector<int> bpsk::modulate(std::vector<int> x)
{
    for (auto & i : x)
        i = 2 * i - 1;
    return x;
}

std::vector<int> bpsk::demodulate(std::vector<int> x)
{
    for (auto & i : x)
        i = (i + 1) / 2;
    return x;
}
