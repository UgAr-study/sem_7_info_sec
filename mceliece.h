#pragma once

#include <random>
#include "qc_mdpc.h"
#include "matrix.h"
#include "utility.h"


class mceliece {
public:
    mceliece(int n0, int p, int w, int t);
    bin_matrix encrypt(const bin_matrix &msg) const;
    bin_matrix decrypt(const bin_matrix &msg) const;
private:
    bin_matrix get_error_vector() const;
private:
    qc_mdpc code;
    bin_matrix public_key;
    std::mt19937 gen;
};