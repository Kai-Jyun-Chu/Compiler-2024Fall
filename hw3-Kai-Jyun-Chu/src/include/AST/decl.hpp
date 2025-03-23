#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include <memory>
#include <vector>
#include <string>
class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, 
            const uint32_t col, 
            std::vector<VariableNode*> *p_vars,
            std::string p_type,
            std::vector<int> *p_dims
            );

    // constant variable declaration
    DeclNode(const uint32_t line, 
            const uint32_t col, 
            std::vector<VariableNode*> *p_vars,
            std::string p_type,
            ConstantValueNode* p_constant
            );
          
    ~DeclNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void print() override;
    std::vector<VariableNode*>* getVariables() const { return variables; }
    const char* getType(){ return type.c_str();}

  private:
    // TODO: variables
    
    std::vector<VariableNode*>* variables;
    std::string type;
    std::vector<int>* dims;
};

#endif
