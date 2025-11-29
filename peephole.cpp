#include "peephole.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>   

// helpers 
static std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace((unsigned char)s[i])) i++;
    while (j > i && std::isspace((unsigned char)s[j-1])) j--;
    return s.substr(i, j - i);
}

static bool isAddZeroAnyReg(const std::string& line) {
    std::string t = trim(line);
    const std::string prefix = "addq $0, %";
    return t.rfind(prefix, 0) == 0;
}


static bool parseMovMemRbpToRax(const std::string& line, std::string& mem) {
    std::string t = trim(line); // movq -8(%rbp), %rax
    const std::string prefix = "movq ";
    if (t.rfind(prefix, 0) != 0) return false;

    size_t comma = t.find(',');
    if (comma == std::string::npos) return false;

    std::string lhs = trim(t.substr(prefix.size(), comma - prefix.size())); // "-8(%rbp)"
    std::string rhs = trim(t.substr(comma + 1));                            // "%rax"

    if (rhs != "%rax") return false;
    if (lhs.find("(%rbp)") == std::string::npos) return false;

    mem = lhs;
    return true;
}

static std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    return lines;
}

static std::string joinLines(const std::vector<std::string>& lines) {
    std::ostringstream oss;
    for (size_t i = 0; i < lines.size(); ++i) {
        oss << lines[i];
        if (i + 1 < lines.size()) oss << "\n";
    }
    return oss.str();
}

static bool isMovRaxToRax(const std::string& line) {
    std::string t = trim(line);
    return t == "movq %rax, %rax";
}

static bool isPushRax(const std::string& line) {
    std::string t = trim(line);
    return t == "pushq %rax";
}

static bool isPopRax(const std::string& line) {
    std::string t = trim(line);
    return t == "popq %rax";
}

static bool parseMovImmToRax(const std::string& line, std::string& imm) {
    std::string t = trim(line);
    const std::string prefix = "movq $";
    if (t.rfind(prefix, 0) != 0) return false;  

    size_t comma = t.find(',', prefix.size());
    if (comma == std::string::npos) return false;

    imm = t.substr(prefix.size(), comma - prefix.size());

    std::string rhs = trim(t.substr(comma + 1));
    if (rhs != "%rax") return false;

    return true;
}

static bool parseMovRaxToMemRbp(const std::string& line, std::string& mem) {
    std::string t = trim(line);
    const std::string prefix = "movq %rax,";
    if (t.rfind(prefix, 0) != 0) return false;

    std::string rhs = trim(t.substr(prefix.size()));
    if (rhs.empty()) return false;
    if (rhs.find("(%rbp)") == std::string::npos) return false;

    mem = rhs;
    return true;
}

std::string PeepholeOptimizer::optimize(const std::string& asmText) {
    std::vector<std::string> in = splitLines(asmText);
    std::vector<std::string> out;
    out.reserve(in.size());

    for (size_t i = 0; i < in.size(); ++i) {
        std::string line = in[i];
        std::string t = trim(line);

        // --- REGLA 1 ---
        if (i + 1 < in.size()) {
            std::string imm, mem;
            if (parseMovImmToRax(line, imm) &&
                parseMovRaxToMemRbp(in[i + 1], mem)) {

                out.push_back(" movq $" + imm + ", " + mem);
                i++;  
                continue;
            }
        }

        // --- REGLA 2 ---
        if (i + 1 < in.size()) {
            std::string mem;
            if (parseMovMemRbpToRax(line, mem) &&   // movq offset(%rbp), %rax
                isPushRax(in[i + 1])) {             // pushq %rax

                out.push_back(" pushq " + mem);
                i++;  
                continue;
            }
        }

        if (isMovRaxToRax(line)) {
            continue; 
        }

        if (isAddZeroAnyReg(line)) {
            continue; 
        }

        if (isPushRax(line) && i + 1 < in.size() && isPopRax(in[i+1])) {
            i++;      
            continue; 
        }

        // gg
        out.push_back(line);
    }

    return joinLines(out);
}

