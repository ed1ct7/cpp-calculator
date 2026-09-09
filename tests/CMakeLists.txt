# Один бинарник на все юнит-тесты. Новый файл теста - добавить сюда строку.
add_executable(unit_tests
        LexerTest.cpp
)

# calc_core   - код, который тестируем (даёт и заголовки из index/)
# GTest::gtest_main - сам фреймворк + готовый main(), свой писать не нужно
target_link_libraries(unit_tests PRIVATE calc_core GTest::gtest_main)
set_target_properties(unit_tests PROPERTIES CXX_EXTENSIONS OFF)

# Регистрирует каждый TEST() в ctest как отдельный тест.
include(GoogleTest)
gtest_discover_tests(unit_tests)
