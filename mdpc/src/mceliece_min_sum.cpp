#include "mceliece_min_sum.h"

mceliece_min_sum::mceliece_min_sum(int n0, int p, int w, float snr, int seed)
        : code(n0, p, w), public_key(code.generator_matrix()), gen(seed), ch(snr), snr(snr)
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
    auto llr = bpsk::demodulate_llr(rx, snr);
    std::vector<int> res(llr.size());
    float scale = 10.f; // scale factor for quantization
    for (int i = 0; i < res.size(); ++i)
        res[i] = std::lround(llr[i] * scale);
    return res;
}

BinMatrix mceliece_min_sum::decrypt(const std::vector<int> &msg) const
{
    return std::vector<int>();
}
