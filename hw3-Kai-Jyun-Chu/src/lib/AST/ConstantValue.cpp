#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     const char* const type, int valueInt)
    : ExpressionNode{line, col}, type(type), valueInt(valueInt),
      valueDouble(0.0), valueString(""), valueBool(false) {}


ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     const char* const type, double valueDouble)
    : ExpressionNode{line, col}, type(type), valueInt(0),
      valueDouble(valueDouble), valueString(""), valueBool(false) {}


ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     const char* const type, const char* const valueString)
    : ExpressionNode{line, col}, type(type), valueInt(0),
      valueDouble(0.0), valueString(valueString), valueBool(false) {}


ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     const char* const type, bool valueBool)
    : ExpressionNode{line, col}, type(type), valueInt(0),
      valueDouble(0.0), valueString(""), valueBool(valueBool) {}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}

