#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
    VariableReferenceNode* varnode,
    ExpressionNode* expr)
    : AstNode{line, col} ,varnode(varnode), expr(expr){}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(varnode!=nullptr){
        varnode->accept(p_visitor);
        
    }
    if(expr!=nullptr){
        expr->accept(p_visitor);
    }
}
