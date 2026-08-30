
// Generated from SE.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SEParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, Set = 6, SetU = 7, 
    SetForce = 8, Reset = 9, Interpolate = 10, Get = 11, WaitVar = 12, SaveVars = 13, 
    RestoreVars = 14, Boolean = 15, Op = 16, Numeric = 17, String = 18, 
    Identifier = 19, WS = 20, LINE_COMMENT = 21, BLOCK_COMMENT = 22
  };

  enum {
    RuleTable = 0, RuleKey = 1, RuleValueGroup = 2, RuleBoolOp = 3, RuleVariableOp = 4, 
    RuleValue = 5, RuleSubTable = 6, RuleSimpleTypes = 7, RuleArray = 8, 
    RuleTuple = 9, RuleModifier = 10, RuleOp = 11
  };

  explicit SEParser(antlr4::TokenStream *input);

  SEParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~SEParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class TableContext;
  class KeyContext;
  class ValueGroupContext;
  class BoolOpContext;
  class VariableOpContext;
  class ValueContext;
  class SubTableContext;
  class SimpleTypesContext;
  class ArrayContext;
  class TupleContext;
  class ModifierContext;
  class OpContext; 

  class  TableContext : public antlr4::ParserRuleContext {
  public:
    TableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TableContext *table();
    KeyContext *key();
    ValueGroupContext *valueGroup();
    BoolOpContext *boolOp();
    VariableOpContext *variableOp();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TableContext* table();

  class  KeyContext : public antlr4::ParserRuleContext {
  public:
    KeyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  KeyContext* key();

  class  ValueGroupContext : public antlr4::ParserRuleContext {
  public:
    ValueGroupContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueContext *value();
    ValueGroupContext *valueGroup();
    SubTableContext *subTable();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ValueGroupContext* valueGroup();

  class  BoolOpContext : public antlr4::ParserRuleContext {
  public:
    BoolOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SimpleTypesContext *> simpleTypes();
    SimpleTypesContext* simpleTypes(size_t i);
    OpContext *op();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BoolOpContext* boolOp();

  class  VariableOpContext : public antlr4::ParserRuleContext {
  public:
    VariableOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ModifierContext *modifier();
    antlr4::tree::TerminalNode *Identifier();
    SimpleTypesContext *simpleTypes();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VariableOpContext* variableOp();

  class  ValueContext : public antlr4::ParserRuleContext {
  public:
    ValueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SimpleTypesContext *simpleTypes();
    ArrayContext *array();
    TupleContext *tuple();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ValueContext* value();

  class  SubTableContext : public antlr4::ParserRuleContext {
  public:
    SubTableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TableContext *table();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SubTableContext* subTable();

  class  SimpleTypesContext : public antlr4::ParserRuleContext {
  public:
    SimpleTypesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Numeric();
    antlr4::tree::TerminalNode *String();
    antlr4::tree::TerminalNode *Boolean();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SimpleTypesContext* simpleTypes();

  class  ArrayContext : public antlr4::ParserRuleContext {
  public:
    ArrayContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SimpleTypesContext *simpleTypes();
    ArrayContext *array();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ArrayContext* array();
  ArrayContext* array(int precedence);
  class  TupleContext : public antlr4::ParserRuleContext {
  public:
    TupleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ValueContext *value();
    TupleContext *tuple();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TupleContext* tuple();

  class  ModifierContext : public antlr4::ParserRuleContext {
  public:
    ModifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Set();
    antlr4::tree::TerminalNode *SetU();
    antlr4::tree::TerminalNode *SetForce();
    antlr4::tree::TerminalNode *Reset();
    antlr4::tree::TerminalNode *Interpolate();
    antlr4::tree::TerminalNode *Get();
    antlr4::tree::TerminalNode *WaitVar();
    antlr4::tree::TerminalNode *SaveVars();
    antlr4::tree::TerminalNode *RestoreVars();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ModifierContext* modifier();

  class  OpContext : public antlr4::ParserRuleContext {
  public:
    OpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *Op();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OpContext* op();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool arraySempred(ArrayContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

