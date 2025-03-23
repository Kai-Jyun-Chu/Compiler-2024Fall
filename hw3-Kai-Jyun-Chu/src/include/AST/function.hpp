#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, 
    const uint32_t col,
    const char* const t_name,
    std::vector<DeclNode*>* t_declaration,
    const char* const t_return_type,
    CompoundStatementNode* t_compound_state
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */);
    ~FunctionNode() = default;
    void visitChildNodes(AstNodeVisitor &p_visitor)  override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    const char* getNameCString(){ return name.c_str();}
    const char* getALLType(){ 
      static std::string ans;  
      ans.clear();
      ans += return_type;
      ans += " (";
      if (declarations != nullptr && !declarations->empty()){
          for (size_t i=0; i < (*declarations).size();i++){
              DeclNode* declnode=(*declarations)[i];
              if(declnode){
                for(auto& var : *declnode->getVariables()) {
                  ans += var->getType() + ", ";
                }
              }
              

              
          }
          ans.pop_back();
          ans.pop_back();
      }

    ans += ")";  

    return ans.c_str();  
      
    }

    void print() override;

  private:
    // TODO: name, declarations, return type, compound statement
    std::string name;
    std::vector<DeclNode*>* declarations;
    std::string return_type;
    CompoundStatementNode* compound_statement;
};

#endif
