#include "../include/mceliece.h"

//Initialize the mceliece cryptosystem
mceliece::mceliece(int n0, int p, int w, int t, int seed)
        : code(n0, p, w, seed), public_key(code.generator_matrix()), gen(seed), error_weight(t)
{
}

mceliece::mceliece(const BinMatrix &mdpc, int t, int seed)
        : code(mdpc), public_key(code.generator_matrix()), error_weight(t), gen(seed)
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
std::vector<int> mceliece::encrypt(const BinMatrix &msg)
{
    if (msg.Num_Columns() != public_key.Num_Rows()) {
        printf("Length of message is incorrect: expected %d\n", public_key.Num_Rows());
        exit(0);
    }
    BinMatrix error = get_error_vector();
    //printf("error generated...\n");
    BinMatrix word = matrix_mult(msg, public_key) + error;
    //printf("Messsage encrypted....\n");
    std::vector<int> res(word.Num_Columns());
    for (int i = 0; i < res.size(); ++i)
        res[i] = word[0][i];
    return res;
}

//Encrypt with castom public key
BinMatrix mceliece::encrypt(const BinMatrix &public_key_, const BinMatrix &msg)
{
    if (msg.Num_Columns() != public_key_.Num_Rows()) {
        printf("Length of message is incorrect: expected %d\n", public_key_.Num_Rows());
        exit(0);
    }
    BinMatrix error = get_error_vector();
    BinMatrix word = matrix_mult(msg, public_key_) + error;
    return word;
}

//Decrypting the recieved message
BinMatrix mceliece::decrypt(const std::vector<int> &encrypted) const
{
    std::vector<bool> encr(encrypted.size());
    for (int i = 0; i < encr.size(); ++i)
        encr[i] = encrypted[i];
    BinMatrix word(encr, 1, encr.size());
    if (word.Num_Columns() != code.codeword_length()) {
        throw std::logic_error("Length of message is incorrect.\n Expected: "
                               + std::to_string(code.codeword_length()));
    }
    //printf("Decryption started...\n");
    BinMatrix msg = code.decode(word);
    msg = msg.mat_splice(0, msg.Num_Rows() - 1, 0, code.word_length() - 1);
    return msg;
}

BinMatrix mceliece::decrypt(const BinMatrix &private_key, const BinMatrix &cipher)
{
    BinMatrix msg = code.decode(private_key, cipher);
    msg = msg.mat_splice(0, msg.Num_Rows() - 1, 0, code.word_length() - 1);
    return msg;
}