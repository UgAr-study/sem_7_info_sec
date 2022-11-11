
#include "qc_mdpc.h"
#include "Matrix.hpp"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char const *argv[]) 
{
    const std::string fName_private = "private.txt";
    const std::string fName_public = "public.txt";

    if (argc != 5) {
        std::cout << "invalid arguments command line:\n argc = " <<  argc << std::endl;
        std::cout << "They are:\n";
        for (int i = 0; i < argc; i++)
        {
            printf ("[%d] %s\n", i, argv[i]);
        }
        std::cout << "Expected: \n" 
        << "n0 -- number of circular blocks\n"
        << "p  -- permutation size of each circular block\n"
        << "w  -- row weight\n"
        << "t  -- error word weight\n";
        
        return 1;
    }
    std::size_t pos = {};
    std::string sNum = argv[1];
    int n0 = std::stoi(sNum, &pos);
    if (pos != sNum.size()) {
        std::cout << "Incorrect n0: " << sNum << std::endl;
    }

    sNum = argv[2];
    int p = std::stoi(sNum, &pos);
    if (pos != sNum.size()) {
        std::cout << "Incorrect p: " << sNum << std::endl;
    }

    sNum = argv[3];
    int w = std::stoi(sNum, &pos);
    if (pos != sNum.size()) {
        std::cout << "Incorrect w: " << sNum << std::endl;
    }

    sNum = argv[4];
    int t = std::stoi(sNum, &pos);
    if (pos != sNum.size()) {
        std::cout << "Incorrect t: " << sNum << std::endl;
    }

    qc_mdpc key(n0, p, w, t);
    auto&& HMatrix = key.parity_check_matrix();
    auto&& GMatrix = key.generator_matrix();

    std::ofstream fOut(fName_private);
    fOut << "Private Key:" << std::endl;
    fOut << HMatrix << std::endl;
    fOut.close();

    fOut.open(fName_public);
    fOut << "Public Key:" << std::endl;
    fOut << GMatrix << std::endl;
    fOut.close();

    return 0;
}