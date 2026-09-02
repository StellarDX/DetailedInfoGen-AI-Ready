
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
    RuleTuple = 9, RuleComparableTypes = 10, RuleModifier = 11, RuleOp = 12
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
  class ComparableTypesContext;
  class ModifierContext;
  class OpContext; 

  class  TableContext : public antlr4::ParserRuleContext {
  public:
    TableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    TableContext() = default;
    void copyFrom(TableContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  TableWithKeyValueSubtableContext : public TableContext {
  public:
    TableWithKeyValueSubtableContext(TableContext *ctx);

    KeyContext *key();
    ValueGroupContext *valueGroup();
    SubTableContext *subTable();
    TableContext *table();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TableWithKeyValueContext : public TableContext {
  public:
    TableWithKeyValueContext(TableContext *ctx);

    KeyContext *key();
    ValueGroupContext *valueGroup();
    TableContext *table();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TableWithKeyOnlyContext : public TableContext {
  public:
    TableWithKeyOnlyContext(TableContext *ctx);

    KeyContext *key();
    TableContext *table();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TableWithBoolOperationContext : public TableContext {
  public:
    TableWithBoolOperationContext(TableContext *ctx);

    KeyContext *key();
    BoolOpContext *boolOp();
    TableContext *table();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TableWithKeySubTableContext : public TableContext {
  public:
    TableWithKeySubTableContext(TableContext *ctx);

    KeyContext *key();
    SubTableContext *subTable();
    TableContext *table();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TableWithVarOperationContext : public TableContext {
  public:
    TableWithVarOperationContext(TableContext *ctx);

    VariableOpContext *variableOp();
    TableContext *table();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TableIsEmptyContext : public TableContext {
  public:
    TableIsEmptyContext(TableContext *ctx);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  TableContext* table();

  class  KeyContext : public antlr4::ParserRuleContext {
  public:
    KeyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    KeyContext() = default;
    void copyFrom(KeyContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  KeyLabelContext : public KeyContext {
  public:
    KeyLabelContext(KeyContext *ctx);

    antlr4::tree::TerminalNode *Identifier();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  KeyContext* key();

  class  ValueGroupContext : public antlr4::ParserRuleContext {
  public:
    ValueGroupContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ValueGroupContext() = default;
    void copyFrom(ValueGroupContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ValueGroupIteratingContext : public ValueGroupContext {
  public:
    ValueGroupIteratingContext(ValueGroupContext *ctx);

    ValueContext *value();
    ValueGroupContext *valueGroup();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  ValueGroupTerminatingContext : public ValueGroupContext {
  public:
    ValueGroupTerminatingContext(ValueGroupContext *ctx);

    ValueContext *value();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  ValueGroupContext* valueGroup();

  class  BoolOpContext : public antlr4::ParserRuleContext {
  public:
    BoolOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    BoolOpContext() = default;
    void copyFrom(BoolOpContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  BoolOperationContext : public BoolOpContext {
  public:
    BoolOperationContext(BoolOpContext *ctx);

    std::vector<ComparableTypesContext *> comparableTypes();
    ComparableTypesContext* comparableTypes(size_t i);
    OpContext *op();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  BoolOpContext* boolOp();

  class  VariableOpContext : public antlr4::ParserRuleContext {
  public:
    VariableOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    VariableOpContext() = default;
    void copyFrom(VariableOpContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  VariableOperationContext : public VariableOpContext {
  public:
    VariableOperationContext(VariableOpContext *ctx);

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
   
    ValueContext() = default;
    void copyFrom(ValueContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ValueAsTupleContext : public ValueContext {
  public:
    ValueAsTupleContext(ValueContext *ctx);

    TupleContext *tuple();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  ValueAsArrayContext : public ValueContext {
  public:
    ValueAsArrayContext(ValueContext *ctx);

    ArrayContext *array();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  ValueAsRawTypesContext : public ValueContext {
  public:
    ValueAsRawTypesContext(ValueContext *ctx);

    SimpleTypesContext *simpleTypes();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  ValueContext* value();

  class  SubTableContext : public antlr4::ParserRuleContext {
  public:
    SubTableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    SubTableContext() = default;
    void copyFrom(SubTableContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  SubTableExpandContext : public SubTableContext {
  public:
    SubTableExpandContext(SubTableContext *ctx);

    TableContext *table();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  SubTableContext* subTable();

  class  SimpleTypesContext : public antlr4::ParserRuleContext {
  public:
    SimpleTypesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    SimpleTypesContext() = default;
    void copyFrom(SimpleTypesContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  SimpleTypeStringContext : public SimpleTypesContext {
  public:
    SimpleTypeStringContext(SimpleTypesContext *ctx);

    antlr4::tree::TerminalNode *String();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  SimpleTypeBooleanContext : public SimpleTypesContext {
  public:
    SimpleTypeBooleanContext(SimpleTypesContext *ctx);

    antlr4::tree::TerminalNode *Boolean();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  SimpleTypeNumericContext : public SimpleTypesContext {
  public:
    SimpleTypeNumericContext(SimpleTypesContext *ctx);

    antlr4::tree::TerminalNode *Numeric();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  SimpleTypesContext* simpleTypes();

  class  ArrayContext : public antlr4::ParserRuleContext {
  public:
    ArrayContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ArrayContext() = default;
    void copyFrom(ArrayContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ArrayIterating2Context : public ArrayContext {
  public:
    ArrayIterating2Context(ArrayContext *ctx);

    ArrayContext *array();
    SimpleTypesContext *simpleTypes();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  ArrayIterating1Context : public ArrayContext {
  public:
    ArrayIterating1Context(ArrayContext *ctx);

    ArrayContext *array();
    SimpleTypesContext *simpleTypes();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  ArrayStartContext : public ArrayContext {
  public:
    ArrayStartContext(ArrayContext *ctx);

    SimpleTypesContext *simpleTypes();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  ArrayContext* array();
  ArrayContext* array(int precedence);
  class  TupleContext : public antlr4::ParserRuleContext {
  public:
    TupleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    TupleContext() = default;
    void copyFrom(TupleContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  TupleIterating2Context : public TupleContext {
  public:
    TupleIterating2Context(TupleContext *ctx);

    ValueContext *value();
    TupleContext *tuple();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TupleIterating1Context : public TupleContext {
  public:
    TupleIterating1Context(TupleContext *ctx);

    ValueContext *value();
    TupleContext *tuple();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TupleTerminating2Context : public TupleContext {
  public:
    TupleTerminating2Context(TupleContext *ctx);

    ValueContext *value();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  TupleTerminating1Context : public TupleContext {
  public:
    TupleTerminating1Context(TupleContext *ctx);

    ValueContext *value();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  TupleContext* tuple();

  class  ComparableTypesContext : public antlr4::ParserRuleContext {
  public:
    ComparableTypesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ComparableTypesContext() = default;
    void copyFrom(ComparableTypesContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  CompTypeAsIdentContext : public ComparableTypesContext {
  public:
    CompTypeAsIdentContext(ComparableTypesContext *ctx);

    antlr4::tree::TerminalNode *Identifier();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  CompTypeAsRawTypesContext : public ComparableTypesContext {
  public:
    CompTypeAsRawTypesContext(ComparableTypesContext *ctx);

    SimpleTypesContext *simpleTypes();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  ComparableTypesContext* comparableTypes();

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

