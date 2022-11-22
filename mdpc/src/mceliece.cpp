#include "../include/mceliece.h"

//Initialize the mceliece cryptosystem
mceliece::mceliece(int n0, int p, int w, int t, int seed)
        : code(n0, p, w), public_key(code.generator_matrix()), gen(seed), error_weight(t)
{
}

//Generate a random error vector of length code.n of weight = code.t
BinMatrix mceliece::get_error_vector()
{
    BinMatrix error(1, code.codeword_length());
    int curr_weight = 0;
    int idx;
    std::uniform_int_distribution<int> u_rand(1, code.codeword_length() - 1);
    while (curr_weight < error_weight) {
        idx = u_rand(gen);
        if (!error[0][idx]) {
            error[0][idx] = 1;
            curr_weight++;
        }
    }
    return error;
}

//Encrypting the message to be sent
BinMatrix mceliece::encrypt(const BinMatrix &msg)
{
    if (msg.Num_Columns() != public_key.Num_Rows()) {
        printf("Length of message is incorrect.\n");
        exit(0);
    }
    BinMatrix error = get_error_vector();
    //printf("error generated...\n");
    BinMatrix word = matrix_mult(msg, public_key) + error;
    //printf("Messsage encrypted....\n");
    return word;
}

//Decrypting the recieved message
BinMatrix mceliece::decrypt(const BinMatrix &word) const
{
    if (word.Num_Columns() != code.codeword_length()) {
        printf("Length of message is incorrect.\n");
        exit(0);
    }
    //printf("Decryption started...\n");
    BinMatrix msg = code.decode(word);
    msg = msg.mat_splice(0, msg.Num_Rows() - 1, 0, code.word_length() - 1);
    return msg;
}