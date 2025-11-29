#include "peephole.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>   // para isspace

// helpers chiquitos
static std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace((unsigned char)s[i])) i++;
    while (j > i && std::isspace((unsigned char)s[j-1])) j--;
    return s.substr(i, j - i);
}

static bool isAddZeroRax(const std::string& line) {
    return trim(line) == "addq $0, %rax";
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

// regla: coincide con "movq %rax, %rax" ignorando espacios extra
static bool isMovRaxToRax(const std::string& line) {
    std::string t = trim(line);
    return t == "movq %rax, %rax";
}

// reglas push/pop %rax
static bool isPushRax(const std::string& line) {
    std::string t = trim(line);
    return t == "pushq %rax";
}

static bool isPopRax(const std::string& line) {
    std::string t = trim(line);
    return t == "popq %rax";
}

// --- NUEVO: detectar patrón movq $IMM, %rax ---
static bool parseMovImmToRax(const std::string& line, std::string& imm) {
    std::string t = trim(line);
    // esperamos algo como: movq $1, %rax
    const std::string prefix = "movq $";
    if (t.rfind(prefix, 0) != 0) return false;  // no empieza con "movq $"

    size_t comma = t.find(',', prefix.size());
    if (comma == std::string::npos) return false;

    // entre "$" y "," está el inmediato
    imm = t.substr(prefix.size(), comma - prefix.size());

    std::string rhs = trim(t.substr(comma + 1));
    if (rhs != "%rax") return false;

    return true;
}

// --- NUEVO: detectar patrón movq %rax, OFFSET(%rbp) ---
static bool parseMovRaxToMemRbp(const std::string& line, std::string& mem) {
    std::string t = trim(line);
    // esperamos algo como: movq %rax, -8(%rbp)
    const std::string prefix = "movq %rax,";
    if (t.rfind(prefix, 0) != 0) return false;

    std::string rhs = trim(t.substr(prefix.size()));
    // rhs debería ser algo tipo "-8(%rbp)"
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

        // --- REGLA NUEVA ---
        // movq $IMM, %rax
        // movq %rax, OFFSET(%rbp)
        //  ==> movq $IMM, OFFSET(%rbp)
        if (i + 1 < in.size()) {
            std::string imm, mem;
            if (parseMovImmToRax(line, imm) &&
                parseMovRaxToMemRbp(in[i + 1], mem)) {

                // Generamos la versión combinada (con un espacio inicial para alinearse con tu estilo)
                out.push_back(" movq $" + imm + ", " + mem);
                i++;  // saltamos la siguiente línea porque ya la consumimos
                continue;
            }
        }

        // 1) eliminar "movq %rax, %rax"
        if (isMovRaxToRax(line)) {
            continue; // no lo copiamos a 'out'
        }

        // 1.5) eliminar addq $0, %rax
        if (isAddZeroRax(line)) {
            continue; // no hace nada, la eliminamos
        }

        // 2) eliminar patrón pushq %rax ; popq %rax (consecutivos)
        if (isPushRax(line) && i + 1 < in.size() && isPopRax(in[i+1])) {
            i++;      // saltamos también la siguiente línea
            continue; // no agregamos ninguna de las dos
        }

        // Si no matcheó ninguna regla, copiamos tal cual
        out.push_back(line);
    }

    return joinLines(out);
}
