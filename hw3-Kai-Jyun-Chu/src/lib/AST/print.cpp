#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, 
const uint32_t col,
ExpressionNode* expressNode
)
    : AstNode{line, col}, expressNode(expressNode) {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(expressNode!=nullptr){
        expressNode->accept(p_visitor);
    }
 }
