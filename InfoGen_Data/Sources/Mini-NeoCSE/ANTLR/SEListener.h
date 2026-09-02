
// Generated from SE.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SEParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by SEParser.
 */
class  SEListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterTableWithKeyValue(SEParser::TableWithKeyValueContext *ctx) = 0;
  virtual void exitTableWithKeyValue(SEParser::TableWithKeyValueContext *ctx) = 0;

  virtual void enterTableWithKeyValueSubtable(SEParser::TableWithKeyValueSubtableContext *ctx) = 0;
  virtual void exitTableWithKeyValueSubtable(SEParser::TableWithKeyValueSubtableContext *ctx) = 0;

  virtual void enterTableWithKeyOnly(SEParser::TableWithKeyOnlyContext *ctx) = 0;
  virtual void exitTableWithKeyOnly(SEParser::TableWithKeyOnlyContext *ctx) = 0;

  virtual void enterTableWithKeySubTable(SEParser::TableWithKeySubTableContext *ctx) = 0;
  virtual void exitTableWithKeySubTable(SEParser::TableWithKeySubTableContext *ctx) = 0;

  virtual void enterTableWithBoolOperation(SEParser::TableWithBoolOperationContext *ctx) = 0;
  virtual void exitTableWithBoolOperation(SEParser::TableWithBoolOperationContext *ctx) = 0;

  virtual void enterTableWithVarOperation(SEParser::TableWithVarOperationContext *ctx) = 0;
  virtual void exitTableWithVarOperation(SEParser::TableWithVarOperationContext *ctx) = 0;

  virtual void enterTableIsEmpty(SEParser::TableIsEmptyContext *ctx) = 0;
  virtual void exitTableIsEmpty(SEParser::TableIsEmptyContext *ctx) = 0;

  virtual void enterKeyLabel(SEParser::KeyLabelContext *ctx) = 0;
  virtual void exitKeyLabel(SEParser::KeyLabelContext *ctx) = 0;

  virtual void enterValueGroupIterating(SEParser::ValueGroupIteratingContext *ctx) = 0;
  virtual void exitValueGroupIterating(SEParser::ValueGroupIteratingContext *ctx) = 0;

  virtual void enterValueGroupTerminating(SEParser::ValueGroupTerminatingContext *ctx) = 0;
  virtual void exitValueGroupTerminating(SEParser::ValueGroupTerminatingContext *ctx) = 0;

  virtual void enterBoolOperation(SEParser::BoolOperationContext *ctx) = 0;
  virtual void exitBoolOperation(SEParser::BoolOperationContext *ctx) = 0;

  virtual void enterVariableOperation(SEParser::VariableOperationContext *ctx) = 0;
  virtual void exitVariableOperation(SEParser::VariableOperationContext *ctx) = 0;

  virtual void enterValueAsRawTypes(SEParser::ValueAsRawTypesContext *ctx) = 0;
  virtual void exitValueAsRawTypes(SEParser::ValueAsRawTypesContext *ctx) = 0;

  virtual void enterValueAsArray(SEParser::ValueAsArrayContext *ctx) = 0;
  virtual void exitValueAsArray(SEParser::ValueAsArrayContext *ctx) = 0;

  virtual void enterValueAsTuple(SEParser::ValueAsTupleContext *ctx) = 0;
  virtual void exitValueAsTuple(SEParser::ValueAsTupleContext *ctx) = 0;

  virtual void enterSubTableExpand(SEParser::SubTableExpandContext *ctx) = 0;
  virtual void exitSubTableExpand(SEParser::SubTableExpandContext *ctx) = 0;

  virtual void enterSimpleTypeNumeric(SEParser::SimpleTypeNumericContext *ctx) = 0;
  virtual void exitSimpleTypeNumeric(SEParser::SimpleTypeNumericContext *ctx) = 0;

  virtual void enterSimpleTypeString(SEParser::SimpleTypeStringContext *ctx) = 0;
  virtual void exitSimpleTypeString(SEParser::SimpleTypeStringContext *ctx) = 0;

  virtual void enterSimpleTypeBoolean(SEParser::SimpleTypeBooleanContext *ctx) = 0;
  virtual void exitSimpleTypeBoolean(SEParser::SimpleTypeBooleanContext *ctx) = 0;

  virtual void enterArrayIterating2(SEParser::ArrayIterating2Context *ctx) = 0;
  virtual void exitArrayIterating2(SEParser::ArrayIterating2Context *ctx) = 0;

  virtual void enterArrayIterating1(SEParser::ArrayIterating1Context *ctx) = 0;
  virtual void exitArrayIterating1(SEParser::ArrayIterating1Context *ctx) = 0;

  virtual void enterArrayStart(SEParser::ArrayStartContext *ctx) = 0;
  virtual void exitArrayStart(SEParser::ArrayStartContext *ctx) = 0;

  virtual void enterTupleTerminating1(SEParser::TupleTerminating1Context *ctx) = 0;
  virtual void exitTupleTerminating1(SEParser::TupleTerminating1Context *ctx) = 0;

  virtual void enterTupleIterating1(SEParser::TupleIterating1Context *ctx) = 0;
  virtual void exitTupleIterating1(SEParser::TupleIterating1Context *ctx) = 0;

  virtual void enterTupleIterating2(SEParser::TupleIterating2Context *ctx) = 0;
  virtual void exitTupleIterating2(SEParser::TupleIterating2Context *ctx) = 0;

  virtual void enterTupleTerminating2(SEParser::TupleTerminating2Context *ctx) = 0;
  virtual void exitTupleTerminating2(SEParser::TupleTerminating2Context *ctx) = 0;

  virtual void enterCompTypeAsIdent(SEParser::CompTypeAsIdentContext *ctx) = 0;
  virtual void exitCompTypeAsIdent(SEParser::CompTypeAsIdentContext *ctx) = 0;

  virtual void enterCompTypeAsRawTypes(SEParser::CompTypeAsRawTypesContext *ctx) = 0;
  virtual void exitCompTypeAsRawTypes(SEParser::CompTypeAsRawTypesContext *ctx) = 0;

  virtual void enterModifier(SEParser::ModifierContext *ctx) = 0;
  virtual void exitModifier(SEParser::ModifierContext *ctx) = 0;

  virtual void enterOp(SEParser::OpContext *ctx) = 0;
  virtual void exitOp(SEParser::OpContext *ctx) = 0;


};

