#pragma once

#include <vector>
#include <random>

struct bpsk {
    static std::vector<float> modulate(const std::vector<bool>& x);
    static std::vector<float> demodulate_llr(std::vector<float> x, float sigma);
};

struct awgn {
    float                              snr;
    std::mt19937                       rng;
    float                              sigma = powf(10.f, -snr / 20.f);
    std::normal_distribution<float>    noise{0.f, sigma};

    explicit awgn(float snr) : snr(snr) {}

    std::vector<float>
    operator()(std::vector<float> msg)
    {
        for (auto& signal : msg)
            signal += noise(rng);
        return msg;
    }

    std::vector<float>
    demodulate(const std::vector<float>& modulated) const
    {
        return bpsk::demodulate_llr(modulated, sigma);
    }
};