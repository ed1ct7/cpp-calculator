cmake_minimum_required(VERSION 4.1.2)
project(CppLCClimb LANGUAGES CXX)
add_executable(calculator src/main.cpp
        index/Token.h
        index/Lexer.h
        index/Repl.h
        src/Repl.cpp)
target_compile_features(calculator PRIVATE cxx_std_23)
set_target_properties(calculator PROPERTIES CXX_EXTENSIONS OFF)
