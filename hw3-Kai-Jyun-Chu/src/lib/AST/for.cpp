#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
    DeclNode* declnode,
    AssignmentNode* assnode,
    ExpressionNode* expr,
    CompoundStatementNode* cs)
    : AstNode{line, col} , declnode(declnode), assnode(assnode), expr(expr), cs(cs) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declnode!=nullptr){
        declnode->accept(p_visitor);
    }
    if(assnode!=nullptr){
        assnode->accept(p_visitor);

    }
    if(expr!=nullptr){
        expr->accept(p_visitor);
    }
    if(cs!=nullptr){
        cs->accept(p_visitor);
    }
}
