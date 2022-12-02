#pragma once

#include <random>
#include "qc_mdpc.h"

class mceliece {
public:
    mceliece(int n0, int p, int w, int t, int seed = -1);
    mceliece(const BinMatrix& mdpc, int t, int seed = -1);
    virtual std::vector<int> encrypt(const BinMatrix &msg);
    BinMatrix encrypt(const BinMatrix &public_key, const BinMatrix &msg);
    virtual BinMatrix decrypt(const std::vector<int> &msg) const;
    BinMatrix decrypt(const BinMatrix &private_key, const BinMatrix &cipher);
private:
    BinMatrix get_error_vector();
public:
    qc_mdpc code;
    int error_weight;
    BinMatrix public_key;
    std::mt19937 gen;
};