#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
    ExpressionNode* expr,
    CompoundStatementNode* cs)
    : AstNode{line, col}, expr(expr), cs(cs) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr!=nullptr){
        expr->accept(p_visitor);
    }
    if(cs!=nullptr){
        cs->accept(p_visitor);
    }
}
