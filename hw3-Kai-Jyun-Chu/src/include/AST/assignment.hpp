#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/VariableReference.hpp"
#include "visitor/AstNodeVisitor.hpp"
class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
    VariableReferenceNode* varnode,
    ExpressionNode* expr
                   /* TODO: variable reference, expression */);
    ~AssignmentNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void print() override;

  private:
    // TODO: variable reference, expression
    VariableReferenceNode* varnode;
    ExpressionNode* expr;
};

#endif
