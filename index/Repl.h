#ifndef CPPLCCLIMB_REPL_H
#define CPPLCCLIMB_REPL_H

#include <string>

class Repl {
public:
    static void run();

private:
    static std::string process(const std::string &line);
};

#endif //CPPLCCLIMB_REPL_H
