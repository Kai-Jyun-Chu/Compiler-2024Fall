#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(
    const uint32_t line, 
    const uint32_t col,
    const char* const p_name,
    const char* const p_type,
    ConstantValueNode* p_const,
    std::vector<int>* p_dims
     )
    : AstNode{line, col}, 
    name(p_name), 
    type(p_type ? p_type : ""), 
    constValue(p_const), 
    dims(p_dims)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if(constValue != nullptr)
        constValue->accept(p_visitor);
}
