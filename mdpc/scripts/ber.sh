#!/bin/bash

parallel ../../cmake-build-release/mdpc/mdpc_ber_est --mat ../results/private_251260.txt --samples 1000 --snr {1} ::: {2..10}