#pragma once

#include <vector>
#include "Matrix.hpp"

//using BinMatrix = mtrx::Matrix_t<bool>;

class BinMatrix : public mtrx::Matrix_t<bool> {
public:
    BinMatrix() : mtrx::Matrix_t<bool>() {}
    BinMatrix(size_t numRows, size_t numColumns) : mtrx::Matrix_t<bool>(numRows, numColumns) {}
    BinMatrix(const BinMatrix& other)  = default;
    BinMatrix(const std::vector<bool>& vecMtrx, int numRows, int numColumns);
    BinMatrix(Matrix_t<bool> matrix) : mtrx::Matrix_t<bool>(matrix){}

    BinMatrix& add_rows(size_t row1, size_t row2);
    //Add the elements of row1 to row2 in the column index range [a,b] and save to row1
    BinMatrix& add_rows_new(size_t row1, size_t row2, size_t a, size_t b);
    //Add col1 and col2 from in the row index range [a,b] and save to col2
    BinMatrix& add_cols(int col1, int col2, int a, int b);

    void swap(size_t row1, size_t row2);
    //Get Row echelon form
    BinMatrix matrix_rref();

    BinMatrix& make_indentity();

    bool is_identity() const;
    bool is_zero_matrix() const;

    //Inverse of matrix
    BinMatrix circ_matrix_inverse() const;

    BinMatrix mat_splice(int row1, int row2, int col1, int col2);

    //Finding the basis of the kernel space of a matrix A
    BinMatrix mat_kernel() const;
    //Concatenate the matrices A and B as [A|B]
};

BinMatrix operator+(const BinMatrix& left, const BinMatrix& right);
BinMatrix matrix_mult(const BinMatrix& A, const BinMatrix& B);
//Concatenate the matrices A and B as [A|B]
BinMatrix concat_horizontal(const BinMatrix& A, const BinMatrix& B);
//Concatenate the matrices A and B vertically
BinMatrix concat_vertical(const BinMatrix& A, const BinMatrix& B);
