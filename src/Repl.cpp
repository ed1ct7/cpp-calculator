#include "../index/Repl.h"

#include <iostream>

void Repl::run() {
    std::string line;

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            std::cout << '\n';
            return;
        }

        if (line.empty()) {
            continue;
        }

        if (line == "exit") {
            return;
        }

        if (line == "help") {
            std::cout << "help - show this message\n"
                         "exit - quit the calculator\n"
                         "anything else is treated as an expression\n";
            continue;
        }

        std::cout << process(line) << '\n';
    }
}

std::string Repl::process(const std::string &line) {
    return "not evaluated yet, got: \"" + line + "\"";
}
