#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char* name,
                                               std::vector<ExpressionNode*>* exprs)
    : ExpressionNode{line, col} , name(name), exprs(exprs){}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(exprs!=nullptr){
        for(auto &thing:*exprs){
            thing->accept(p_visitor);
        }
    }
}
