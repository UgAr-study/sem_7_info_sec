#include "../include/min_sum_decoder.hpp"
#include <cassert>

std::vector<int> MinSumDecoder::encode(const std::vector<bool>& word) {
    // TODO
    BinMatrix vec(word, word.size(), 1);
    auto codeword = qcMdpc.encode(vec);

    std::vector<int> encoded(codeword.Num_Rows());
    // map 0, 1 to 1, -1
    for (int i = 0; i < encoded.size(); ++i)
        encoded[i] = 1 - 2 * codeword[i][0];

}

std::vector<bool> MinSumDecoder::decode(const std::vector<int>& in_llrs) {
    // TODO
    int itNum = 0;

    BinMatrix H = qcMdpc.parity_check_matrix();
    BinMatrix x(in_llrs.size(), 1);

    while (itNum < maxItNum) {
        for (int var_id = 0; var_id < qcMdpc.codeword_length(); ++var_id) {
            int sum = std::accumulate(R_msgs[var_id].begin(), R_msgs[var_id].end(), int());
            x[var_id][0] = (sum > 0) ? 1 : 0;
        }
        if (matrix_mult(H, x).is_zero_matrix()) {
            break;
        }

        // Vertical
        for (int var_id = 0; var_id < qcMdpc.codeword_length(); ++var_id) {
            int globalSum = in_llrs[var_id] + std::accumulate(R_msgs[var_id].begin(), R_msgs[var_id].end(), int());
            int subMsg = 0;
            for (const auto &check_id: qcMdpc.adjacent_check_nodes(var_id)) {
                int index = *std::find(qcMdpc.adjacent_var_nodes(check_id).begin(),
                                      qcMdpc.adjacent_var_nodes(check_id).end(), var_id);
                Q_msgs[check_id][index] = globalSum - R_msgs[subMsg][check_id];
                subMsg++;
            }
        }
        // Horizontal
        for (int check_id = 0; check_id < qcMdpc.word_length(); ++check_id) {
            int minVal = *std::min_element(Q_msgs[check_id].begin(), Q_msgs[check_id].end());
            int sign = 1;
            for (const auto& var_id : qcMdpc.adjacent_var_nodes(check_id)) {
                sign *= Q_msgs[check_id][var_id];
                if (Q_msgs[check_id][var_id] < 0) {
                    sign *= -1;
                } else if (Q_msgs[check_id][var_id] > 0) {
                    sign *= 1;
                } else {
                    sign *= 0;
                    break;
                }
            }
            int subMsg = 0;
            for (const auto& var_id : qcMdpc.adjacent_var_nodes(check_id)) {
                int index = *std::find(qcMdpc.adjacent_check_nodes(var_id).begin(),
                                       qcMdpc.adjacent_check_nodes(var_id).end(), check_id);
                R_msgs[var_id][index] = minVal * sign / (Q_msgs[var_id][subMsg]);
                subMsg++;
            }
        }
        itNum++;
    }
    auto decoded_word = qcMdpc.decode(x);
    std::vector<bool> res(decoded_word.Num_Rows());
    for (int i = 0; i < res.size(); ++i)
        res[i] = decoded_word[i][0];
    return res;
}
