#pragma once

#include <vector>
#include "channel.h"
#include "qc_mdpc.h"

struct Message {
    std::vector<bool> information;
    std::vector<float> rx;
    std::vector<int> llr;
};

class MsgGenerator {
public:
    MsgGenerator(const qc_mdpc& mdpc, float snr) : ch(snr), code(mdpc) {};
    Message zero();
    Message random();
private:
    Message from_channel(const std::vector<bool>& info);
private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> distr{0, 1};
    awgn ch;
    qc_mdpc code;
};