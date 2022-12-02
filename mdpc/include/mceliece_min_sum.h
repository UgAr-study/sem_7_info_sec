#pragma once

#include <random>
#include <vector>
#include "BinMatrix.h"
#include "channel.h"
#include "qc_mdpc.h"
#include "mceliece.h"
#include "min_sum_decoder.hpp"

class mceliece_min_sum : public mceliece {
public:
    mceliece_min_sum(int n0, int p, int w, float snr, int seed = -1);
    mceliece_min_sum(const BinMatrix& mdpc, float snr);
    /**
     * @param msg information which we want to encrypt (matrix 1 x N)
     * @return encrypted information with noise from AWGN
     */
    std::vector<int> encrypt(const BinMatrix &msg) override;
    BinMatrix decrypt(const std::vector<int> &msg) const override;
private:
    MinSumDecoder decoder;
    awgn ch;
};
