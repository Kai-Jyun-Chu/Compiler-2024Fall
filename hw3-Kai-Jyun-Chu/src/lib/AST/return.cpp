#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
ExpressionNode* expr)
    : AstNode{line, col} , expr(expr){}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr!=nullptr){
        expr->accept(p_visitor);
    }
}
