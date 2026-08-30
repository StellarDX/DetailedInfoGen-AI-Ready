
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

  virtual void enterTable(SEParser::TableContext * /*ctx*/) override { }
  virtual void exitTable(SEParser::TableContext * /*ctx*/) override { }

  virtual void enterKey(SEParser::KeyContext * /*ctx*/) override { }
  virtual void exitKey(SEParser::KeyContext * /*ctx*/) override { }

  virtual void enterValueGroup(SEParser::ValueGroupContext * /*ctx*/) override { }
  virtual void exitValueGroup(SEParser::ValueGroupContext * /*ctx*/) override { }

  virtual void enterBoolOp(SEParser::BoolOpContext * /*ctx*/) override { }
  virtual void exitBoolOp(SEParser::BoolOpContext * /*ctx*/) override { }

  virtual void enterVariableOp(SEParser::VariableOpContext * /*ctx*/) override { }
  virtual void exitVariableOp(SEParser::VariableOpContext * /*ctx*/) override { }

  virtual void enterValue(SEParser::ValueContext * /*ctx*/) override { }
  virtual void exitValue(SEParser::ValueContext * /*ctx*/) override { }

  virtual void enterSubTable(SEParser::SubTableContext * /*ctx*/) override { }
  virtual void exitSubTable(SEParser::SubTableContext * /*ctx*/) override { }

  virtual void enterNumeric(SEParser::NumericContext * /*ctx*/) override { }
  virtual void exitNumeric(SEParser::NumericContext * /*ctx*/) override { }

  virtual void enterString(SEParser::StringContext * /*ctx*/) override { }
  virtual void exitString(SEParser::StringContext * /*ctx*/) override { }

  virtual void enterBoolean(SEParser::BooleanContext * /*ctx*/) override { }
  virtual void exitBoolean(SEParser::BooleanContext * /*ctx*/) override { }

  virtual void enterArray(SEParser::ArrayContext * /*ctx*/) override { }
  virtual void exitArray(SEParser::ArrayContext * /*ctx*/) override { }

  virtual void enterTuple(SEParser::TupleContext * /*ctx*/) override { }
  virtual void exitTuple(SEParser::TupleContext * /*ctx*/) override { }

  virtual void enterComparableTypes(SEParser::ComparableTypesContext * /*ctx*/) override { }
  virtual void exitComparableTypes(SEParser::ComparableTypesContext * /*ctx*/) override { }

  virtual void enterModifier(SEParser::ModifierContext * /*ctx*/) override { }
  virtual void exitModifier(SEParser::ModifierContext * /*ctx*/) override { }

  virtual void enterOp(SEParser::OpContext * /*ctx*/) override { }
  virtual void exitOp(SEParser::OpContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

