#pragma once

#include "decoder.h"
#include "qc_mdpc.h"
#include <vector>

class MinSumDecoder : public Decoder {
public:
    MinSumDecoder(const qc_mdpc& qc_mdpc, int maxItNum = 32);

    virtual std::vector<int> encode(const std::vector<int>& word);
    virtual std::vector<int> decode(const std::vector<int>& codeword);

private:
    std::vector<std::vector<int>> R_msgs; // var * check
    std::vector<std::vector<int>> Q_msgs; // check * var
    qc_mdpc qcMdpc;
    int maxItNum;
};