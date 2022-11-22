#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "../include/GF.h"
#include "../include/Euclidean.h"

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


arma::mat decrypt_one(arma::mat H, arma::mat G, arma::mat S, arma::mat P, arma::mat ciphertext, const Poly_t g_z, const Field_t GF){
    Poly_t z;
    z.degree = 1;
    z.coefficient[0] = 0;
    z.coefficient[1] = 1;

    arma::mat error_codeword = ciphertext*P.t();
    arma::mat syndrome = H*error_codeword.t();
    for(unsigned int i = 0; i < H.n_rows; i++) syndrome(i, 0) = ((int)syndrome(i, 0))%2;

    // express syndrome s(z) as an array of coefficients of a polynomial of degree degree
    Poly_t s_z;
    s_z.degree = g_z.degree - 1;
    for(unsigned i = 0; i < s_z.degree + 1; i++) s_z.coefficient[i] = vec2int(syndrome.rows(8*i, 8*(i + 1) - 1), 8);
    while(s_z.coefficient[s_z.degree] == 0){
        s_z.degree--;
    }

    // calculate sigma(z)
    Poly_t h_z = Euclidean_inv(s_z, g_z, GF);
    Poly_t d_2_z = Euclidean_add_p(h_z, z, GF);
    for(unsigned i = 0; i < 8*g_z.degree - 1; i++){
        d_2_z = Euclidean_pow(d_2_z, g_z, 2, GF);
    }
    Poly_t d_z = d_2_z;

    Poly_t a_z;
    Poly_t b_z;
    Poly_t d_i_z = Euclidean_inv(d_z, g_z, GF);
    if(g_z.degree%2){
        if(d_i_z.degree == (g_z.degree - 1)/2){
            a_z.degree = 0;
            a_z.coefficient[0] = 1;
            b_z = d_i_z;
        }else if(d_i_z.degree < (g_z.degree - 1)/2){
            a_z = Euclidean_pow(z, g_z, (g_z.degree - 1)/2 - d_i_z.degree, GF);
            b_z = Euclidean_mult_pp(d_i_z, a_z, GF);
        }else{
            Poly_t r_1, r_0, u_1, u_0, v_1, v_0;
            u_1.degree = 0;
            u_1.coefficient[0] = 1;
            u_0.degree = 0;
            u_0.coefficient[0] = 0;
            v_1.degree = 0;
            v_1.coefficient[0] = 0;
            v_0.degree = 0;
            v_0.coefficient[0] = 1;
            r_1 = g_z;
            r_0 = d_i_z;
            while((r_0.degree != 0)||(r_0.coefficient[0] != 0)){
                Poly_t quotient = Euclidean_div_pp(r_1, r_0, GF);

                Poly_t remainder = Euclidean_add_p(r_1, Euclidean_mult_pp(r_0, quotient, GF), GF);
                Poly_t temp_u = Euclidean_add_p(u_1, Euclidean_mult_pp(u_0, quotient, GF), GF);
                Poly_t temp_v = Euclidean_add_p(v_1, Euclidean_mult_pp(v_0, quotient, GF), GF);
                r_1 = r_0;
                r_0 = remainder;
                u_1 = u_0;
                u_0 = temp_u;
                v_1 = v_0;
                v_0 = temp_v;
                if(r_1.degree == (g_z.degree - 1)/2){
                    b_z = r_1;
                    a_z = Euclidean_modp(Euclidean_mult_pp(d_z, b_z, GF), g_z, GF);
                    break;
                }
            }
        }
    }else{
        if(d_z.degree == g_z.degree/2){
            b_z.degree = 0;
            b_z.coefficient[0] = 1;
            a_z = d_z;
        }else if(d_z.degree < g_z.degree/2){
            b_z = Euclidean_pow(z, g_z, g_z.degree/2 - d_z.degree, GF);
            a_z = Euclidean_mult_pp(d_z, b_z, GF);
        }else{
            Poly_t r_1, r_0, u_1, u_0, v_1, v_0;
            u_1.degree = 0;
            u_1.coefficient[0] = 1;
            u_0.degree = 0;
            u_0.coefficient[0] = 0;
            v_1.degree = 0;
            v_1.coefficient[0] = 0;
            v_0.degree = 0;
            v_0.coefficient[0] = 1;
            r_1 = g_z;
            r_0 = d_z;
            while((r_0.degree != 0)||(r_0.coefficient[0] != 0)){
                Poly_t quotient = Euclidean_div_pp(r_1, r_0, GF);

                Poly_t remainder = Euclidean_add_p(r_1, Euclidean_mult_pp(r_0, quotient, GF), GF);
                Poly_t temp_u = Euclidean_add_p(u_1, Euclidean_mult_pp(u_0, quotient, GF), GF);
                Poly_t temp_v = Euclidean_add_p(v_1, Euclidean_mult_pp(v_0, quotient, GF), GF);
                r_1 = r_0;
                r_0 = remainder;
                u_1 = u_0;
                u_0 = temp_u;
                v_1 = v_0;
                v_0 = temp_v;
                if(r_1.degree == g_z.degree/2){
                    a_z = r_1;
                    b_z = Euclidean_modp(Euclidean_mult_pp(d_i_z, b_z, GF), g_z, GF);
                    break;
                }
            }
        }
    }

    // get the location of errors
    Poly_t sigma_z = Euclidean_add_p(Euclidean_mult_pp(a_z, a_z, GF), Euclidean_mult_pp(Euclidean_mult_pp(b_z, b_z, GF), z, GF), GF);
    arma::mat codeword = error_codeword;
    for(unsigned i = 0; i < GF.max_ele; i++){
        unsigned sum = 0;
        unsigned multiplier = 1;
        for(unsigned j = 0; j < sigma_z.degree + 1; j++){
            sum = Euclidean_add_c(sum, Euclidean_mult_cc(multiplier, sigma_z.coefficient[j], GF));
            multiplier = Euclidean_mult_cc(multiplier, GF.gen[i], GF);
        }
        if(sum == 0){
            codeword(0, i) = ((unsigned)codeword(0, i) + 1)%2;
        }
    }

    // get m
    arma::mat temp_g = arma::join_horiz(G.t(), codeword.t());
    for(unsigned int j = 0; j < G.n_rows; j++){
        unsigned int i = j;
        while(!temp_g(i, j)&&(i < G.n_cols)) i++;
        temp_g.swap_rows(i, j);
        // elimination
        for(i = 0; i < G.n_cols; i++){
            if(temp_g(i, j)&&(i != j)){
                for(unsigned int k = 0; k < G.n_rows + 1; k++){
                    temp_g(i, k) = ((unsigned int)temp_g (i, k) + (unsigned int)temp_g (j, k))%2;
                }
            }
        }
    }
    arma::mat retrieve = temp_g(arma::span(0, G.n_rows - 1), arma::span(G.n_rows, G.n_rows)).t()*S.t();

    for(unsigned i = 0; i < retrieve.n_cols; i++){
        retrieve(0, i) = ((unsigned)retrieve(0, i))%2;
    }
    return retrieve;
}


