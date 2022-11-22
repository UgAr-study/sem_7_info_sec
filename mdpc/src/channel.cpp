#include "../include/channel.h"

std::vector<float> bpsk::modulate(const std::vector<bool>& x)
{
    std::vector<float> res(x.size());
    for (int i = 0; i < x.size(); ++i)
        res[i] = 2.f * x[i] - 1.f;
    return res;
}

std::vector<float> bpsk::demodulate_llr(std::vector<float> x, float sigma)
{
    for (auto & i : x)
        i = 2.f * i / sigma / sigma;
    return x;
}
