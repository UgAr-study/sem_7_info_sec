#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "../include/GF.h"

#include <armadillo>


arma::mat int2vec(unsigned int input, int length)
{
	arma::mat output = arma::zeros<arma::mat>(length,1);
	for (int i = 0; i < length; ++i)
	{
		if(((input>>i)&0x00000001) == 0x00000001)
			output(i,0) = 1;
	}
	return output;
}


void adderror(arma::mat& cipher, int weight)
{
	arma::vec temp_v = arma::randu<arma::vec>(cipher.n_rows);
	arma::uvec temp_v_e = arma::sort_index(temp_v);
	arma::mat e = arma::zeros(cipher.n_rows,1);
	for (int i = 0; i < weight; ++i)
	{
		e(temp_v_e(i),0) = 1;
	}
	cipher = cipher + e;
	for (unsigned int i = 0; i < cipher.n_rows; ++i)
	{
		cipher(i,0) = ((unsigned int)cipher(i,0))%2;
	}
}


unsigned int vec2int(arma::mat A, int length)
{
	unsigned int output = 0;
	unsigned int power = 1;
	for (int i = 0; i < length; ++i)
	{
		if(A(i,0)==1)
			output+= power;
		power*=2;
	}
	return output;
}


int main()
{
    // FIXME hardcoded
    const std::string fName_privateG = "privateG.txt";
    const std::string fName_privateS = "privateS.txt";
    const std::string fName_privateP = "privateP.txt";
    const std::string fName_public = "public.txt";
    const std::string fName_msg = "msg.txt";
    const std::string fName_cipher = "cipher.txt";
    // test plain text
    const std::string plaintext = "bbbbbbbbbbbdjsksbbbbbbbbbbbbbbbbbbbbbb";
    assert(plaintext.size() == 38); // small step to goal FIXME 

    // set seed
    //=--------
    int seed = 911;
    if(seed == -1) {
        seed = std::time(0);
    }
    std::srand(seed);
    arma::arma_rng::set_seed(seed);

    // read keys
    //=---------
    arma::mat G, S, P, Ghat;

    std::ifstream fIn(fName_privateG);
    G.load(fIn, arma::raw_ascii);
    fIn.close();

    fIn.open(fName_privateS);
    S.load(fIn, arma::raw_ascii);
    fIn.close();

    fIn.open(fName_privateP);
    P.load(fIn, arma::raw_ascii);
    fIn.close();

    fIn.open(fName_public);
    Ghat.load(fIn, arma::raw_ascii);
    fIn.close();

    // start encrypt
    //=-------------
    std::string plaintext1 = plaintext.substr(0, 19);
    std::string plaintext2 = plaintext.substr(19, 38);
    char temp = plaintext1[0];

    arma::mat blk1 = int2vec(static_cast<unsigned int>(temp), 8);
    for (int i = 1; i < 19; ++i)
    {
        temp = (char)plaintext1[i];
        blk1 = arma::join_cols(blk1, int2vec((unsigned int)temp, 8));
    }

    temp = (char)plaintext2[0];
    arma::mat blk2 = int2vec((unsigned int)temp, 8);
    for (int i = 1; i < 19; ++i)
    {
        temp = (char)plaintext2[i];
        blk2 = arma::join_cols(blk2, int2vec((unsigned int)temp, 8));
    }

    arma::mat cipher1 = arma::trans(blk1) * Ghat;
    arma::mat cipher2 = arma::trans(blk2) * Ghat;
    cipher1 = arma::trans(cipher1);
    cipher2 = arma::trans(cipher2);

    // coder
    //=-----
    adderror(cipher1, 13);
    adderror(cipher2, 13);
    arma::mat cipher = arma::join_cols(cipher1, cipher2);

    unsigned int ciphertext[64];
    for (int i = 0; i < 64; ++i)
        ciphertext[i] = vec2int(cipher(arma::span(8 * i, 8 * i + 7), 0), 8);

    // save result
    //=-----------
    std::ofstream fOut(fName_cipher);
    for (int i = 0; i < 64; ++i)
        fOut << ciphertext[i] << " ";
    fOut.close();
    
    return 0;
}