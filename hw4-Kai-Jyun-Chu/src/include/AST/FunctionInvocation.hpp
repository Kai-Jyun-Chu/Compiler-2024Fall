#ifndef AST_FUNCTION_INVOCATION_NODE_H
#define AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>
#include <string>
#include <vector>

class FunctionInvocationNode final : public ExpressionNode {
  public:
    using ExprNodes = std::vector<std::unique_ptr<ExpressionNode>>;

  private:
    std::string m_name;
    ExprNodes m_args;

  public:
    ~FunctionInvocationNode() = default;
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           const char *const p_name, ExprNodes &p_args)
        : ExpressionNode{line, col}, m_name(p_name), m_args(std::move(p_args)){}

    const char *getNameCString() const { return m_name.c_str(); }
    int getArgsNum(){
      return m_args.size();
    }
    const ExprNodes &getExpressNode() const { return m_args; }

    std::vector<std::string> getArgString(){
      std::vector<std::string> ans;
      for(auto &thing:m_args){
        ans.push_back(thing->getType()->getPTypeCString());
      }
      return ans;
    }
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