const unsigned int g_z[14]={53,100,17,229,248,45,120,152,113,131,133,197,103,129};	

int main()
{
    // FIXME hardcoded
    const std::string fName_privateG = "privateG.txt";
    const std::string fName_privateS = "privateS.txt";
    const std::string fName_privateP = "privateP.txt";
    const std::string fName_public = "public.txt";
    const std::string fName_matH = "matH.txt";
    const std::string fName_msg = "msg.txt";
    const std::string fName_cipher = "cipher.txt";
    const std::string fName_decrypted = "decrypted.txt";

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
	arma::mat G, S, P, Ghat, H;

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

    fIn.open(fName_matH);
    H.load(fIn, arma::raw_ascii);
    fIn.close();

    // read cipher text
    //=----------------
    fIn.open(fName_cipher);
    unsigned int part1[32] = {0}, part2[32]= {0};
	for (int i = 0; i < 32; ++i)
		fIn >> part1[i];
	for (int i = 0; i < 32; ++i)
		fIn >> part2[i];
    fIn.close();

    // start decrypt
    //=-------------
    arma::mat blk1 = int2vec(part1[0],8);
	for (int i = 1; i < 32; ++i)
		blk1 = arma::join_cols(blk1, int2vec(part1[i],8));
	blk1 = arma::trans(blk1);

	arma::mat blk2 = int2vec(part2[0],8);
	for (int i = 1; i < 32; ++i)
		blk2 = arma::join_cols(blk2, int2vec(part2[i],8));
	blk2 = arma::trans(blk2);

	// decode
    //=------
	Field_t GF;
	GF.P_x = 0453;
	GF.max_ele = 256;
	GF.gen[0] = 0;
	GF.gen_inv[0] = 0;
   	GF.gen[1] = 1;
  	GF.gen_inv[1] = 1;
	for(unsigned i = 2; i < GF.max_ele; i++){
        GF.gen[i] = galois_mul(GF.gen[i - 1], 2, GF.P_x);
        GF.gen_inv[GF.gen[i]] = i;
   	}

   	Poly_t GZ;
   	GZ.degree = 13;
   	for (int i = 0; i <= 13; ++i)
   		GZ.coefficient[i] = g_z[i];

	arma::mat plain1 = decrypt_one(H, G, S, P, blk1, GZ, GF);
	arma::mat plain2 = decrypt_one(H, G, S, P, blk2, GZ, GF);

	for (int i = 0; i < 152; ++i)
	{
		plain1(0,i) = ((unsigned int)plain1(0,i))%2;
		plain2(0,i) = ((unsigned int)plain2(0,i))%2;
	}
	plain2 = arma::trans(plain2);
	plain1 = arma::trans(plain1);

	std::string text1 = "";
	std::string text2 = "";
	unsigned int temp;
	for (int i = 0; i < 19; ++i)
	{
		temp = vec2int(plain1(arma::span(i*8,i*8+7),0),8);
		text1 = text1 + (char)temp;
		temp = vec2int(plain2(arma::span(i*8,i*8+7),0),8);
		text2 = text2 + (char)temp;
	}


    // save result
    //=-----------
	std::string text = text1+text2;
	std::cout << "decryption done!" << std::endl;
	std::cout << text << std::endl;

    //std::ofstream fOut(fName_decrypted);
    //for (int i = 0; i < 64; ++i)
    //    fOut << ciphertext[i] << " ";
    //fOut.close();
    
    return 0;
}