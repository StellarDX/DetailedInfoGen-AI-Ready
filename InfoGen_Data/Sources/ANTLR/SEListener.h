
// Generated from SE.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SEParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by SEParser.
 */
class  SEListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterStart(SEParser::StartContext *ctx) = 0;
  virtual void exitStart(SEParser::StartContext *ctx) = 0;

  virtual void enterTable(SEParser::TableContext *ctx) = 0;
  virtual void exitTable(SEParser::TableContext *ctx) = 0;

  virtual void enterEntry(SEParser::EntryContext *ctx) = 0;
  virtual void exitEntry(SEParser::EntryContext *ctx) = 0;

  virtual void enterKey(SEParser::KeyContext *ctx) = 0;
  virtual void exitKey(SEParser::KeyContext *ctx) = 0;

  virtual void enterValueGroup(SEParser::ValueGroupContext *ctx) = 0;
  virtual void exitValueGroup(SEParser::ValueGroupContext *ctx) = 0;

  virtual void enterBoolBlock(SEParser::BoolBlockContext *ctx) = 0;
  virtual void exitBoolBlock(SEParser::BoolBlockContext *ctx) = 0;

  virtual void enterBoolOp(SEParser::BoolOpContext *ctx) = 0;
  virtual void exitBoolOp(SEParser::BoolOpContext *ctx) = 0;

  virtual void enterValue(SEParser::ValueContext *ctx) = 0;
  virtual void exitValue(SEParser::ValueContext *ctx) = 0;

  virtual void enterSubTable(SEParser::SubTableContext *ctx) = 0;
  virtual void exitSubTable(SEParser::SubTableContext *ctx) = 0;

  virtual void enterSimpleTypes(SEParser::SimpleTypesContext *ctx) = 0;
  virtual void exitSimpleTypes(SEParser::SimpleTypesContext *ctx) = 0;

  virtual void enterNumeric(SEParser::NumericContext *ctx) = 0;
  virtual void exitNumeric(SEParser::NumericContext *ctx) = 0;

  virtual void enterVariable(SEParser::VariableContext *ctx) = 0;
  virtual void exitVariable(SEParser::VariableContext *ctx) = 0;

  virtual void enterArray(SEParser::ArrayContext *ctx) = 0;
  virtual void exitArray(SEParser::ArrayContext *ctx) = 0;

  virtual void enterTuple(SEParser::TupleContext *ctx) = 0;
  virtual void exitTuple(SEParser::TupleContext *ctx) = 0;


};

