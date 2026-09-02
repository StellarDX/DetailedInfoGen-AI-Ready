
// Generated from SE.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "SEListener.h"


/**
 * This class provides an empty implementation of SEListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  SEBaseListener : public SEListener {
public:

  virtual void enterTableWithKeyValue(SEParser::TableWithKeyValueContext * /*ctx*/) override { }
  virtual void exitTableWithKeyValue(SEParser::TableWithKeyValueContext * /*ctx*/) override { }

  virtual void enterTableWithKeyValueSubtable(SEParser::TableWithKeyValueSubtableContext * /*ctx*/) override { }
  virtual void exitTableWithKeyValueSubtable(SEParser::TableWithKeyValueSubtableContext * /*ctx*/) override { }

  virtual void enterTableWithKeyOnly(SEParser::TableWithKeyOnlyContext * /*ctx*/) override { }
  virtual void exitTableWithKeyOnly(SEParser::TableWithKeyOnlyContext * /*ctx*/) override { }

  virtual void enterTableWithKeySubTable(SEParser::TableWithKeySubTableContext * /*ctx*/) override { }
  virtual void exitTableWithKeySubTable(SEParser::TableWithKeySubTableContext * /*ctx*/) override { }

  virtual void enterTableWithBoolOperation(SEParser::TableWithBoolOperationContext * /*ctx*/) override { }
  virtual void exitTableWithBoolOperation(SEParser::TableWithBoolOperationContext * /*ctx*/) override { }

  virtual void enterTableWithVarOperation(SEParser::TableWithVarOperationContext * /*ctx*/) override { }
  virtual void exitTableWithVarOperation(SEParser::TableWithVarOperationContext * /*ctx*/) override { }

  virtual void enterTableIsEmpty(SEParser::TableIsEmptyContext * /*ctx*/) override { }
  virtual void exitTableIsEmpty(SEParser::TableIsEmptyContext * /*ctx*/) override { }

  virtual void enterKeyLabel(SEParser::KeyLabelContext * /*ctx*/) override { }
  virtual void exitKeyLabel(SEParser::KeyLabelContext * /*ctx*/) override { }

  virtual void enterValueGroupIterating(SEParser::ValueGroupIteratingContext * /*ctx*/) override { }
  virtual void exitValueGroupIterating(SEParser::ValueGroupIteratingContext * /*ctx*/) override { }

  virtual void enterValueGroupTerminating(SEParser::ValueGroupTerminatingContext * /*ctx*/) override { }
  virtual void exitValueGroupTerminating(SEParser::ValueGroupTerminatingContext * /*ctx*/) override { }

  virtual void enterBoolOperation(SEParser::BoolOperationContext * /*ctx*/) override { }
  virtual void exitBoolOperation(SEParser::BoolOperationContext * /*ctx*/) override { }

  virtual void enterVariableOperation(SEParser::VariableOperationContext * /*ctx*/) override { }
  virtual void exitVariableOperation(SEParser::VariableOperationContext * /*ctx*/) override { }

  virtual void enterValueAsRawTypes(SEParser::ValueAsRawTypesContext * /*ctx*/) override { }
  virtual void exitValueAsRawTypes(SEParser::ValueAsRawTypesContext * /*ctx*/) override { }

  virtual void enterValueAsArray(SEParser::ValueAsArrayContext * /*ctx*/) override { }
  virtual void exitValueAsArray(SEParser::ValueAsArrayContext * /*ctx*/) override { }

  virtual void enterValueAsTuple(SEParser::ValueAsTupleContext * /*ctx*/) override { }
  virtual void exitValueAsTuple(SEParser::ValueAsTupleContext * /*ctx*/) override { }

  virtual void enterSubTableExpand(SEParser::SubTableExpandContext * /*ctx*/) override { }
  virtual void exitSubTableExpand(SEParser::SubTableExpandContext * /*ctx*/) override { }

  virtual void enterSimpleTypeNumeric(SEParser::SimpleTypeNumericContext * /*ctx*/) override { }
  virtual void exitSimpleTypeNumeric(SEParser::SimpleTypeNumericContext * /*ctx*/) override { }

  virtual void enterSimpleTypeString(SEParser::SimpleTypeStringContext * /*ctx*/) override { }
  virtual void exitSimpleTypeString(SEParser::SimpleTypeStringContext * /*ctx*/) override { }

  virtual void enterSimpleTypeBoolean(SEParser::SimpleTypeBooleanContext * /*ctx*/) override { }
  virtual void exitSimpleTypeBoolean(SEParser::SimpleTypeBooleanContext * /*ctx*/) override { }

  virtual void enterArrayIterating2(SEParser::ArrayIterating2Context * /*ctx*/) override { }
  virtual void exitArrayIterating2(SEParser::ArrayIterating2Context * /*ctx*/) override { }

  virtual void enterArrayIterating1(SEParser::ArrayIterating1Context * /*ctx*/) override { }
  virtual void exitArrayIterating1(SEParser::ArrayIterating1Context * /*ctx*/) override { }

  virtual void enterArrayStart(SEParser::ArrayStartContext * /*ctx*/) override { }
  virtual void exitArrayStart(SEParser::ArrayStartContext * /*ctx*/) override { }

  virtual void enterTupleTerminating1(SEParser::TupleTerminating1Context * /*ctx*/) override { }
  virtual void exitTupleTerminating1(SEParser::TupleTerminating1Context * /*ctx*/) override { }

  virtual void enterTupleIterating1(SEParser::TupleIterating1Context * /*ctx*/) override { }
  virtual void exitTupleIterating1(SEParser::TupleIterating1Context * /*ctx*/) override { }

  virtual void enterTupleIterating2(SEParser::TupleIterating2Context * /*ctx*/) override { }
  virtual void exitTupleIterating2(SEParser::TupleIterating2Context * /*ctx*/) override { }

  virtual void enterTupleTerminating2(SEParser::TupleTerminating2Context * /*ctx*/) override { }
  virtual void exitTupleTerminating2(SEParser::TupleTerminating2Context * /*ctx*/) override { }

  virtual void enterCompTypeAsIdent(SEParser::CompTypeAsIdentContext * /*ctx*/) override { }
  virtual void exitCompTypeAsIdent(SEParser::CompTypeAsIdentContext * /*ctx*/) override { }

  virtual void enterCompTypeAsRawTypes(SEParser::CompTypeAsRawTypesContext * /*ctx*/) override { }
  virtual void exitCompTypeAsRawTypes(SEParser::CompTypeAsRawTypesContext * /*ctx*/) override { }

  virtual void enterModifier(SEParser::ModifierContext * /*ctx*/) override { }
  virtual void exitModifier(SEParser::ModifierContext * /*ctx*/) override { }

  virtual void enterOp(SEParser::OpContext * /*ctx*/) override { }
  virtual void exitOp(SEParser::OpContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

