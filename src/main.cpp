#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "codegen.h"
#include "lexer.h"
#include "optimizer.h"
#include "parser.h"
#include "semantic.h"
#include "tac_generator.h"

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        std::cerr
            << "Usage: plaincode <source-file.plain> [output-file.c]\n";
        return 1;
    }

    const std::string sourcePath = argv[1];
    const std::string outputPath =
        (argc == 3) ? argv[2] : "output.c";

    std::ifstream input(sourcePath);

    if (!input) {
        std::cerr << "Error: could not open source file '"
                  << sourcePath << "'\n";
        return 1;
    }

    const std::string source(
        (std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>()
    );

    try {
        // Stage 1: Lexical analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // Stage 2: Parsing and AST construction
        Parser parser(tokens);
        std::unique_ptr<ASTNode> ast = parser.parse();

        // Stage 3: Semantic analysis
        SemanticAnalyzer semanticAnalyzer;
        semanticAnalyzer.analyze(*ast);

        // Stage 4: TAC generation
        TACGenerator tacGenerator;
        TACProgram tac = tacGenerator.generate(*ast);

        // Stage 5: TAC optimization
        Optimizer optimizer;
        TACProgram optimizedTac = optimizer.optimize(tac);

        // Stage 6: C code generation
        CCodeGenerator codeGenerator;
        std::string generatedC =
            codeGenerator.generate(optimizedTac);

        // Stage 7: Write generated C code
        std::ofstream output(outputPath);

        if (!output) {
            std::cerr << "Error: could not create output file '"
                      << outputPath << "'\n";
            return 1;
        }

        output << generatedC;

        if (!output) {
            std::cerr << "Error: failed to write output file '"
                      << outputPath << "'\n";
            return 1;
        }

        output.close();

        if (!output) {
            std::cerr << "Error: failed to close output file '"
                      << outputPath << "'\n";
            return 1;
        }

        std::cout << "Compilation successful!\n";
        std::cout << "Generated C file: "
                  << outputPath << '\n';

    } catch (const std::exception& error) {
        std::cerr << "Compilation failed: "
                  << error.what() << '\n';
        return 1;
    }

    return 0;
}
