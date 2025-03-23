#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector<DeclNode*>* decl,
                                             std::vector<AstNode*>* state
                                             )
    : AstNode{line, col}, declarations(decl), statement(state) {
        
    }

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declarations != nullptr){
        for(auto &decl : *declarations){
            if (decl) {
                decl->accept(p_visitor); 
            }
        }
    }
    
    if(statement != nullptr){
        for(auto &sta : *statement){
            if (sta) {
                sta->accept(p_visitor); 
            }
        }
    }
}
