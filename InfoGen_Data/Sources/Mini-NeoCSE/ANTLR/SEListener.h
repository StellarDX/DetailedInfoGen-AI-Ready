
// Generated from SE.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SEParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by SEParser.
 */
class  SEListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterTable(SEParser::TableContext *ctx) = 0;
  virtual void exitTable(SEParser::TableContext *ctx) = 0;

  virtual void enterKey(SEParser::KeyContext *ctx) = 0;
  virtual void exitKey(SEParser::KeyContext *ctx) = 0;

  virtual void enterValueGroup(SEParser::ValueGroupContext *ctx) = 0;
  virtual void exitValueGroup(SEParser::ValueGroupContext *ctx) = 0;

  virtual void enterBoolOp(SEParser::BoolOpContext *ctx) = 0;
  virtual void exitBoolOp(SEParser::BoolOpContext *ctx) = 0;

  virtual void enterVariableOp(SEParser::VariableOpContext *ctx) = 0;
  virtual void exitVariableOp(SEParser::VariableOpContext *ctx) = 0;

  virtual void enterValue(SEParser::ValueContext *ctx) = 0;
  virtual void exitValue(SEParser::ValueContext *ctx) = 0;

  virtual void enterSubTable(SEParser::SubTableContext *ctx) = 0;
  virtual void exitSubTable(SEParser::SubTableContext *ctx) = 0;

  virtual void enterNumeric(SEParser::NumericContext *ctx) = 0;
  virtual void exitNumeric(SEParser::NumericContext *ctx) = 0;

  virtual void enterString(SEParser::StringContext *ctx) = 0;
  virtual void exitString(SEParser::StringContext *ctx) = 0;

  virtual void enterBoolean(SEParser::BooleanContext *ctx) = 0;
  virtual void exitBoolean(SEParser::BooleanContext *ctx) = 0;

  virtual void enterArray(SEParser::ArrayContext *ctx) = 0;
  virtual void exitArray(SEParser::ArrayContext *ctx) = 0;

  virtual void enterTuple(SEParser::TupleContext *ctx) = 0;
  virtual void exitTuple(SEParser::TupleContext *ctx) = 0;

  virtual void enterComparableTypes(SEParser::ComparableTypesContext *ctx) = 0;
  virtual void exitComparableTypes(SEParser::ComparableTypesContext *ctx) = 0;

  virtual void enterModifier(SEParser::ModifierContext *ctx) = 0;
  virtual void exitModifier(SEParser::ModifierContext *ctx) = 0;

  virtual void enterOp(SEParser::OpContext *ctx) = 0;
  virtual void exitOp(SEParser::OpContext *ctx) = 0;


};

