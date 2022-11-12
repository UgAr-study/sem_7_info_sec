#include "Matrix.hpp"
#include "mceliece.h"
#include <stdlib.h>
#include <stdio.h>


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

	int k = (n0 - 1) * p;
	std::cout << "Enter Message of length " << k << "\n";
	std::uint8_t inp;

	BinMatrix msg (1, k);
	for (int i = 0; i < k; i++)
	{
		std::scanf ("%hho", &inp);
		msg[0][i] = inp;
	}

	mceleice crypt (n0, p, w, t, seed);
    BinMatrix m = crypt.encrypt (msg);

    outStream.open ("Encryption.txt");
    outStream << "Encryption message: \n";

    for (int i = 0; i < m->cols; i++)
    {
        outStream << m[0][i] << " ";
    }
    outStream.close ();
	
	return 0;
}