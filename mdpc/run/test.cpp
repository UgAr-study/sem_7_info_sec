#include "../include/Matrix.hpp"
#include "../include/mceliece.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <chrono>
#include <random>

int main(int argc, char const *argv[])
{
	int n0 = 0;
    int p = 0;
    int w = 0;
    int t = 0;
    int seed = 0;
	std::cout << "Enter n0: " << "\n";
	std::scanf ("%d", &n0);
	std::cout << "Enter p: " << "\n";
	std::scanf("%d", &p);
	std::cout << "Enter w: " << "\n";
	std::scanf("%d", &w);
	std::cout << "Enter t: " << "\n";
	std::scanf("%d", &t);
	std::cout << "Enter seed: " << "\n";
	std::scanf("%d", &seed);

    if(seed == -1) {
        seed = std::time(nullptr);
    }
    std::srand(seed);

    // generate random message
    //=-----------------------
	int k = (n0 - 1) * p;
    
	BinMatrix msg (1, k);
	for (int i = 0; i < k; i++)
	{
		msg[0][i] = std::rand();
	}

	mceliece crypt (n0, p, w, t, seed);
    BinMatrix m = crypt.encrypt(msg);
    BinMatrix decrypted = crypt.decrypt(m);

	bool res = (decrypted == msg);
    std::cout << "Status: " << res << std::endl;
	
	return 0;
}