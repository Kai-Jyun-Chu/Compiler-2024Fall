#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
    const char* name
                          /* TODO: name */);
    // array reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
    const char* name,
    std::vector<ExpressionNode*>* exprs
                           /* TODO: name, expressions */);
    ~VariableReferenceNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    const char* getName(){return name.c_str();}
    void print() override;

  private:
    // TODO: variable name, expressions
    std::string name;
    std::vector<ExpressionNode*>* exprs;
};

#endif
