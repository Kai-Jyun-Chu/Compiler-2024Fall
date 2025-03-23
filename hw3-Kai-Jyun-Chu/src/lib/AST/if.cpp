#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
    ExpressionNode* expr,
    CompoundStatementNode* cs,
    CompoundStatementNode* cselse)
    : AstNode{line, col}, expr(expr), cs(cs), cselse(cselse){}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expr!=nullptr){
        expr->accept(p_visitor);
    }
    if(cs!=nullptr){
        cs->accept(p_visitor);
    }
    if(cselse!=nullptr){
        cselse->accept(p_visitor);
    }
}
