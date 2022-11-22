#pragma once

#include <random>
#include <vector>


class McElieceGoppa final {
public:
    McElieceGoppa();
    
public:
    std::vector<char> encrypt(const std::vector<char> &msg);
    std::vector<char> decrypt(const std::vector<char> &msg);
    
private:
    std::vector<char> get_error_vector();
};