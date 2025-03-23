#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H
#include "visitor/AstNodeVisitor.hpp"
#include "AST/expression.hpp"
#include <string>
#include <variant>

class ConstantValueNode : public ExpressionNode {
  public:
  
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      const char* const type, int valueInt);

    
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      const char* const type, double valueDouble);

    
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      const char* const type, const char* const valueString);

    
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      const char* const type, bool valueBool);
    ~ConstantValueNode() = default;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    

    const char* getType(){return type.c_str();}
    int getInt(){return valueInt;}
    double getDouble(){return valueDouble;}
    const char* getString(){return valueString.c_str();}
    bool getBool(){return valueBool;}

    void print() override;

  private:
    // TODO: constant value
    std::string type;
    int valueInt;
    double valueDouble;
    std::string valueString;
    bool valueBool;

};

#endif
