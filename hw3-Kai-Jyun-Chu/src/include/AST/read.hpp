#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/VariableReference.hpp"


class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col,
    VariableReferenceNode* varnode
             /* TODO: variable reference */);
    ~ReadNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void print() override;

  private:
    // TODO: variable reference
    VariableReferenceNode* varnode;
};

#endif
