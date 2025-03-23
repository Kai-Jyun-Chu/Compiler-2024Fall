#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <vector>
#include <string>
class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                const char* const p_name,
                const char*  p_type,
                ConstantValueNode* p_const,
                std::vector<int>* p_dims
                 /* TODO: variable name, type, constant value */);
    ~VariableNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    const char *getNameCString() const { return name.c_str(); }
    std::string getType() const {
      std::string outtype;
      if(dims != nullptr && dims -> size() != 0){
        
        outtype = type + " ";
        for(auto i: *dims){
          outtype += "[" + std::to_string(i) + "]";
        }
      }else{
        outtype = type;
      } 
      
      return outtype;
      }
  private:
    // TODO: variable name, type, constant value
    std::string name;
    std::string type;
    ConstantValueNode* constValue;
    std::vector<int>* dims;
};

#endif
