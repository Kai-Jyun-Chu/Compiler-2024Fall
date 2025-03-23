#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, 
    const uint32_t col,
    const char* op,
    ExpressionNode* l,
    ExpressionNode* r
    
                       /* TODO: operator, expressions */);
    ~BinaryOperatorNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void print() override;
    const char* getOp(){return op.c_str();}
  private:
    // TODO: operator, expressions
    std::string op;
    ExpressionNode* l;
    ExpressionNode* r;
};

#endif
