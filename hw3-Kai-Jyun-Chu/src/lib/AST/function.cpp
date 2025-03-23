#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
    const char* const t_name,
    std::vector<DeclNode*>* t_declaration,
    const char* const t_return_type,
    CompoundStatementNode* t_compound_state)
    : AstNode{line, col}, name(t_name), declarations(t_declaration), return_type(t_return_type), compound_statement(t_compound_state) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declarations != nullptr){
        for(auto &decl : *declarations){
            if (decl) {
                decl->accept(p_visitor); 
            }
        }
    }
    if(compound_statement != nullptr){
        compound_statement->accept(p_visitor);
    }
}
