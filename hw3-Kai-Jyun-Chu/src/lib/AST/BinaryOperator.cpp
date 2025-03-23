#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, 
const uint32_t col,
const char* op,
ExpressionNode* l,
ExpressionNode* r
)
    : ExpressionNode{line, col}, op(op),l(l), r(r) {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(l!=nullptr){
        l->accept(p_visitor);
    }
    if(r!=nullptr){
        r->accept(p_visitor);
    }
}
