#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/assignment.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
    DeclNode* declnode,
    AssignmentNode* assnode,
    ExpressionNode* expr,
    CompoundStatementNode* cs
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void print() override;

  private:
    // TODO: declaration, assignment, expression, compound statement
    DeclNode* declnode;
    AssignmentNode* assnode;
    ExpressionNode* expr;
    CompoundStatementNode* cs;
};

#endif
