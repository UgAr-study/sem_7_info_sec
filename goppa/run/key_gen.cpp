#include <iostream>
#include <fstream>
#include <string>

#include "../include/GF.h"

#include <armadillo>

unsigned int G_z(unsigned int alpha, unsigned int P_x, unsigned int g_z[],int degree)
{
	unsigned output = 0;
	unsigned power = 1;
	for (int i = 0; i < degree+1; ++i)
	{
		output ^= galois_mul(power,g_z[i],P_x);
		power = galois_mul(power,alpha,P_x);

	}
	return output;
}


arma::mat MyMatrixMul(arma::mat A, arma::mat B, unsigned int p_x)
{
	arma::mat C = arma::zeros(A.n_rows,B.n_cols);
	unsigned int temp;
	for (unsigned int i = 0; i < C.n_rows; ++i)
	{
		for (unsigned int j = 0; j < C.n_cols; ++j)
		{
			temp = 0;
			for (unsigned int k = 0; k < A.n_cols; ++k)
			{
				temp^= galois_mul((unsigned int)A(i,k),(unsigned int)B(k,j),p_x);
			}
			C(i,j) = temp;
		}
	}
	return C;
}


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


arma::mat JoinMatrix(arma::mat h,int length)
//wirte H in 0 1 form
{	
	if(h.n_cols == 1)
	{
		arma::mat temp = int2vec(h(0,0),length);
		for (unsigned int i = 1; i < h.n_rows; ++i)
		{
			temp = arma::join_cols(temp,int2vec(h(i,0),length));
		}
		return temp;
	}
	else
	{
		arma::mat mycol = int2vec(h(0,0),length);
		for (unsigned int i = 1; i < h.n_rows; ++i)
		{
			mycol = arma::join_cols(mycol,int2vec(h(i,0),length));
		}

		return arma::join_rows(mycol,JoinMatrix(h(arma::span(0,h.n_rows - 1),arma::span(1,h.n_cols-1)),length));
	}
}



arma::mat findnullspace(const arma::mat &H)
{
    arma::mat temp(H.n_cols, H.n_cols);
    temp.eye();
    // join 2 matrices
    temp = arma::join_vert(H, temp);
    // rearrage columns
    for(unsigned int i = 0; i < H.n_rows; i++){
        unsigned int j = i;
        while(!temp(i, j)&&(j < H.n_cols)) j++;
        arma::vec temp_v = temp.col(i);
        temp.col(i) = temp.col(j);
        temp.col(j) = temp_v;
        // elimination
        for(j = 0; j < H.n_cols; j++){
            if(temp(i, j)&&(i != j)){
                for(unsigned int k = 0; k < temp.n_rows; k++){
                    temp(k, j) = ((unsigned int)temp (k, j) + (unsigned int)temp (k, i))%2;
                }
            }
        }
    }
    unsigned int max_j = 0;
    for(unsigned int i = 0; i < H.n_rows; i++){
        for(unsigned int j = 0; j < H.n_cols; j++){
            if(temp(i, j)&&(j > max_j)) max_j = j;
        }
    }
    // get generator matrix
    arma::mat G_t = temp(arma::span(H.n_rows, temp.n_rows - 1), arma::span(max_j + 1, H.n_cols - 1));
    return G_t.t();
}


unsigned int g_z[14]={53,100,17,229,248,45,120,152,113,131,133,197,103,129};	


int main() 
{
    const std::string fName_privateG = "privateG.txt";
    const std::string fName_privateS = "privateS.txt";
    const std::string fName_privateP = "privateP.txt";
    const std::string fName_matH = "matH.txt";
    const std::string fName_public = "public.txt";

    // set seed
    //=--------
    int seed = 911;
    if(seed == -1) {
        seed = std::time(0);
    }
    std::srand(seed);
    arma::arma_rng::set_seed(seed);


    unsigned int p_x = 0x0000012b;
	int N = 256;
	unsigned int a[256]={0};
	for (int i = 1; i < 256; ++i)
	{
		a[i] = galois_pow(2,i-1,p_x);
	}

	int degree = 13;
	
	for (int i = 1; i < N; ++i)	
		a[i] = galois_pow(2,i-1,p_x);	

	arma::mat Y = arma::zeros(N,N);
	arma::mat C = arma::zeros(degree,degree);
	arma::mat X = arma::zeros(degree,N);

	for (int i = 0; i < N; ++i)
		Y(i,i) = inverse(G_z(a[i],p_x,g_z,degree),p_x);


	for (int i = 0; i < degree; ++i)
	{
		for (int j = i; j < degree; ++j)
		{
			C(i,j) = g_z[degree + i - j];
		}
	}

	
	for (int i = 0; i < degree; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			X(i,j) = galois_pow(a[j], degree - i - 1, p_x);
		}
	}

	arma::mat temp = MyMatrixMul(C,MyMatrixMul(X,Y,p_x),p_x);

	arma::mat H = JoinMatrix(temp, 8);
	arma::mat G = findnullspace(H);

   	arma::mat S = arma::zeros(G.n_rows,G.n_rows);
	arma::vec temp_v = arma::randu<arma::vec>(G.n_rows);
    arma::uvec temp_v_p = arma::sort_index(temp_v);
    for(unsigned int i = 0; i < G.n_rows; i++) 
    	S(i, temp_v_p(i)) = 1;

   	arma::mat P = arma::zeros(G.n_cols,G.n_cols);
	temp_v = arma::randu<arma::vec>(G.n_cols);
    temp_v_p = sort_index(temp_v);
    for(unsigned int i = 0; i < G.n_cols; i++) 
    	P(i, temp_v_p(i)) = 1;

	arma::mat Ghat = S*G*P;
	for (unsigned int i = 0; i < Ghat.n_rows; ++i)
	{
		for (unsigned int j = 0; j < Ghat.n_cols; ++j)
			Ghat(i,j)=((unsigned int)Ghat(i,j))%2;
	}

    // save keys
    //=---------
    std::ofstream fOut(fName_public);
    Ghat.save(fOut, arma::raw_ascii);
    fOut.close();

    fOut.open(fName_privateG);
    G.save(fOut, arma::raw_ascii);
    fOut.close();

    fOut.open(fName_privateS);
    S.save(fOut, arma::raw_ascii);
    fOut.close();

    fOut.open(fName_privateP);
    P.save(fOut, arma::raw_ascii);
    fOut.close();

    fOut.open(fName_matH);
    H.save(fOut, arma::raw_ascii);
    fOut.close();

    return 0;
}