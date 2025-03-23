#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char* name)
    : ExpressionNode{line, col} ,name(name), exprs(nullptr){}

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char* name,
                                             std::vector<ExpressionNode*>* exprs)
    : ExpressionNode{line, col} , name(name), exprs(exprs){}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(exprs!=nullptr){
        for(auto &thing:*exprs){
            thing->accept(p_visitor);
        }
    }
}
