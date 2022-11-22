#pragma once

#include "decoder.h"
#include "qc_mdpc.h"
#include "channel.h"
#include <utility>
#include <vector>

class MinSumDecoder : public Decoder
{
public:
    MinSumDecoder(qc_mdpc qcMdpc, float snr, int maxItNum = 32)
            : qcMdpc(std::move(qcMdpc)), ch(snr), maxItNum(maxItNum) {};

    virtual std::vector<int> encode(const std::vector<bool> &word);
    virtual std::vector<bool> decode(const std::vector<int> &codeword);

private:
    std::vector<std::vector<int>> R_msgs; // var * check
    std::vector<std::vector<int>> Q_msgs; // check * var
    qc_mdpc qcMdpc;
    int maxItNum;
    awgn ch;
};