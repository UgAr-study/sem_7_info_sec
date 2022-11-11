#include "qc_mdpc.h"


qc_mdpc::qc_mdpc(int n0, int p, int w, int t, int seed)
        : n0(n0), p(p), w(w), t(t), row(n0 * p)
{
    if (seed == -1) {
        gen.seed(time(0));
    } else {
        gen.seed(seed);
    }
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
int qc_mdpc::get_row_weight(int begin, int end) const
{
    return std::count(row.begin() + begin, row.begin() + end, 1);
}

//Reset all positions in the row to 0
void qc_mdpc::reset_row()
{
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
BinMatrix qc_mdpc::make_matrix(int nrows, int ncols, const qc_mdpc::row_t &row) const
{
    BinMatrix res(nrows, ncols);
    res[0] = row;
    for (int i = 1; i < nrows; ++i)
        res[i] = shift_row(row);

    return res;
}

//Constructing the pariy check matrix
BinMatrix qc_mdpc::parity_check_matrix() const
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    BinMatrix H = make_matrix(p, p, splice_row(0, p));
    int i;
    for (i = 1; i < n0; i++) {
        BinMatrix M = make_matrix(p, p, splice_row(i * p, (i + 1) * p));
        H = concat_horizontal(H, M);
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
BinMatrix qc_mdpc::generator_matrix() const
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    BinMatrix H = parity_check_matrix();

    //End of modified code
    printf("Construction of G started...\n");
    BinMatrix H_inv = make_matrix(p, p, splice_row((n0 - 1) * p, n)).circ_matrix_inverse();
    BinMatrix H_0 = make_matrix(p, p, splice_row(0, p));
    BinMatrix Q = matrix_mult(H_inv, H_0).Transposition();
    BinMatrix M;
    for (int i = 1; i < n0 - 1; i++) {
        M = make_matrix(p, p, splice_row(i * p, (i + 1) * p));
        M = matrix_mult(H_inv, M).Transposition();
        Q = concat_vertical(Q, M);
    }
    BinMatrix I(k, k);
    I.make_indentity();
    BinMatrix G = concat_horizontal(I, Q);

    //BinMatrix G = mat_kernel(H);
    //G = matrix_rref(G);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time for G: %f\n", cpu_time_used);
    printf("Generator matrix generated....\n");
    return G;
}

//Returns the maximum element of the array
static int get_max(const std::vector<int> &vec)
{
    return *std::max_element(vec.begin(), vec.end());
}

//Encoding the vector vec as a MDPC codeword
BinMatrix qc_mdpc::encode(const BinMatrix &vec) const
{
    BinMatrix G = generator_matrix();
    BinMatrix msg = matrix_mult(vec, G);
    return msg;
}

//Decoding the codeword
BinMatrix qc_mdpc::decode(const BinMatrix &codeword) const
{
    auto word = codeword;
    BinMatrix H = parity_check_matrix();
    BinMatrix syn = matrix_mult(H, word.Transposition());
    int limit = 10;
    int delta = 5;

    auto word_len = word.Num_Columns();
    for (int i = 0; i < limit; i++) {
        //printf("Iteration: %d\n", i);
        std::vector<int> unsatisfied(word_len);
        for (int j = 0; j < word_len; j++) {
            for (int s = 0; s < H.Num_Rows(); s++) {
                if (H[s][j] == 1 && syn[s][0] == 1)
                    unsatisfied[j] = unsatisfied[j] + 1;
            }
        }
        // printf("No. of unsatisfied equations for each bit: \n");
        // for(int idx = 0; idx < word_len; idx++)
        // {
        // 	printf("b%d: %d \n", idx, unsatisfied[idx]);
        // }
        int b = get_max(unsatisfied) - delta;
        for (int j = 0; j < word_len; j++) {
            if (unsatisfied[j] >= b) {
                word[0][j] = !word[0][j];
                syn = syn + H.mat_splice(0, H.Num_Rows() - 1, j, j);
            }
        }
        // printf("Syndrome: ");
        // print_matrix(syn);
        // printf("\n");
        //printf("Iteration: %d\n", i);
        if (syn.is_zero_matrix()) {
            return word;
        }
    }
    printf("Decoding failure...\n");
    exit(0);
}



