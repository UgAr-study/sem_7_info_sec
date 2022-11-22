cmake_minimum_required(VERSION 3.10)


project(mdpc)

# key_gen
#=-------
set(KEY_GEN_EXEC key_gen)
add_executable(
        ${KEY_GEN_EXEC}
        mdpc/run/key_gen.cpp
        mdpc/src/qc_mdpc.cpp
        mdpc/src/BinMatrix.cpp
        mdpc/src/min_sum_decoder.cpp
        mdpc/src/channel.cpp
)
