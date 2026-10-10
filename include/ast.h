
#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>

struct ASTNode {
    std::string type;
    std::string value;
    std::vector<std::unique_ptr<ASTNode>> children;

    ASTNode(const std::string& type,
            const std::string& value = "")
        : type(type), value(value) {}
};

#endif
