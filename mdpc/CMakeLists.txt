cmake_minimum_required(VERSION 3.10)


project(mdpc)

# key_gen
#=-------
set(MDPC_KEY_GEN_EXEC mdpc_key_gen)
add_executable(
        ${MDPC_KEY_GEN_EXEC}
        run/key_gen.cpp
        src/qc_mdpc.cpp
        src/BinMatrix.cpp
)

# encrypt
#=-------
set(MDPC_ENCRYPT_EXEC mdpc_encrypt)
add_executable(
        ${MDPC_ENCRYPT_EXEC}
        run/encrypt.cpp
        src/qc_mdpc.cpp
        src/BinMatrix.cpp
        src/mceliece.cpp
)

# decrypt
#=-------
set(MDPC_DECRYPT_EXEC mdpc_decrypt)
add_executable(
        ${MDPC_DECRYPT_EXEC}
        run/decrypt.cpp
        src/qc_mdpc.cpp
        src/BinMatrix.cpp
        src/mceliece.cpp
)

# test
#=----
set(MDPC_TEST_EXEC mdpc_test)
add_executable(
        ${MDPC_TEST_EXEC}
        run/test.cpp
        src/qc_mdpc.cpp
        src/BinMatrix.cpp
        src/mceliece.cpp
)