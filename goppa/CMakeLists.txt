cmake_minimum_required(VERSION 3.10)


project(goppa)

find_package(Armadillo REQUIRED)


# key_gen
#=-------
set(KEY_GEN_EXEC goppa_key_gen)
add_executable(
        ${KEY_GEN_EXEC}
        run/key_gen.cpp
        src/GF.cpp

)
target_include_directories(${KEY_GEN_EXEC} PUBLIC ${ARMADILLO_INCLUDE_DIRS})
target_link_libraries(${KEY_GEN_EXEC} ${ARMADILLO_LIBRARIES})


# encrypt
#=-------
set(ENCRYPT_EXEC goppa_encrypt)
add_executable(
        ${ENCRYPT_EXEC}
        run/encrypt.cpp
        src/GF.cpp
)
target_include_directories(${ENCRYPT_EXEC} PUBLIC ${ARMADILLO_INCLUDE_DIRS})
target_link_libraries(${ENCRYPT_EXEC} ${ARMADILLO_LIBRARIES})
