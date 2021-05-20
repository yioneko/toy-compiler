cmake_minimum_required(VERSION 3.0.0)
project(toy-compiler VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 11)

include(CTest)
enable_testing()
include_directories(src)
add_executable(toy-compiler src/main.cpp src/tokens.cpp src/lexer.cpp src/symbols.cpp src/gram.cpp src/parser.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
