#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
    ExpressionNode* expr,
    CompoundStatementNode* cs
              /* TODO: expression, compound statement */);
    ~WhileNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void print() override;

  private:
    // TODO: expression, compound statement
    ExpressionNode* expr;
    CompoundStatementNode* cs;
};

#endif
