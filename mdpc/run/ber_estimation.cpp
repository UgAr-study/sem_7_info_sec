#include "matrix_storage.hpp"
#include "min_sum_decoder.hpp"
#include "msg_generator.h"
#include "CLI11.hpp"

int main(int argc, char *argv[])
{
    float snr;
    int n_samples;
    std::string mat_file;
    bool random;
    std::string outFile;

    CLI::App app{"Key generation"};

    app.add_option("--snr", snr, "SNR for which BER estimation is conducted")->required();
    app.add_option("--mat", mat_file, "Path to parity check matrix")->required();
    app.add_option("--samples", n_samples, "Number of trials for Monte Carlo estimation")->required();
    app.add_flag("--random", random, "All-zero words if false, random if true");
    app.add_option("--outFile", outFile, "Kek");

    CLI11_PARSE(app, argc, argv);

    qc_mdpc mdpc = LoadMatrix(mat_file);
    MinSumDecoder decoder(mdpc);
    MsgGenerator generator(mdpc, snr);
    int bit_ers = 0;
    for (int i = 0; i < n_samples; ++i) {
        auto msg = random ? generator.random() : generator.zero();
        auto decoded = decoder.decode(msg.llr);
        for (int j = 0; j < msg.information.size(); ++j)
            bit_ers += (msg.information[j] != decoded[j]);
    }

    auto snr_str = std::to_string(snr);
    std::ofstream file(outFile);
//    file << "SNR\tErrors\tTotal\n";
    file << snr << "\t" << bit_ers << "\t" << n_samples * mdpc.word_length() << "\n";
    file.close();
//    std::cout << "SNR " + snr_str + " done\n";
    return 0;
}
