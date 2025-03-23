#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, std::vector<VariableNode*>* p_vars, std::string p_type,
                    std::vector<int>* p_dim
                    )
    : AstNode{line, col}, variables(p_vars), type(p_type), dims(p_dim){}

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, 
            std::vector<VariableNode*>* p_vars,
            std::string p_type,
            ConstantValueNode* p_constant)
    : AstNode{line, col}, variables(p_vars), type(p_type), dims(nullptr){}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

// void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // printf("DeclNode::visitChildNodes\n");
    if(variables != nullptr){
        for(auto &var : *variables){
            if (var) {
                var->accept(p_visitor); 
            }
        }
    }
}
