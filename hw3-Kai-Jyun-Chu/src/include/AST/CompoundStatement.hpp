#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"
class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          /* TODO: declarations, statements */
                          std::vector<DeclNode*>* decl, 
                          std::vector<AstNode*>* stat
                          );
    ~CompoundStatementNode() = default;
    //
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    //
    void print() override;

  private:
    // TODO: declarations, statements
    std::vector<DeclNode*>* declarations;
    std::vector<AstNode*>* statement;
    
};

#endif
