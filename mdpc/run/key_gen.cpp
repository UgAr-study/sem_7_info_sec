#include "qc_mdpc.h"
#include "CLI11.hpp"

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char const *argv[]) 
{
    int n0 = 0, p = 0, w = 0;
    CLI::App app{"Key generation"};

    app.add_option("--n0", n0, "Number of circular blocks in H matrix");
    app.add_option("--p", p, "Permutation size of circular block");
    app.add_option("--w", w, "Row weight");

    CLI11_PARSE(app, argc, argv);

    const std::string fName_private = "private.txt";
    const std::string fName_public = "public.txt";

    qc_mdpc key(n0, p, w);
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