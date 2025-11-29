#include <iostream>
#include <fstream>
#include <string>
#include <sstream>      

#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"
#include "peephole.h"
#include "dag.h"   

using namespace std;

int main(int argc, const char* argv[]) {
    // Verificar número de argumentos
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo en un string
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear instancias de Scanner 
    Scanner scanner1(input.c_str());

    // Crear instancias de Parser
    Parser parser(&scanner1);

    // Parsear y generar AST
  
    Program* program = parser.parseProgram(); 

    DAGOptimizer dagOpt;
    dagOpt.optimize(program);

    string inputFile(argv[1]);
    size_t dotPos = inputFile.find_last_of('.');
    string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
    string outputFilename = baseName + "-w" + ".s";

    ofstream outfile(outputFilename);
    if (!outfile.is_open()) {
        cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
        return 1;
    }

    cout << "Generando codigo ensamblador en " << outputFilename << endl;
    GenCodeVisitor codigo(outfile);
    codigo.generar(program);
    outfile.close();
    
    return 0;
}
 

/*
int main(int argc, const char* argv[]) {
    // Verificar número de argumentos
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo en un string
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear instancias de Scanner 
    Scanner scanner1(input.c_str());

    // Crear instancias de Parser
    Parser parser(&scanner1);

    // Parsear y generar AST
    Program* program = parser.parseProgram();

    // DAGOptimizer dagOpt;
    // dagOpt.optimize(program);

    // Nombre de archivo de salida
    string inputFile(argv[1]);
    size_t dotPos = inputFile.find_last_of('.');
    string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
    string outputFilename = baseName + "-w" + ".s";

    cout << "Generando codigo ensamblador en " << outputFilename << endl;

    // 1) Generar ASM crudo en un buffer en memoria
    std::ostringstream asmBuffer;
    GenCodeVisitor codigo(asmBuffer);
    codigo.generar(program);

    // 2) Pasar mirilla (peephole) sobre el texto generado
    std::string asmRaw = asmBuffer.str();
    std::string asmOpt = PeepholeOptimizer::optimize(asmRaw);

    cout << "===== ASM RAW =====\n";
    cout << asmRaw << "\n";
    cout << "===== ASM OPT =====\n";
    cout << asmOpt << "\n";


    // 3) Escribir ASM optimizado al archivo .s
    ofstream outfile(outputFilename);
    if (!outfile.is_open()) {
        cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
        return 1;
    }
    outfile << asmOpt;
    outfile.close();
    
    return 0;
}
*/
