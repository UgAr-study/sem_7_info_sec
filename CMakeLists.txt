cmake_minimum_required(VERSION 3.10)


project(mdpc)

# key_gen
#=-------
set(KEY_GEN_EXEC key_gen)
add_executable(${KEY_GEN_EXEC} key_gen.cpp qc_mdpc.cpp BinMatrix.cpp)
