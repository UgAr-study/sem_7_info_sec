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
target_include_directories(${MDPC_KEY_GEN_EXEC} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
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
target_include_directories(${MDPC_ENCRYPT_EXEC} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
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
target_include_directories(${MDPC_DECRYPT_EXEC} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
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
target_include_directories(${MDPC_TEST_EXEC} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)