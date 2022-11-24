#pragma once

#include <vector>

class Decoder {
public:
    Decoder() = default;
//    virtual std::vector<int> encode(const std::vector<bool>& word) = 0;
    virtual std::vector<bool> decode(const std::vector<int>& codeword) const = 0;
    virtual ~Decoder() {}
};
