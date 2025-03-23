#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
const char* op, ExpressionNode* exp)
    : ExpressionNode{line, col}, op(op), exp(exp) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(exp!=nullptr){
        exp->accept(p_visitor);
    }
}
