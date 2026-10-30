#ifndef AST_H
#define AST_H
#include <memory>
#include <string>
#include <vector>
struct ASTNode {
    std::string type;
    std::string value;
    std::vector<std::unique_ptr<ASTNode>> children;
    int line = 0;
    int column = 0;
    ASTNode(const std::string &nodeType, const std::string &nodeValue = "", int nodeLine = 0,
            int nodeColumn = 0)
        : type(nodeType), value(nodeValue), line(nodeLine), column(nodeColumn) {}
};
#endif
