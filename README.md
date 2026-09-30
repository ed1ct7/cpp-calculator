# cpp-calculator

A console calculator with a hand-written lexer and an interactive REPL,
written in C++23.

- `index/Token.h`, `src/Lexer.cpp` — tokenizer for arithmetic expressions
- `src/Repl.cpp` — interactive read-eval-print loop
- `src/main.cpp` — entry point
- `tests/LexerTest.cpp` — GoogleTest unit tests (GoogleTest is downloaded by
  CMake FetchContent at configure time, no manual install needed)

All logic lives in the `calc_core` library (`src/` + `index/`); both the
application and the tests link against it, so the REPL has no hidden logic of
its own.

## Build

```bash
cmake -B build -DCPPLCCLIMB_BUILD_TESTS=ON
cmake --build build

./build/calculator   # REPL
ctest --test-dir build
```

Tests can be disabled with `-DCPPLCCLIMB_BUILD_TESTS=OFF`.
