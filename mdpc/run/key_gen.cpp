#include "qc_mdpc.h"

#include "CLI11.hpp"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char const *argv[]) 
{
    int n0 = 0, p = 0, w = 0, seed = -1;
    CLI::App app{"Key generation"};

    app.add_option("--n0", n0, "Number of circular blocks in H matrix")->required();
    app.add_option("--p", p, "Permutation size of circular block")->required();
    app.add_option("--w", w, "Row weight")->required();
    app.add_option("--seed", seed, "Seed for random. If == -1, seed will be random generated.")->default_val(-1);

    std::string fName_private = "private.txt";
    std::string fName_public = "public.txt";
    app.add_option("--file-private", fName_private, "file for dump private key")->default_str("private.txt");
    app.add_option("--file-public", fName_public, "file for dump public key")->default_str("public.txt");

    CLI11_PARSE(app, argc, argv);

    qc_mdpc key(n0, p, w, seed);
    auto&& HMatrix = key.parity_check_matrix();
    auto&& GMatrix = key.generator_matrix();

    std::ofstream fOut(fName_public);
    fOut << "Public Key:" << std::endl;
    fOut << HMatrix.Num_Rows() << " " << HMatrix.Num_Columns() << std::endl;
    fOut << HMatrix << std::endl;
    fOut.close();

    fOut.open(fName_private);
    fOut << "Private Key:" << std::endl;
    fOut << GMatrix.Num_Rows() << " " << GMatrix.Num_Columns() << std::endl;
    fOut << GMatrix << std::endl;
    fOut.close();

    return 0;
}