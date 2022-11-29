#pragma once

#include "BinMatrix.h"
#include <string>
#include <fstream>

void SaveMatrix(const BinMatrix& mtrx, const std::string& file) {
    std::ofstream fileStream;
    fileStream.open(file);
    assert(fileStream.is_open());
    fileStream << mtrx.Num_Rows() << " " << mtrx.Num_Columns() << std::endl;
    fileStream << mtrx;
    fileStream << std::endl;
    fileStream.close();
}

static void tokenize(std::string const &str, const char delim,
              std::vector<int> &out)
{
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(std::atoi(str.substr(start, end - start).data()));
    }
}

BinMatrix LoadMatrix(const std::string& file) {
    std::ifstream in(file);
    assert(in.is_open());
    std::string line;
    std::vector<int> data;
    getline(in, line);
    tokenize(line, ' ', data);
    assert(data.size() == 2);
    BinMatrix res(data[0], data[1]);
    size_t row = 0;
    while (getline(in, line) && row != res.Num_Rows())
    {
        assert(row < res.Num_Rows());
        data.clear();
        tokenize(line, ' ', data);
        assert(data.size() == res.Num_Columns());
        for (size_t col = 0; col < res.Num_Columns(); ++col) {
            res[row][col] = data[col];
        }
        row++;
    }
    return res;
}

