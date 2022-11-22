#include "../include/qc_mdpc.h"


qc_mdpc::qc_mdpc(int n0, int p, int w, int seed)
        : n0(n0), p(p), w(w), n(n0 * p), r(p), k((n0 - 1) * p),
        row(n0 * p), row_col(p, w), col_row(n0 * p, w)
{
    if (seed == -1) {
        seed = time(0);
        gen.seed(seed);
        std::cout << "Seed: " << seed << std::endl;
    } else {
        gen.seed(seed);
    }

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
    std::cout << "MDPC code generated....\n";
    fill_reverse();
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

// fill row_col and col_row
void qc_mdpc::fill_reverse()
{
    int id = 0;
    for (int c = 0; c < row.size(); ++c)
        if (row[c] == 1)
            row_col[0][id++] = c;
    for (int i = 1; i < row_col.Num_Rows(); ++i)
        for (int c = 0; c < row_col.Num_Columns(); ++c)
            row_col[i][c] = row_col[0][c] / perm_size() + (row_col[0][c] + i) % perm_size();

    std::vector<int> ids(col_row.Num_Rows());
    for (int i = 0; i < row_col.Num_Rows(); ++i) {
        for (int c = 0; c < row_col.Num_Columns(); ++c) {
            int col = row_col[i][c];
            col_row[col][ids[col]++] = i;
        }
    }
}
//Create a binary circular matrix
BinMatrix qc_mdpc::make_matrix(int nrows, int ncols, const qc_mdpc::row_t &row) const
{
    BinMatrix res(nrows, ncols);
    res[0] = row;
    for (int i = 1; i < nrows; ++i) {
        res[i] = shift_row(row, i);
    }

    return res;
}

//Constructing the pariy check matrix
BinMatrix qc_mdpc::parity_check_matrix() const
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    BinMatrix H = make_matrix(p, p, splice_row(0, p));
    for (int i = 1; i < n0; i++) {
        BinMatrix M = make_matrix(p, p, splice_row(i * p, (i + 1) * p));
        H = concat_horizontal(H, M);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "Time for H: " << cpu_time_used << std::endl;
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
    std::cout << "Construction of G started...\n";
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
    std::cout << "Time for G: " << cpu_time_used << std::endl;
    std::cout << "Generator matrix generated....\n";
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
    BinMatrix msg = matrix_mult(vec.Transposition(), G);
    return msg.Transposition();
}

//Decoding the codeword
BinMatrix qc_mdpc::decode(const BinMatrix &codeword) const
{
    auto word = codeword;
    BinMatrix H = parity_check_matrix();
    BinMatrix syn = matrix_mult(H, word.Transposition());
    int limit = 10;
    int delta = 5;

    auto word_len = word.Num_Rows();
    for (int i = 0; i < limit; i++) {
        std::vector<int> unsatisfied(word_len);
        for (int j = 0; j < word_len; j++) {
            for (int s = 0; s < H.Num_Rows(); s++) {
                if (H[s][j] == 1 && syn[s][0] == 1)
                    ++unsatisfied[j];
            }
        }
        int b = get_max(unsatisfied) - delta;
        for (int j = 0; j < word_len; j++) {
            if (unsatisfied[j] >= b) {
                word[j][0] = !word[j][0];
                syn = syn + H.mat_splice(0, H.Num_Rows() - 1, j, j);
            }
        }
        if (syn.is_zero_matrix()) {
            return word;
        }
    }
    std::cout << "Decoding failure...\n";
    return {0, 0};
}



