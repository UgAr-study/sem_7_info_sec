#include "msg_generator.h"

Message MsgGenerator::zero()
{
    std::vector<bool> info(code.word_length(), false);
    return from_channel(info);
}

Message MsgGenerator::random()
{
    std::vector<bool> info(code.word_length());
    for (auto && i : info)
        i = distr(rng);
    return from_channel(info);
}

Message MsgGenerator::from_channel(const std::vector<bool>& info)
{
    Message res;
    res.information = info;
    auto encoded = code.encode(BinMatrix(info, 1, info.size()));
    std::vector<bool> encoded_vec(encoded.Num_Columns());
    for (int i = 0; i < info.size(); ++i)
        encoded_vec[i] = encoded[0][i];
    auto modulated = bpsk::modulate(encoded_vec);
    res.rx = ch(modulated);
    auto llr = bpsk::demodulate_llr(res.rx, ch.sigma);
    res.llr.resize(llr.size());
    float scale = 10.f; // scale factor for quantization
    for (int i = 0; i < llr.size(); ++i)
        res.llr[i] = std::lround(llr[i] * scale);
    return res;
}
