#include "qc_mdpc.h"
#include <stdlib.h>
#include "matrix.h"
#include "utility.h"
#include <stdio.h>
#include <time.h>


qc_mdpc::qc_mdpc(int n0, int p, int w, int t, int seed)
        : n0(n0), p(p), w(w), t(t), row(n0 * p), gen(seed) {
    n = n0 * p;
    r = p;
    k = (n0 - 1) * p;

    std::uniform_int_distribution<int> u_rand(0, row.size() - 1);
    while (true) {
        int flag = 0;
        while (flag < w) {
            int idx = u_rand(gen);
            if (!row[idx]) {
                row[idx] = 1;
                flag++;
            }
        }
        if (get_row_weight((n0 - 1) * p, n0 * p) % 2 == 1) {
            break;
        }
        reset_row();
    }
    printf("MDPC code generated....\n");
}

//Return the weight of the given row from the indices [min, max)
int qc_mdpc::get_row_weight(int begin, int end) const {
    return std::count(row.begin() + begin, row.begin() + end, 1);
}

//Reset all positions in the row to 0
void qc_mdpc::reset_row() {
    std::fill(row.begin(), row.end(), 0);
}

//Rotate the row x positions to the right
qc_mdpc::row_t qc_mdpc::shift_row(const qc_mdpc::row_t &row, int shift)
{
    row_t res(row.size());
    for (int i = 0; i < row.size(); ++i)
        res[(i + shift) % res.size()] = row[i];
    return res;
}

//Splice the row for the given range [begin, end)
qc_mdpc::row_t qc_mdpc::splice_row(int begin, int end) const
{
    row_t res(end - begin);
    for (int i = begin; i < end; ++i)
        res[i - begin] = row[i];
    return res;
}

//Create a binary circular matrix
bin_matrix qc_mdpc::make_matrix(int nrows, int ncols, const qc_mdpc::row_t &row)
{
    bin_matrix res = mat_init(nrows, ncols);
    res.set_matrix_row(0, row);
    for (int i = 1; i < nrows; ++i)
        res.set_matrix_row(i, shift_row(row));
    
    return res;
}

//Constructing the pariy check matrix
bin_matrix qc_mdpc::parity_check_matrix() const
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    bin_matrix H = make_matrix(p, p, splice_row(0, p));
    int i;
    for (i = 1; i < n0; i++) {
        bin_matrix M = make_matrix(p, p, splice_row(i * p, (i + 1) * p));
        H.concat_horizontal(M);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for H: %f\n", cpu_time_used);
    // printf("H: \n");
    // print_matrix(H);
    //printf("Parity matrix generated...\n");
    return H;
}

//Constructing the generator matrix
bin_matrix qc_mdpc::generator_matrix() const
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    bin_matrix H = parity_check_matrix();

    //End of modified code
    printf("Construction of G started...\n");
    bin_matrix H_inv = circ_matrix_inverse(
            make_matrix(p, p, splice_row((n0 - 1) * p, n)));
    bin_matrix H_0 = make_matrix(p, p, splice_row(0, p));
    bin_matrix Q = transpose(matrix_mult(H_inv, H_0));
    bin_matrix M;
    for (int i = 1; i < n0 - 1; i++) {
        M = make_matrix(p, p, splice_row(i * p, (i + 1) * p));
        M = transpose(matrix_mult(H_inv, M));
        Q.concat_vertical(M);
    }
    bin_matrix I = mat_init(k, k);
    make_indentity(I);
    bin_matrix G = concat_horizontal(I, Q);

    //bin_matrix G = mat_kernel(H);
    //G = matrix_rref(G);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for G: %f\n", cpu_time_used);
    printf("Generator matrix generated....\n");
    return G;
}

//Returns the maximum element of the array
static int get_max(const std::vector<int>& vec) {
    return *std::max_element(vec.begin(), vec.end());
}

//Encoding the vector vec as a MDPC codeword
bin_matrix qc_mdpc::encode(bin_matrix vec) {
    bin_matrix G = generator_matrix();
    bin_matrix msg = matrix_mult(vec, G);
    return msg;
}

//Decoding the codeword
bin_matrix qc_mdpc::decode(bin_matrix word) {
    bin_matrix H = parity_check_matrix();
    bin_matrix syn = matrix_mult(H, transpose(word));
    int limit = 10;
    int delta = 5;
    int i, j, k, x;

    for (i = 0; i < limit; i++) {
        //printf("Iteration: %d\n", i);
        std::vector<int> unsatisfied(word->cols);
        for (x = 0; x < word->cols; x++) {
            unsatisfied[x] = 0;
        }
        for (j = 0; j < word->cols; j++) {
            for (k = 0; k < H->rows; k++) {
                if (get_matrix_element(H, k, j) == 1) {
                    if (get_matrix_element(syn, k, 0) == 1) {
                        unsatisfied[j] = unsatisfied[j] + 1;
                    }
                }
            }
        }
        // printf("No. of unsatisfied equations for each bit: \n");
        // for(int idx = 0; idx < word->cols; idx++)
        // {
        // 	printf("b%d: %d \n", idx, unsatisfied[idx]);
        // }
        int b = get_max(unsatisfied) - delta;
        for (j = 0; j < word->cols; j++) {
            if (unsatisfied[j] >= b) {
                set_matrix_element(word, 0, j, (get_matrix_element(word, 0, j) ^ 1));
                syn = add_matrix(syn, mat_splice(H, 0, H->rows - 1, j, j));
            }
        }
        // printf("Syndrome: ");
        // print_matrix(syn);
        // printf("\n");
        //printf("Iteration: %d\n", i);
        if (is_zero_matrix(syn)) {
            return word;
        }
    }
    printf("Decoding failure...\n");
    exit(0);
}



