#include "BinMatrix.h"

BinMatrix &BinMatrix::add_rows(size_t row1, size_t row2) {
    for(size_t i = 0; i < Num_Columns(); ++i) {
        (*this)[row2][i] = (*this)[row1][i] ^ (*this)[row2][i];
    }
    return *this;
}

BinMatrix &BinMatrix::add_rows_new(size_t row1, size_t row2, size_t a, size_t b) {
    for(size_t i = a; i < b; ++i) {
        (*this)[row2][i] = (*this)[row1][i] ^ (*this)[row2][i];
    }
    return *this;
}

BinMatrix &BinMatrix::add_cols(int col1, int col2, int a, int b)
{
    for(int i = a; i < b; i++) {
        (*this)[i][col2] = (*this)[i][col1] ^ (*this)[i][col2];
    }
    return *this;
}

void BinMatrix::swap(size_t row1, size_t row2) {
    Swap_Rows(row1, row2);
}

BinMatrix operator+(const BinMatrix& left, const BinMatrix& right) {
    if (left.Num_Rows() < right.Num_Rows() || left.Num_Columns() < right.Num_Columns()) {
        std::cerr << "BinMatrix Add ERROR!\n";
        exit(EXIT_FAILURE);
    }
    BinMatrix res = left;
    for(int i = 0; i < res.Num_Rows(); i++) {
        for(int j = 0; j < res.Num_Columns(); j++)
        {
            res[i][j] = left[i][j] ^ right[i][j];
        }
    }
    return res;
}

