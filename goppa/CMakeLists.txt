cmake_minimum_required(VERSION 3.10)


project(goppa)

# key_gen
#=-------
set(KEY_GEN_EXEC goppa_key_gen)
add_executable(
        ${KEY_GEN_EXEC}
        run/key_gen.cpp
        src/GF.cpp
        
)

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})
target_link_libraries(${KEY_GEN_EXEC} ${ARMADILLO_LIBRARIES})
