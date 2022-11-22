#pragma once

#include <random>
#include <vector>
#include "BinMatrix.h"
#include "channel.h"
#include "qc_mdpc.h"
#include "min_sum_decoder.hpp"

class mceliece_min_sum {
public:
    mceliece_min_sum(int n0, int p, int w, float snr, int seed = -1);
    /**
     * @param msg information which we want to encrypt (matrix 1 x N)
     * @return encrypted information with noise from AWGN
     */
    std::vector<int> encrypt(const BinMatrix &msg);
    BinMatrix decrypt(const std::vector<int> &msg) const;
private:
    qc_mdpc code;
    MinSumDecoder decoder;
    BinMatrix public_key;
    awgn ch;
};
