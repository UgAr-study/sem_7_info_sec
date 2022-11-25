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

    int rowWeight = qcMdpc.row_weight(0);

    std::vector<std::vector<int>> R_msgs(qcMdpc.word_length(), std::vector<int>(rowWeight)); // Check2Var
    std::vector<std::vector<int>> Q_msgs(qcMdpc.word_length(), std::vector<int>(rowWeight)); // Var2Check

    int itNum = 0;

    BinMatrix H = qcMdpc.parity_check_matrix();
    std::cout << H << std::endl;
    BinMatrix x(in_llrs.size(), 1);

    while (itNum < maxItNum) {
        std::vector<int> sums = in_llrs;
        for (int check = 0; check < qcMdpc.word_length(); ++check) {
            for (int varId = 0; varId < rowWeight; ++varId) {
                sums[qcMdpc.adjacent_var_node(check, varId)] += R_msgs[check][varId];
            }
        }

        for (int var = 0; var < qcMdpc.codeword_length(); ++var) {
            x[var][0] = (sums[var] >= 0) ? 1 : 0;
        }

        if (matrix_mult(H, x).is_zero_matrix()) {
            break;
        }

        // Vertical
        for (int check = 0; check < qcMdpc.word_length(); ++check) {
            for (int varId = 0; varId < rowWeight; ++varId) {
              Q_msgs[check][varId] = sums[qcMdpc.adjacent_var_node(check, varId)] - R_msgs[check][varId];
            }
        }

        // Horizontal
        for (int check = 0; check < qcMdpc.word_length(); ++check) {
//            int minVal1 = std::abs(Q_msgs[check][0]);
            std::vector<std::pair<int, int>> minVal(2);
            minVal[0] = std::make_pair(0, std::abs(Q_msgs[check][0]));
            minVal[1] = std::make_pair(1, std::abs(Q_msgs[check][1]));
            if (minVal[0].second > minVal[1].second) {
                std::swap(minVal[0], minVal[1]);
            }
            int sign = Q_msgs[check][0] >= 0 ? 1 : -1;
            sign *= Q_msgs[check][1] >= 0 ? 1 : -1;

            for (int varId = 2; varId < rowWeight; ++varId) {
                if (std::abs(Q_msgs[check][varId]) < minVal[1].second) {
                    if (std::abs(Q_msgs[check][varId]) > minVal[0].second) {
                        minVal[1] = std::make_pair(varId, std::abs(Q_msgs[check][varId]));
                    } else {
                        minVal[0] = std::make_pair(varId, std::abs(Q_msgs[check][varId]));
                    }
                }
                sign *= Q_msgs[check][varId] >= 0 ? 1 : -1;
            }
            for (int varId = 0; varId < rowWeight; ++varId) {
                if (minVal[0].first != varId) {
                    R_msgs[check][varId] = minVal[0].second * sign / (Q_msgs[check][varId] >= 0 ? 1 : -1);
                } else {
                    R_msgs[check][varId] = minVal[1].second * sign / (Q_msgs[check][varId] >= 0 ? 1 : -1);
                }
            }
        }
        itNum++;
    }
    std::vector<bool> res(x.Num_Rows());
    for (int i = 0; i < res.size(); ++i) {
        res[i] = x[i][0];
    }
    return res;
}
