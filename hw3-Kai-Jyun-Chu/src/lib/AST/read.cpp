#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, VariableReferenceNode* varnode)
    : AstNode{line, col}, varnode(varnode) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(varnode!=nullptr){
        varnode->accept(p_visitor);
    }
}
