cmake_minimum_required(VERSION 3.19)

project(spinhalf_chain_structure_factor)

find_package(xdiag REQUIRED HINTS "../../install")
add_executable(main main.cpp)
target_link_libraries(main PUBLIC xdiag::xdiag)
