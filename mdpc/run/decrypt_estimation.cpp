#include "mceliece_min_sum.h"
#include "matrix_storage.hpp"
#include "min_sum_decoder.hpp"
#include "msg_generator.h"
#include "CLI11.hpp"
#include <chrono>

int main (int argc, char* argv[])
{
    float snr;
    int n_samples;
    std::string mat_file;
    std::string file_out;
    bool random;

    CLI::App app{"Key generation"};

    app.add_option("--snr", snr, "SNR for encryption")->required();
    app.add_option("--mat", mat_file, "Path to parity check matrix")->required();
    app.add_option("--output", mat_file, "Path to output file, if skipped, standard output is used ")->default_str("std");
    app.add_option("--samples", n_samples, "Number of trials for mean decryption time estimation")->required();
    app.add_flag("--random", random, "All-zero words if false, random if true");

    CLI11_PARSE(app, argc, argv);

    auto bin_mat = LoadMatrix(mat_file);
    mceliece_min_sum crypt(bin_mat, snr);
    MsgGenerator generator(bin_mat, snr);
    std::vector<long> times(n_samples);
    int decr_ers = 0;
    for (int i = 0; i < n_samples; ++i) {
        auto msg = random ? generator.random() : generator.zero();
        BinMatrix info(msg.information, 1, msg.information.size());
        auto ciphered = crypt.encrypt(info);

        auto start_time = std::chrono::steady_clock::now();
        auto decrypted = crypt.decrypt(ciphered);
        auto end_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        times[i] = elapsed.count();
        if (info != decrypted)
            decr_ers++;
    }

    if (file_out == "std") {
        std::cout << "Decryption failures: " << decr_ers << "\nTimes:" << std::endl;
        for (auto &&t: times)
            std::cout << t << std::endl;
    }
    else {
        std::ofstream out(file_out);
        out << "Decryption failures: " << decr_ers << "\nTimes:" << std::endl;
        for (auto &&t: times)
            out << t << std::endl;
        out.close();
    }
    return 0;
}