#include "min_sum_decoder.hpp"
#include <cassert>

//std::vector<int> MinSumDecoder::encode(const std::vector<bool>& word) {
//    // TODO
//    BinMatrix vec(word, word.size(), 1);
//    auto codeword = qcMdpc.encode(vec);
//
//    std::vector<bool> codeword_vec(codeword.Num_Rows());
//    for (int i = 0; i < codeword_vec.size(); ++i)
//        codeword_vec[i] = codeword[i][0];
//    auto modulated = bpsk::modulate(codeword_vec);
//    auto rx = ch(modulated);
//}

std::vector<bool> MinSumDecoder::decode(const std::vector<int>& in_llrs) const {
    // TODO

    std::vector<std::vector<int>> R_msgs(qcMdpc.word_length()); // Check2Var
    for (int i = 0; i < R_msgs.size(); ++i) {
           R_msgs[i] = std::vector<int>(qcMdpc.row_weight(i));
    }
    std::vector<std::vector<int>> Q_msgs(qcMdpc.codeword_length()); // Var2Check
    for (int i = 0; i < Q_msgs.size(); ++i) {
        Q_msgs[i] = std::vector<int>(qcMdpc.col_weight(i));
    }

    int itNum = 0;

    BinMatrix H = qcMdpc.parity_check_matrix();
    std::cout << H << std::endl;
    BinMatrix x(in_llrs.size(), 1);

    while (itNum < maxItNum) {
        std::vector<int> sums = in_llrs;
        for (int checkPos = 0; checkPos < R_msgs.size(); ++checkPos) {
            for (int varId = 0; varId < R_msgs[checkPos].size(); ++varId) {
                sums[qcMdpc.adjacent_var_node(checkPos, varId)] += R_msgs[checkPos][varId];
            }
        }
        for (int varPos = 0; varPos < qcMdpc.codeword_length(); ++varPos) {
            x[varPos][0] = (sums[varPos] >= 0) ? 1 : 0;
        }


        if (matrix_mult(H, x).is_zero_matrix()) {
            break;
        }

        // Vertical
        for (int varPos = 0; varPos < qcMdpc.codeword_length(); ++varPos) {
            for (int checkId = 0; checkId < qcMdpc.col_weight(varPos); ++checkId) {
                int varId = -1;
                for (int var_id = 0; var_id < qcMdpc.col_weight(varPos); ++var_id) {
                   if (varPos == qcMdpc.adjacent_check_node(varPos, checkId)) {
                       varId = var_id;
                       break;
                   }
                }
                assert(varId >= 0);
                Q_msgs[varPos][checkId] = sums[varPos] -
                        R_msgs[qcMdpc.adjacent_check_node(varPos, checkId)][varId];
            }
        }
        // Horizontal
        for (int check_id = 0; check_id < qcMdpc.word_length(); ++check_id) {

        }
        itNum++;
    }
    std::vector<bool> res(x.Num_Rows());
    for (int i = 0; i < res.size(); ++i) {
        res[i] = x[i][0];
    }
    return res;
}
