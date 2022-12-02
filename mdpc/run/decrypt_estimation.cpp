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
    int error_weight;
    std::string mat_file;
    std::string file_out;
    std::string decoder_type;
    bool random;

    CLI::App app{"Key generation"};

    app.add_option("--snr", snr, "SNR for msf decoder encryption")->default_val(1.);
    app.add_option("--mat", mat_file, "Path to parity check matrix")->required();
    app.add_option("--output", file_out, "Path to output file, if skipped, standard output is used ")->default_str("std");
    app.add_option("--samples", n_samples, "Number of trials for mean decryption time estimation")->required();
    app.add_option("--decoder-type", decoder_type, "msf or hard decoder")->default_str("hard");
    app.add_option("--t", error_weight, "error weight for hard decoder")->default_val(0);
    app.add_flag("--random", random, "All-zero words if false, random if true");

    CLI11_PARSE(app, argc, argv);

    auto bin_mat = LoadMatrix(mat_file);
    std::shared_ptr<mceliece> crypt;
    if (decoder_type == "hard")
        crypt = std::make_shared<mceliece>(bin_mat, error_weight);
    else if (decoder_type == "msf")
        crypt = std::make_shared<mceliece_min_sum>(bin_mat, snr);
    MsgGenerator generator(bin_mat, snr);
    std::vector<long> times(n_samples);
    int decr_ers = 0;
    for (int i = 0; i < n_samples; ++i) {
        auto msg = random ? generator.random() : generator.zero();
        BinMatrix info(msg.information, 1, msg.information.size());
        auto ciphered = crypt->encrypt(info);

        auto start_time = std::chrono::steady_clock::now();
        auto decrypted = crypt->decrypt(ciphered);
        auto end_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        times[i] = elapsed.count();
        if (info != decrypted)
            decr_ers++;
    }

    if (file_out == "std") {
        std::cout << "Decryption failures: " << decr_ers << "\nTimes (us):" << std::endl;
        for (auto &&t: times)
            std::cout << t << std::endl;
    }
    else {
        std::ofstream out(file_out);
        out << "Decryption failures: " << decr_ers << "\nTimes (us):" << std::endl;
        for (auto &&t: times)
            out << t << std::endl;
        out.close();
    }
    return 0;
}