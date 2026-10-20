#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

class SemanticAnalyzer {
public:
    void analyze(const ASTNode& program);
};

#endif
