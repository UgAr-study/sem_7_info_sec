#include "mceliece_min_sum.h"
#include "matrix_storage.hpp"
#include "CLI11.hpp"

int main(int argc, char *argv[])
{
    float snr;
    int n_samples;
    std::string mat_file;

    CLI::App app{"Key generation"};

    app.add_option("--snr", snr, "SNR for which BER estimation is conducted")->required();
    app.add_option("--mat", mat_file, "Path to parity check matrix")->required();
    app.add_option("--samples", n_samples, "Number of trials for Monte Carlo estimation")->required();

    CLI11_PARSE(app, argc, argv);

    auto mdpc = LoadMatrix(mat_file);
    mceliece_min_sum mceliece();
}
