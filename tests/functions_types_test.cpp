#include "codegen.h"
#include "lexer.h"
#include "optimizer.h"
#include "parser.h"
#include "semantic.h"
#include "tac_generator.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

static std::string compile(const std::string &source) {
    Lexer lexer(source);
    Parser parser(lexer.tokenize());
    auto ast = parser.parse();
    SemanticAnalyzer semantic;
    semantic.analyze(*ast);
    TACGenerator generator;
    Optimizer optimizer;
    CCodeGenerator codegen;
    return codegen.generate(optimizer.optimize(generator.generate(*ast)));
}

int main() {
    const std::string source = "FUNCTION add(a, b)\n"
                               "    RETURN a + b\n"
                               "ENDFUNCTION\n"
                               "SET answer TO add(12, 30)\n"
                               "SET price TO 19.95\n"
                               "SET greeting TO \"hello\"\n"
                               "DISPLAY answer\n"
                               "DISPLAY price\n"
                               "DISPLAY greeting\n";
    const std::string c = compile(source);
    assert(c.find("double add(double a, double b)") != std::string::npos);
    assert(c.find("answer = __pc_tmp") != std::string::npos);
    assert(c.find("double price") != std::string::npos);
    assert(c.find("const char* greeting") != std::string::npos);
    assert(c.find("printf(\"%s\\n\", greeting)") != std::string::npos);

    bool rejected = false;
    try {
        (void)compile("SET x TO \"text\"\nSET x TO 3\n");
    } catch (const std::runtime_error &e) {
        rejected = std::string(e.what()).find("cannot change variable type") != std::string::npos;
    }
    assert(rejected);

    rejected = false;
    try {
        (void)compile("SET x TO missing(1)\n");
    } catch (const std::runtime_error &e) {
        rejected = std::string(e.what()).find("undefined function") != std::string::npos;
    }
    assert(rejected);

    rejected = false;
    try {
        (void)compile("FUNCTION bad(a)\n RETURN a\nENDFUNCTION\nSET x TO bad()\n");
    } catch (const std::runtime_error &e) {
        rejected = std::string(e.what()).find("expects 1 argument") != std::string::npos;
    }
    assert(rejected);

    std::cout << "Function and data-type tests passed!\n";
    return 0;
}
