#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"
#include "qc_mdpc.h"
#include "mceliece.h"
#include "utility.h"
#include <time.h>

//Initialize the mceliece cryptosystem
mceliece::mceliece(int n0, int p, int w, int t) : code(n0, p, w, t), public_key(generator_matrix(code))
{
}

//Generate a random error vector of length code.n of weight = code.t
bin_matrix mceliece::get_error_vector() const
{
    bin_matrix error(1, code.n);
    int curr_weight = 0;
    int idx;
    std::uniform_int_distribution<int> u_rand(1, code.n - 1);
    while (curr_weight < code.t) {
        idx = u_rand(gen);
        if (!get_matrix_element(error, 0, idx)) {
            set_matrix_element(error, 0, idx, 1);
            curr_weight++;
        }
    }
    return error;
}

//Encrypting the message to be sent
bin_matrix mceliece::encrypt(const bin_matrix &msg) const
{
    if (msg.cols != public_key.rows) {
        printf("Length of message is incorrect.\n");
        exit(0);
    }
    bin_matrix error = get_error_vector();
    //printf("error generated...\n");
    bin_matrix word = add_matrix(matrix_mult(msg, public_key), error);
    //printf("Messsage encrypted....\n");
    return word;
}

//Decrypting the recieved message
bin_matrix mceliece::decrypt(const bin_matrix &word) const
{
    if (word.cols != code.n) {
        printf("Length of message is incorrect.\n");
        exit(0);
    }
    //printf("Decryption started...\n");
    bin_matrix msg = code.decode(word);
    msg = mat_splice(msg, 0, msg->rows - 1, 0, code.k - 1);
    return msg;
}