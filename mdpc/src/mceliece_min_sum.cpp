#include "mceliece_min_sum.h"

mceliece_min_sum::mceliece_min_sum(int n0, int p, int w, float snr, int seed)
        : mceliece(n0, p, w, 0, seed), ch(snr), decoder(code)
{
}

mceliece_min_sum::mceliece_min_sum(const BinMatrix &mdpc, float snr)
        : mceliece(mdpc, 0, 0), ch(snr), decoder(code)
{
}

std::vector<int> mceliece_min_sum::encrypt(const BinMatrix &msg)
{
    if (msg.Num_Columns() != public_key.Num_Rows())
        throw std::invalid_argument("Length of message is incorrect.\n");

    BinMatrix word = matrix_mult(msg, public_key);
    std::vector<bool> word_vec(word.Num_Columns());
    for (int i = 0; i < word_vec.size(); ++i)
        word_vec[i] = word[0][i];

    auto modulated = bpsk::modulate(word_vec);
    auto rx = ch(modulated);
    auto llr = bpsk::demodulate_llr(rx, ch.sigma);
    std::vector<int> res(llr.size());
    float scale = 10.f; // scale factor for quantization
    for (int i = 0; i < res.size(); ++i)
        res[i] = std::lround(llr[i] * scale);
    return res;
}

BinMatrix mceliece_min_sum::decrypt(const std::vector<int> &msg) const
{
    if (msg.size() != code.codeword_length()) {
        printf("Length of message is incorrect.\n");
        exit(0);
    }
    //printf("Decryption started...\n");
    auto decodeRes = decoder.decode(msg);
//    msg = msg.mat_splice(0, msg.Num_Rows() - 1, 0, code.word_length() - 1);
//    return msg;
    BinMatrix res(1, code.word_length());
    for (int i = 0; i < code.word_length(); ++i) {
        res[0][i] = decodeRes[i];
    }
    return res;
}
