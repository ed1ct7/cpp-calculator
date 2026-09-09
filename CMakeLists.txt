cmake_minimum_required(VERSION 4.1.2)
project(CppLCClimb LANGUAGES CXX)

option(CPPLCCLIMB_BUILD_TESTS "Build unit tests" ON)

# ---------------------------------------------------------------------------
# calc_core - вся логика проекта, без main().
# И приложение, и тесты линкуются с этой библиотекой.
# ---------------------------------------------------------------------------
add_library(calc_core
        src/Lexer.cpp
        src/Repl.cpp
        index/Token.h
        index/Lexer.h
        index/Repl.h
)
target_include_directories(calc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/index)
target_compile_features(calc_core PUBLIC cxx_std_23)
set_target_properties(calc_core PROPERTIES CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# calculator - точка входа, только main().
# ---------------------------------------------------------------------------
add_executable(calculator src/main.cpp)
target_link_libraries(calculator PRIVATE calc_core)
set_target_properties(calculator PROPERTIES CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Тесты: GoogleTest скачивается при конфигурации, ставить руками не нужно.
# ---------------------------------------------------------------------------
if (CPPLCCLIMB_BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.15.2
            GIT_SHALLOW TRUE
    )
    # На MSVC библиотека и тесты должны использовать одну и ту же runtime.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(tests)
endif ()
