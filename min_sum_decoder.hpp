#pragma once

#include "decoder.h"
#include "qc_mdpc.h"
#include <utility>
#include <vector>

class MinSumDecoder : public Decoder
{
public:
    MinSumDecoder(qc_mdpc qcMdpc, int maxItNum = 32)
            : qcMdpc(std::move(qcMdpc)), maxItNum(maxItNum) {};

    virtual std::vector<int> encode(const std::vector<int> &word);
    virtual std::vector<int> decode(const std::vector<int> &codeword);

private:
    std::vector<std::vector<int>> R_msgs; // var * check
    std::vector<std::vector<int>> Q_msgs; // check * var
    qc_mdpc qcMdpc;
    int maxItNum;
};