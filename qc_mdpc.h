#pragma once

#include <algorithm>
#include <vector>
#include <random>
#include "matrix.h"
#include "utility.h"

class qc_mdpc
{
private:
    using row_t = std::vector<unsigned short>;
public:
    qc_mdpc(int n0, int p, int w, int t, int seed = -1);
    bin_matrix parity_check_matrix() const;
    bin_matrix generator_matrix() const;
    bin_matrix encode(bin_matrix vec);
    bin_matrix decode(bin_matrix word);

private:
    void reset_row();
    int get_row_weight(int begin, int end) const;
    static row_t shift_row(const row_t& row, int shift = 1);
    row_t splice_row(int begin, int end) const;
    bin_matrix make_matrix(int nrows, int ncols, const row_t& row);
private:
    std::mt19937 gen;
    row_t row; // one row is enough to describe the whole MDPC matrix
    /*
     * n0 -- number of circular blocks H = (h_0 | h_1 | ... | h_{n0-1})
     * p -- permutation size of each circular block
     * n = n0 * p -- number of columns in the matrix (codeword's length)
     * r = p -- number of rows in the matrix
     * k = n - r -- non encoded word's length
     * w -- row weight
     * t -- error word weight
     */
    int n0, p, w, t, n, k, r;
};

