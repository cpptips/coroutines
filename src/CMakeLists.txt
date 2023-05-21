cmake_minimum_required(VERSION 3.26)
project(test)

aux_source_directory(. SRC)
set(LIBS pthread)
add_executable(test ${SRC})
target_link_libraries (test ${LIBS})

