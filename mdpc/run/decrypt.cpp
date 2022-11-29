#include "../include/Matrix.hpp"
#include "../include/mceliece.h"

#include "../../common/CLI11.hpp"

#include <stdlib.h>
#include "../include/BinMatrix.h"
#include <fstream>

int main (int const argc, char const *const argv[])
{
	int n0 = 0, p = 0, w = 0, t = 0, seed = -1;
    CLI::App app{"Decrypt"};

    app.add_option("--n0", n0, "Number of circular blocks in H matrix")->required();
    app.add_option("--p", p, "Permutation size of circular block")->required();
    app.add_option("--w", w, "Row weight")->required();
    app.add_option("--t", t, "Error word weight")->required();
    app.add_option("--seed", seed, "Seed for random. If == -1, seed will be random generated.")->default_val(-1);

    std::string fName_private = "private.txt";
    app.add_option("--file-private", fName_private, "file with private key")->default_str("private.txt");

	std::string fName_cipher = "";
    app.add_option("--file-cipher", fName_cipher, "file with cipher to decrypt")->required();

	std::string fName_res = "Decrypted.txt";
    app.add_option("--file-res", fName_res, "result file")->default_str("Decrypted.txt");

    CLI11_PARSE(app, argc, argv);

	std::cout << "File with public key: " << fName_private << std::endl;


	int k = (n0 - 1) * p;
	std::cout << "Enter code of length " << k << "\n";

	std::uint8_t inp;
	BinMatrix cipher (1, k);
	for (int i = 0; i < k; i++)
	{
		std::scanf ("%hho", &inp);
		cipher[0][i] =  inp;
	}

	// read private key
	//=----------------
	std::ifstream fIn(fName_private);
	std::string header = {};
	std::getline(fIn, header);
	if(header != "Private Key:") {
		throw std::runtime_error("Header should be 'Private Key:', but is: " + header);
	}

	size_t c = 0, r = 0;
	fIn >> r >> c;
	BinMatrix private_key(r, c);
	for(size_t i = 0; i < r; ++i) {
		for(size_t k = 0; k < c; ++k) {
			int tmp = {};
			fIn >> tmp;
			private_key[i][k] = tmp;
		}
	}
	 
    mceliece crypt(n0, p, w, t, seed);
    BinMatrix m = crypt.decrypt(private_key, cipher);

	std::cout << "Dump msg to: " << fName_res << std::endl;
    std::ofstream outStream(fName_res);
    outStream << m;
    outStream.close();

	return 0;
}