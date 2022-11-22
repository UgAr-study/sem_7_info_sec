#pragma once

#include "decoder.h"
#include "qc_mdpc.h"
#include "channel.h"
#include <utility>
#include <vector>

class MinSumDecoder : public Decoder
{
public:
    MinSumDecoder(const qc_mdpc& qcMdpc, int maxItNum = 32)
            : qcMdpc(qcMdpc), maxItNum(maxItNum) {};

    virtual std::vector<bool> decode(const std::vector<int> &codeword) const;

private:
    qc_mdpc qcMdpc;
    int maxItNum;
};