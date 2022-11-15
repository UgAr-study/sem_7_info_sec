#pragma once

#include <vector>

class Decoder {
public:
    Decoder() = default;
    virtual std::vector<int> encode(const std::vector<int>& word) = 0;
    virtual std::vector<int> decode(const std::vector<int>& codeword) = 0;
    virtual ~Decoder() {}
};