BinMatrix BinMatrix::matrix_rref() {
    BinMatrix tmp = (*this);

    int r = 0;
    while(r < Num_Rows()) {
        if(!tmp[r][r]) {
            int i = 0;
            for(i = r + 1; i < tmp.Num_Rows(); ++i) {
                if(tmp[i][r]) {
                    tmp.swap(r, i);
                    break;
                }
            }
            if(i == Num_Rows()) {
                std::cerr << "Matix cannot be transformed" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            for(int i = 0; i < Num_Rows(); i++) {
                if(tmp[i][r] && i != r) {
                    tmp.add_rows(r, i);
                }
            }
            r++;
        }
    }
    return tmp;
}

BinMatrix matrix_mult(const BinMatrix& A, const BinMatrix& B) {
    if (A.Num_Columns() != B.Num_Rows()) {
        std::cerr << "Matrices have erroneous dimensions: ("
                  << A.Num_Rows() << ", " << A.Num_Columns() << ") to ("
                  << B.Num_Rows() << ", " << B.Num_Columns() << ")" << std::endl;
        exit(0);
    }

    BinMatrix C(A.Num_Rows(), B.Num_Columns());
    BinMatrix B_T = B.Transposition();

    for(int i = 0; i < C.Num_Rows(); i++) {
        for(int j = 0  ; j < C.Num_Columns(); j++) {
            bool val = false;
            for(int k = 0; k < B.Num_Rows(); k++) {
                val = (val ^ (A[i][k] & B_T[j][k]));
            }
            C[i][j] = val;
        }
    }
    return C;
}

BinMatrix& BinMatrix::make_indentity() {
    if (Num_Columns() != Num_Rows()) {
        std::cerr << "Matrix not square" << std::endl;
        exit(0);
    }
    Clear();
    for (int i = 0; i < Num_Rows(); ++i) {
        (*this)[i][i] = true;
    }
    return (*this);
}

bool BinMatrix::is_identity() const {
    for(int i = 0; i < Num_Rows(); ++i) {
        for(int j = 0; j < Num_Columns(); ++j) {
            if(i == j) {
                if(!(*this)[i][j]) {
                    return false;
                }
            } else {
                if((*this)[i][j]) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool BinMatrix::is_zero_matrix() const {
    for(int i = 0; i < Num_Rows(); i++)
    {
        for(int j = 0; j < Num_Columns(); j++)
        {
            if((*this)[i][j]) {
                return false;
            }
        }
    }
    return true;
}

BinMatrix BinMatrix::circ_matrix_inverse() const {
    if(Num_Rows() != Num_Columns()) {
        printf("Inverse not possible\n");
        exit(0);
    }

    if(is_identity()) {
        return (*this);
    }

    BinMatrix B(Num_Rows(), Num_Columns());
    B.make_indentity();
    BinMatrix copy = (*this);

    int i;
    for(i = 0; i < Num_Columns(); ++i) {
        if((*this)[i][i]) {
            for(int j = 0; j < Num_Rows(); ++j) {
                if(i != j && (*this)[j][i]) {
                    B.add_rows_new(i, j, 0, Num_Columns());
                    copy.add_rows_new(i, j, i, Num_Columns());
                }
            }
        } else {
            int k;
            for(k = i + 1; k < Num_Rows(); ++k) {
                if(copy[k][i]) {
                    B.add_rows(k, i);
                    copy.add_rows(k, i);
                    i = i - 1;
                    break;
                }
            }
        }
    }
    if(!copy.is_identity())
    {
        std::cerr << "Could not find inverse" << std::endl;
        exit(EXIT_FAILURE);
    }

    return B;
}

BinMatrix BinMatrix::mat_splice(int row1, int row2, int col1, int col2) {
    int row_count = row2 - row1 + 1;
    int col_count = col2 - col1 + 1;
    int idx1, idx2;

    BinMatrix res(row_count, col_count);
    for(int i = 0; i < row_count; ++i) {
        idx1 = row1 + i;
        for(int j = 0; j < col_count; ++j) {
            idx2 = col1 + j;
            res[i][j] = (*this)[idx1][idx2];
        }
    }
    return res;
}

BinMatrix BinMatrix::mat_kernel() const {
    int row_count = Num_Rows();
    int col_count = Num_Columns();

    BinMatrix tmp(col_count, row_count + col_count);
    BinMatrix ans(col_count, col_count - row_count);

    for (int i = 0; i < tmp.Num_Rows(); ++i) {
        for (int j = 0; j < row_count; ++j) {
            tmp[i][j] = (*this)[j][i];
        }
    }

    for (int i = 0; i < col_count; ++i) {
        tmp[i][i + row_count] = true;
    }

    int r = 0;
    while (r < row_count) {
        if (!tmp[r][r]) {
            int i;
            for (i = r + 1; i < tmp.Num_Rows(); i++) {
                if (tmp[i][r]) {
                    tmp.swap(r, i);
                    break;
                }
            }
            if (i == tmp.Num_Rows()) {
                ans = tmp.mat_splice(row_count, col_count - 1, row_count, row_count + col_count - 1);
                return ans.matrix_rref();
            }
        } else {
            for (int i = 0; i < tmp.Num_Rows(); ++i) {
                if (tmp[i][r] && i != r) {
                    tmp.add_rows(r, i);
                }
            }
            r++;
        }
    }
    ans = tmp.mat_splice(row_count, col_count - 1, row_count, row_count + col_count - 1);
    return ans.matrix_rref();
}

BinMatrix concat_horizontal(const BinMatrix& A, const BinMatrix& B) {
    if(A.Num_Rows() != B.Num_Rows()) {
        std::cerr << "Incompatible dimensions" << std::endl;
        exit(0);
    }
    BinMatrix tmp(A.Num_Rows(), A.Num_Columns() + B.Num_Columns());
    for(int i = 0; i < tmp.Num_Rows(); i++) {
        for(int j = 0; j < tmp.Num_Columns(); j++) {
            if(j < A.Num_Columns()) {
                tmp[i][j] = A[i][j];
            } else {
                tmp[i][j] = B[i][j - A.Num_Columns()];
            }
        }
    }
    return tmp;
}

BinMatrix concat_vertical(const BinMatrix& A, const BinMatrix& B) {
    if(A.Num_Columns() != B.Num_Columns()) {
        std::cerr << "Incompatible dimensions" << std::endl;
        exit(0);
    }
    BinMatrix tmp(A.Num_Rows() + B.Num_Rows(), A.Num_Columns());
    for(int i = 0; i < tmp.Num_Rows(); ++i) {
        for(int j = 0; j < tmp.Num_Columns(); j++)
        {
            if(i < A.Num_Rows()) {
                tmp[i][j] = A[i][j];
            } else {
                tmp[i][j] = B[i - A.Num_Rows()][j];
            }
        }
    }
    return tmp;
}