#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "tac_generator.h"
#include <iostream>
#include <stdexcept>
#include <string>
static void require(bool ok, const std::string &message) {
    if (!ok)
        throw std::runtime_error(message);
}
int main() {
    const std::string source = "SET n TO 0\n"
                               "WHILE n < 3 DO\n"
                               "  SET n TO n + 1\n"
                               "  IF n == 2 THEN\n"
                               "    DISPLAY 20\n"
                               "  ELSE\n"
                               "    DISPLAY n\n"
                               "  ENDIF\n"
                               "ENDWHILE\n";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto ast = parser.parse();
    SemanticAnalyzer semantic;
    semantic.analyze(*ast);
    TACGenerator generator;
    auto tac = generator.generate(*ast);
    bool labels = false, jumps = false, comparisons = false;
    for (const auto &i : tac) {
        labels |= i.op == TACOp::Label;
        jumps |= i.op == TACOp::JumpIfZero;
        comparisons |= i.op == TACOp::Less || i.op == TACOp::Equal;
    }
    require(labels && jumps && comparisons, "control-flow TAC was not generated");
    CCodeGenerator codegen;
    const auto c = codegen.generate(tac);
    require(c.find("goto __pc_label") != std::string::npos, "C output has no control-flow jumps");
    bool rejected = false;
    try {
        Lexer bad("IF missing == 1 THEN\n DISPLAY 1\nENDIF\n");
        auto ts = bad.tokenize();
        Parser p(ts);
        auto a = p.parse();
        semantic.analyze(*a);
    } catch (const std::exception &e) {
        rejected = std::string(e.what()).find("line 1") != std::string::npos;
    }
    require(rejected, "undefined condition variable should include source location");
    std::cout << "Control-flow tests passed!\n";
}
