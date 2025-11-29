#ifndef PEEPHOLE_H
#define PEEPHOLE_H

#include <string>

using namespace std;

class PeepholeOptimizer {
public:
    static string optimize(const string& asmText);
};

#endif
