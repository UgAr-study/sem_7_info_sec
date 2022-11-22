#pragma once

#include <random>
#include "qc_mdpc.h"

class mceliece {
public:
    mceliece(int n0, int p, int w, int t, int seed = -1);
    BinMatrix encrypt(const BinMatrix &msg);
    BinMatrix decrypt(const BinMatrix &msg) const;
private:
    BinMatrix get_error_vector();
private:
    qc_mdpc code;
    int error_weight;
    BinMatrix public_key;
    std::mt19937 gen;
};