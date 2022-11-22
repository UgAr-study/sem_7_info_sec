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
target_include_directories(${KEY_GEN_EXEC} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/mdpc/include
)

add_executable(
        encrypt
        mdpc/run/encrypt.cpp
        mdpc/src/mceliece.cpp
        mdpc/src/qc_mdpc.cpp
        mdpc/src/BinMatrix.cpp
        mdpc/src/min_sum_decoder.cpp
        mdpc/src/mceliece_min_sum.cpp)
target_include_directories(encrypt PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/mdpc/include
)

add_executable(
        test
        mdpc/test/test.cpp
        mdpc/src/mceliece.cpp
        mdpc/src/channel.cpp
        mdpc/src/qc_mdpc.cpp
        mdpc/src/BinMatrix.cpp
        mdpc/src/min_sum_decoder.cpp
        mdpc/src/mceliece_min_sum.cpp)
target_include_directories(test PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/mdpc/include
)