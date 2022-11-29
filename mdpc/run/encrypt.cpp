#include "../include/Matrix.hpp"
#include "../include/mceliece.h"

#include "../../common/CLI11.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <fstream>

int main(int argc, char const *argv[])
{
	int n0 = 0, p = 0, w = 0, t = 0, seed = -1;
    CLI::App app{"Encrypt"};

    app.add_option("--n0", n0, "Number of circular blocks in H matrix")->required();
    app.add_option("--p", p, "Permutation size of circular block")->required();
    app.add_option("--w", w, "Row weight")->required();
    app.add_option("--t", t, "Error word weight")->required();
    app.add_option("--seed", seed, "Seed for random. If == -1, seed will be random generated.")->default_val(-1);

    std::string fName_public = "public.txt";
    app.add_option("--file-public", fName_public, "file with public key")->default_str("public.txt");

	std::string fName_msg = "";
    app.add_option("--file-msg", fName_msg, "file with msg to encrypt. file should contain msg in format: 0 0 1 0 ...")->required();

	std::string fName_res = "Encrypted.txt";
    app.add_option("--file-res", fName_res, "file wfor dump cipher")->default_str("Encrypted.txt");

    CLI11_PARSE(app, argc, argv);

	std::cout << "File with public key: " << fName_public << std::endl;


	std::ifstream fIn(fName_msg);
	int k = (n0 - 1) * p;
	BinMatrix msg (1, k);
	for (int i = 0; i < k; i++)
	{
		fIn >> msg[0][i];
	}

	// read public key
	//=---------------
	fIn.open(fName_public);
	std::string header = {};
	std::getline(fIn, header);
	if(header != "Public Key:") {
		throw std::runtime_error("Header should be 'Public Key:', but is: " + header);
	}

	size_t c = 0, r = 0;
	fIn >> r >> c;
	BinMatrix public_key(r, c);
	for(size_t i = 0; i < r; ++i) {
		for(size_t k = 0; k < c; ++k) {
			int tmp = {};
			fIn >> tmp;
			public_key[i][k] = tmp;
		}
	}

	mceliece crypt (n0, p, w, t, seed);
    BinMatrix m = crypt.encrypt(public_key, msg);

	std::cout << "Cipher dump to: " << fName_res << std::endl;
	std::ofstream outStream(fName_res);
    outStream << "Encryption message: \n";
	outStream << m;
    outStream.close();
	
	return 0;
}