
// Generated from SE.g4 by ANTLR 4.13.2


#include "SEListener.h"

#include "SEParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct SEParserStaticData final {
  SEParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  SEParserStaticData(const SEParserStaticData&) = delete;
  SEParserStaticData(SEParserStaticData&&) = delete;
  SEParserStaticData& operator=(const SEParserStaticData&) = delete;
  SEParserStaticData& operator=(SEParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag seParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
std::unique_ptr<SEParserStaticData> seParserStaticData = nullptr;

void seParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (seParserStaticData != nullptr) {
    return;
  }
#else
  assert(seParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<SEParserStaticData>(
    std::vector<std::string>{
      "table", "key", "valueGroup", "boolOp", "variableOp", "value", "subTable", 
      "simpleTypes", "array", "tuple", "comparableTypes", "modifier", "op"
    },
    std::vector<std::string>{
      "", "'{'", "'}'", "'('", "')'", "','", "'Set'", "'SetU'", "'SetForce'", 
      "'Reset'", "'Interpolate'", "'Get'", "'WaitVar'", "'SaveVars'", "'RestoreVars'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "Set", "SetU", "SetForce", "Reset", "Interpolate", 
      "Get", "WaitVar", "SaveVars", "RestoreVars", "Boolean", "Op", "Numeric", 
      "String", "Identifier", "WS", "LINE_COMMENT", "BLOCK_COMMENT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,22,125,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,1,0,1,0,1,0,1,0,1,
  	0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  	1,0,1,0,1,0,1,0,3,0,53,8,0,1,1,1,1,1,2,1,2,1,2,1,2,3,2,61,8,2,1,3,1,3,
  	1,3,1,3,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,5,1,5,1,5,1,5,3,5,80,8,
  	5,1,6,1,6,1,6,1,6,1,7,1,7,1,7,3,7,89,8,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,
  	1,8,5,8,99,8,8,10,8,12,8,102,9,8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,
  	1,9,1,9,3,9,115,8,9,1,10,1,10,3,10,119,8,10,1,11,1,11,1,12,1,12,1,12,
  	0,1,16,13,0,2,4,6,8,10,12,14,16,18,20,22,24,0,1,1,0,6,14,128,0,52,1,0,
  	0,0,2,54,1,0,0,0,4,60,1,0,0,0,6,62,1,0,0,0,8,66,1,0,0,0,10,79,1,0,0,0,
  	12,81,1,0,0,0,14,88,1,0,0,0,16,90,1,0,0,0,18,114,1,0,0,0,20,118,1,0,0,
  	0,22,120,1,0,0,0,24,122,1,0,0,0,26,27,3,2,1,0,27,28,3,4,2,0,28,29,3,0,
  	0,0,29,53,1,0,0,0,30,31,3,2,1,0,31,32,3,4,2,0,32,33,3,12,6,0,33,34,3,
  	0,0,0,34,53,1,0,0,0,35,36,3,2,1,0,36,37,3,0,0,0,37,53,1,0,0,0,38,39,3,
  	2,1,0,39,40,3,12,6,0,40,41,3,0,0,0,41,53,1,0,0,0,42,43,3,2,1,0,43,44,
  	5,1,0,0,44,45,3,6,3,0,45,46,5,2,0,0,46,47,3,0,0,0,47,53,1,0,0,0,48,49,
  	3,8,4,0,49,50,3,0,0,0,50,53,1,0,0,0,51,53,1,0,0,0,52,26,1,0,0,0,52,30,
  	1,0,0,0,52,35,1,0,0,0,52,38,1,0,0,0,52,42,1,0,0,0,52,48,1,0,0,0,52,51,
  	1,0,0,0,53,1,1,0,0,0,54,55,5,19,0,0,55,3,1,0,0,0,56,57,3,10,5,0,57,58,
  	3,4,2,0,58,61,1,0,0,0,59,61,3,10,5,0,60,56,1,0,0,0,60,59,1,0,0,0,61,5,
  	1,0,0,0,62,63,3,20,10,0,63,64,3,24,12,0,64,65,3,20,10,0,65,7,1,0,0,0,
  	66,67,3,22,11,0,67,68,5,19,0,0,68,69,3,14,7,0,69,9,1,0,0,0,70,80,3,14,
  	7,0,71,72,5,3,0,0,72,73,3,16,8,0,73,74,5,4,0,0,74,80,1,0,0,0,75,76,5,
  	1,0,0,76,77,3,18,9,0,77,78,5,2,0,0,78,80,1,0,0,0,79,70,1,0,0,0,79,71,
  	1,0,0,0,79,75,1,0,0,0,80,11,1,0,0,0,81,82,5,1,0,0,82,83,3,0,0,0,83,84,
  	5,2,0,0,84,13,1,0,0,0,85,89,5,17,0,0,86,89,5,18,0,0,87,89,5,15,0,0,88,
  	85,1,0,0,0,88,86,1,0,0,0,88,87,1,0,0,0,89,15,1,0,0,0,90,91,6,8,-1,0,91,
  	92,3,14,7,0,92,100,1,0,0,0,93,94,10,2,0,0,94,95,5,5,0,0,95,99,3,14,7,
  	0,96,97,10,1,0,0,97,99,3,14,7,0,98,93,1,0,0,0,98,96,1,0,0,0,99,102,1,
  	0,0,0,100,98,1,0,0,0,100,101,1,0,0,0,101,17,1,0,0,0,102,100,1,0,0,0,103,
  	115,3,10,5,0,104,105,3,10,5,0,105,106,5,5,0,0,106,107,3,18,9,0,107,115,
  	1,0,0,0,108,109,3,10,5,0,109,110,3,18,9,0,110,115,1,0,0,0,111,112,3,10,
  	5,0,112,113,5,5,0,0,113,115,1,0,0,0,114,103,1,0,0,0,114,104,1,0,0,0,114,
  	108,1,0,0,0,114,111,1,0,0,0,115,19,1,0,0,0,116,119,5,19,0,0,117,119,3,
  	14,7,0,118,116,1,0,0,0,118,117,1,0,0,0,119,21,1,0,0,0,120,121,7,0,0,0,
  	121,23,1,0,0,0,122,123,5,16,0,0,123,25,1,0,0,0,8,52,60,79,88,98,100,114,
  	118
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  seParserStaticData = std::move(staticData);
}

}

SEParser::SEParser(TokenStream *input) : SEParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

SEParser::SEParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  SEParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *seParserStaticData->atn, seParserStaticData->decisionToDFA, seParserStaticData->sharedContextCache, options);
}

SEParser::~SEParser() {
  delete _interpreter;
}

const atn::ATN& SEParser::getATN() const {
  return *seParserStaticData->atn;
}

std::string SEParser::getGrammarFileName() const {
  return "SE.g4";
}

const std::vector<std::string>& SEParser::getRuleNames() const {
  return seParserStaticData->ruleNames;
}

const dfa::Vocabulary& SEParser::getVocabulary() const {
  return seParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView SEParser::getSerializedATN() const {
  return seParserStaticData->serializedATN;
}


//----------------- TableContext ------------------------------------------------------------------

SEParser::TableContext::TableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::TableContext::getRuleIndex() const {
  return SEParser::RuleTable;
}

void SEParser::TableContext::copyFrom(TableContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- TableWithKeyValueSubtableContext ------------------------------------------------------------------

SEParser::KeyContext* SEParser::TableWithKeyValueSubtableContext::key() {
  return getRuleContext<SEParser::KeyContext>(0);
}

SEParser::ValueGroupContext* SEParser::TableWithKeyValueSubtableContext::valueGroup() {
  return getRuleContext<SEParser::ValueGroupContext>(0);
}

SEParser::SubTableContext* SEParser::TableWithKeyValueSubtableContext::subTable() {
  return getRuleContext<SEParser::SubTableContext>(0);
}

SEParser::TableContext* SEParser::TableWithKeyValueSubtableContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::TableWithKeyValueSubtableContext::TableWithKeyValueSubtableContext(TableContext *ctx) { copyFrom(ctx); }

void SEParser::TableWithKeyValueSubtableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableWithKeyValueSubtable(this);
}
void SEParser::TableWithKeyValueSubtableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableWithKeyValueSubtable(this);
}
//----------------- TableWithKeyValueContext ------------------------------------------------------------------

SEParser::KeyContext* SEParser::TableWithKeyValueContext::key() {
  return getRuleContext<SEParser::KeyContext>(0);
}

SEParser::ValueGroupContext* SEParser::TableWithKeyValueContext::valueGroup() {
  return getRuleContext<SEParser::ValueGroupContext>(0);
}

SEParser::TableContext* SEParser::TableWithKeyValueContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::TableWithKeyValueContext::TableWithKeyValueContext(TableContext *ctx) { copyFrom(ctx); }

void SEParser::TableWithKeyValueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableWithKeyValue(this);
}
void SEParser::TableWithKeyValueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableWithKeyValue(this);
}
//----------------- TableWithKeyOnlyContext ------------------------------------------------------------------

SEParser::KeyContext* SEParser::TableWithKeyOnlyContext::key() {
  return getRuleContext<SEParser::KeyContext>(0);
}

SEParser::TableContext* SEParser::TableWithKeyOnlyContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::TableWithKeyOnlyContext::TableWithKeyOnlyContext(TableContext *ctx) { copyFrom(ctx); }

void SEParser::TableWithKeyOnlyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableWithKeyOnly(this);
}
void SEParser::TableWithKeyOnlyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableWithKeyOnly(this);
}
//----------------- TableWithBoolOperationContext ------------------------------------------------------------------

SEParser::KeyContext* SEParser::TableWithBoolOperationContext::key() {
  return getRuleContext<SEParser::KeyContext>(0);
}

SEParser::BoolOpContext* SEParser::TableWithBoolOperationContext::boolOp() {
  return getRuleContext<SEParser::BoolOpContext>(0);
}

SEParser::TableContext* SEParser::TableWithBoolOperationContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::TableWithBoolOperationContext::TableWithBoolOperationContext(TableContext *ctx) { copyFrom(ctx); }

void SEParser::TableWithBoolOperationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableWithBoolOperation(this);
}
void SEParser::TableWithBoolOperationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableWithBoolOperation(this);
}
//----------------- TableWithKeySubTableContext ------------------------------------------------------------------

SEParser::KeyContext* SEParser::TableWithKeySubTableContext::key() {
  return getRuleContext<SEParser::KeyContext>(0);
}

SEParser::SubTableContext* SEParser::TableWithKeySubTableContext::subTable() {
  return getRuleContext<SEParser::SubTableContext>(0);
}

SEParser::TableContext* SEParser::TableWithKeySubTableContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::TableWithKeySubTableContext::TableWithKeySubTableContext(TableContext *ctx) { copyFrom(ctx); }

void SEParser::TableWithKeySubTableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableWithKeySubTable(this);
}
void SEParser::TableWithKeySubTableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableWithKeySubTable(this);
}
//----------------- TableWithVarOperationContext ------------------------------------------------------------------

SEParser::VariableOpContext* SEParser::TableWithVarOperationContext::variableOp() {
  return getRuleContext<SEParser::VariableOpContext>(0);
}

SEParser::TableContext* SEParser::TableWithVarOperationContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::TableWithVarOperationContext::TableWithVarOperationContext(TableContext *ctx) { copyFrom(ctx); }

void SEParser::TableWithVarOperationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableWithVarOperation(this);
}
void SEParser::TableWithVarOperationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableWithVarOperation(this);
}
//----------------- TableIsEmptyContext ------------------------------------------------------------------

SEParser::TableIsEmptyContext::TableIsEmptyContext(TableContext *ctx) { copyFrom(ctx); }

void SEParser::TableIsEmptyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTableIsEmpty(this);
}
void SEParser::TableIsEmptyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTableIsEmpty(this);
}
SEParser::TableContext* SEParser::table() {
  TableContext *_localctx = _tracker.createInstance<TableContext>(_ctx, getState());
  enterRule(_localctx, 0, SEParser::RuleTable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(52);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SEParser::TableWithKeyValueContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(26);
      key();
      setState(27);
      valueGroup();
      setState(28);
      table();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SEParser::TableWithKeyValueSubtableContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(30);
      key();
      setState(31);
      valueGroup();
      setState(32);
      subTable();
      setState(33);
      table();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<SEParser::TableWithKeyOnlyContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(35);
      key();
      setState(36);
      table();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<SEParser::TableWithKeySubTableContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(38);
      key();
      setState(39);
      subTable();
      setState(40);
      table();
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<SEParser::TableWithBoolOperationContext>(_localctx);
      enterOuterAlt(_localctx, 5);
      setState(42);
      key();
      setState(43);
      match(SEParser::T__0);
      setState(44);
      boolOp();
      setState(45);
      match(SEParser::T__1);
      setState(46);
      table();
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<SEParser::TableWithVarOperationContext>(_localctx);
      enterOuterAlt(_localctx, 6);
      setState(48);
      variableOp();
      setState(49);
      table();
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<SEParser::TableIsEmptyContext>(_localctx);
      enterOuterAlt(_localctx, 7);

      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- KeyContext ------------------------------------------------------------------

SEParser::KeyContext::KeyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::KeyContext::getRuleIndex() const {
  return SEParser::RuleKey;
}

void SEParser::KeyContext::copyFrom(KeyContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- KeyLabelContext ------------------------------------------------------------------

tree::TerminalNode* SEParser::KeyLabelContext::Identifier() {
  return getToken(SEParser::Identifier, 0);
}

SEParser::KeyLabelContext::KeyLabelContext(KeyContext *ctx) { copyFrom(ctx); }

void SEParser::KeyLabelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKeyLabel(this);
}
void SEParser::KeyLabelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKeyLabel(this);
}
SEParser::KeyContext* SEParser::key() {
  KeyContext *_localctx = _tracker.createInstance<KeyContext>(_ctx, getState());
  enterRule(_localctx, 2, SEParser::RuleKey);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    _localctx = _tracker.createInstance<SEParser::KeyLabelContext>(_localctx);
    enterOuterAlt(_localctx, 1);
    setState(54);
    match(SEParser::Identifier);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueGroupContext ------------------------------------------------------------------

SEParser::ValueGroupContext::ValueGroupContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::ValueGroupContext::getRuleIndex() const {
  return SEParser::RuleValueGroup;
}

void SEParser::ValueGroupContext::copyFrom(ValueGroupContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ValueGroupIteratingContext ------------------------------------------------------------------

SEParser::ValueContext* SEParser::ValueGroupIteratingContext::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::ValueGroupContext* SEParser::ValueGroupIteratingContext::valueGroup() {
  return getRuleContext<SEParser::ValueGroupContext>(0);
}

SEParser::ValueGroupIteratingContext::ValueGroupIteratingContext(ValueGroupContext *ctx) { copyFrom(ctx); }

void SEParser::ValueGroupIteratingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueGroupIterating(this);
}
void SEParser::ValueGroupIteratingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueGroupIterating(this);
}
//----------------- ValueGroupTerminatingContext ------------------------------------------------------------------

SEParser::ValueContext* SEParser::ValueGroupTerminatingContext::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::ValueGroupTerminatingContext::ValueGroupTerminatingContext(ValueGroupContext *ctx) { copyFrom(ctx); }

void SEParser::ValueGroupTerminatingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueGroupTerminating(this);
}
void SEParser::ValueGroupTerminatingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueGroupTerminating(this);
}
SEParser::ValueGroupContext* SEParser::valueGroup() {
  ValueGroupContext *_localctx = _tracker.createInstance<ValueGroupContext>(_ctx, getState());
  enterRule(_localctx, 4, SEParser::RuleValueGroup);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(60);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SEParser::ValueGroupIteratingContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(56);
      value();
      setState(57);
      valueGroup();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SEParser::ValueGroupTerminatingContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(59);
      value();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BoolOpContext ------------------------------------------------------------------

SEParser::BoolOpContext::BoolOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::BoolOpContext::getRuleIndex() const {
  return SEParser::RuleBoolOp;
}

void SEParser::BoolOpContext::copyFrom(BoolOpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- BoolOperationContext ------------------------------------------------------------------

std::vector<SEParser::ComparableTypesContext *> SEParser::BoolOperationContext::comparableTypes() {
  return getRuleContexts<SEParser::ComparableTypesContext>();
}

SEParser::ComparableTypesContext* SEParser::BoolOperationContext::comparableTypes(size_t i) {
  return getRuleContext<SEParser::ComparableTypesContext>(i);
}

SEParser::OpContext* SEParser::BoolOperationContext::op() {
  return getRuleContext<SEParser::OpContext>(0);
}

SEParser::BoolOperationContext::BoolOperationContext(BoolOpContext *ctx) { copyFrom(ctx); }

void SEParser::BoolOperationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBoolOperation(this);
}
void SEParser::BoolOperationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBoolOperation(this);
}
SEParser::BoolOpContext* SEParser::boolOp() {
  BoolOpContext *_localctx = _tracker.createInstance<BoolOpContext>(_ctx, getState());
  enterRule(_localctx, 6, SEParser::RuleBoolOp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    _localctx = _tracker.createInstance<SEParser::BoolOperationContext>(_localctx);
    enterOuterAlt(_localctx, 1);
    setState(62);
    comparableTypes();
    setState(63);
    op();
    setState(64);
    comparableTypes();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableOpContext ------------------------------------------------------------------

SEParser::VariableOpContext::VariableOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::VariableOpContext::getRuleIndex() const {
  return SEParser::RuleVariableOp;
}

void SEParser::VariableOpContext::copyFrom(VariableOpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- VariableOperationContext ------------------------------------------------------------------

SEParser::ModifierContext* SEParser::VariableOperationContext::modifier() {
  return getRuleContext<SEParser::ModifierContext>(0);
}

tree::TerminalNode* SEParser::VariableOperationContext::Identifier() {
  return getToken(SEParser::Identifier, 0);
}

SEParser::SimpleTypesContext* SEParser::VariableOperationContext::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::VariableOperationContext::VariableOperationContext(VariableOpContext *ctx) { copyFrom(ctx); }

void SEParser::VariableOperationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableOperation(this);
}
void SEParser::VariableOperationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableOperation(this);
}
SEParser::VariableOpContext* SEParser::variableOp() {
  VariableOpContext *_localctx = _tracker.createInstance<VariableOpContext>(_ctx, getState());
  enterRule(_localctx, 8, SEParser::RuleVariableOp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    _localctx = _tracker.createInstance<SEParser::VariableOperationContext>(_localctx);
    enterOuterAlt(_localctx, 1);
    setState(66);
    modifier();
    setState(67);
    match(SEParser::Identifier);
    setState(68);
    simpleTypes();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ValueContext ------------------------------------------------------------------

SEParser::ValueContext::ValueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::ValueContext::getRuleIndex() const {
  return SEParser::RuleValue;
}

void SEParser::ValueContext::copyFrom(ValueContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ValueAsTupleContext ------------------------------------------------------------------

SEParser::TupleContext* SEParser::ValueAsTupleContext::tuple() {
  return getRuleContext<SEParser::TupleContext>(0);
}

SEParser::ValueAsTupleContext::ValueAsTupleContext(ValueContext *ctx) { copyFrom(ctx); }

void SEParser::ValueAsTupleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueAsTuple(this);
}
void SEParser::ValueAsTupleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueAsTuple(this);
}
//----------------- ValueAsArrayContext ------------------------------------------------------------------

SEParser::ArrayContext* SEParser::ValueAsArrayContext::array() {
  return getRuleContext<SEParser::ArrayContext>(0);
}

SEParser::ValueAsArrayContext::ValueAsArrayContext(ValueContext *ctx) { copyFrom(ctx); }

void SEParser::ValueAsArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueAsArray(this);
}
void SEParser::ValueAsArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueAsArray(this);
}
//----------------- ValueAsRawTypesContext ------------------------------------------------------------------

SEParser::SimpleTypesContext* SEParser::ValueAsRawTypesContext::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::ValueAsRawTypesContext::ValueAsRawTypesContext(ValueContext *ctx) { copyFrom(ctx); }

void SEParser::ValueAsRawTypesContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueAsRawTypes(this);
}
void SEParser::ValueAsRawTypesContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueAsRawTypes(this);
}
SEParser::ValueContext* SEParser::value() {
  ValueContext *_localctx = _tracker.createInstance<ValueContext>(_ctx, getState());
  enterRule(_localctx, 10, SEParser::RuleValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(79);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SEParser::Boolean:
      case SEParser::Numeric:
      case SEParser::String: {
        _localctx = _tracker.createInstance<SEParser::ValueAsRawTypesContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(70);
        simpleTypes();
        break;
      }

      case SEParser::T__2: {
        _localctx = _tracker.createInstance<SEParser::ValueAsArrayContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(71);
        match(SEParser::T__2);
        setState(72);
        array(0);
        setState(73);
        match(SEParser::T__3);
        break;
      }

      case SEParser::T__0: {
        _localctx = _tracker.createInstance<SEParser::ValueAsTupleContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(75);
        match(SEParser::T__0);
        setState(76);
        tuple();
        setState(77);
        match(SEParser::T__1);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SubTableContext ------------------------------------------------------------------

SEParser::SubTableContext::SubTableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::SubTableContext::getRuleIndex() const {
  return SEParser::RuleSubTable;
}

void SEParser::SubTableContext::copyFrom(SubTableContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- SubTableExpandContext ------------------------------------------------------------------

SEParser::TableContext* SEParser::SubTableExpandContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::SubTableExpandContext::SubTableExpandContext(SubTableContext *ctx) { copyFrom(ctx); }

void SEParser::SubTableExpandContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSubTableExpand(this);
}
void SEParser::SubTableExpandContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSubTableExpand(this);
}
SEParser::SubTableContext* SEParser::subTable() {
  SubTableContext *_localctx = _tracker.createInstance<SubTableContext>(_ctx, getState());
  enterRule(_localctx, 12, SEParser::RuleSubTable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    _localctx = _tracker.createInstance<SEParser::SubTableExpandContext>(_localctx);
    enterOuterAlt(_localctx, 1);
    setState(81);
    match(SEParser::T__0);
    setState(82);
    table();
    setState(83);
    match(SEParser::T__1);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SimpleTypesContext ------------------------------------------------------------------

SEParser::SimpleTypesContext::SimpleTypesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::SimpleTypesContext::getRuleIndex() const {
  return SEParser::RuleSimpleTypes;
}

void SEParser::SimpleTypesContext::copyFrom(SimpleTypesContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- SimpleTypeStringContext ------------------------------------------------------------------

tree::TerminalNode* SEParser::SimpleTypeStringContext::String() {
  return getToken(SEParser::String, 0);
}

SEParser::SimpleTypeStringContext::SimpleTypeStringContext(SimpleTypesContext *ctx) { copyFrom(ctx); }

void SEParser::SimpleTypeStringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleTypeString(this);
}
void SEParser::SimpleTypeStringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleTypeString(this);
}
//----------------- SimpleTypeBooleanContext ------------------------------------------------------------------

tree::TerminalNode* SEParser::SimpleTypeBooleanContext::Boolean() {
  return getToken(SEParser::Boolean, 0);
}

SEParser::SimpleTypeBooleanContext::SimpleTypeBooleanContext(SimpleTypesContext *ctx) { copyFrom(ctx); }

void SEParser::SimpleTypeBooleanContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleTypeBoolean(this);
}
void SEParser::SimpleTypeBooleanContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleTypeBoolean(this);
}
//----------------- SimpleTypeNumericContext ------------------------------------------------------------------

tree::TerminalNode* SEParser::SimpleTypeNumericContext::Numeric() {
  return getToken(SEParser::Numeric, 0);
}

SEParser::SimpleTypeNumericContext::SimpleTypeNumericContext(SimpleTypesContext *ctx) { copyFrom(ctx); }

void SEParser::SimpleTypeNumericContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleTypeNumeric(this);
}
void SEParser::SimpleTypeNumericContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleTypeNumeric(this);
}
SEParser::SimpleTypesContext* SEParser::simpleTypes() {
  SimpleTypesContext *_localctx = _tracker.createInstance<SimpleTypesContext>(_ctx, getState());
  enterRule(_localctx, 14, SEParser::RuleSimpleTypes);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(88);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SEParser::Numeric: {
        _localctx = _tracker.createInstance<SEParser::SimpleTypeNumericContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(85);
        match(SEParser::Numeric);
        break;
      }

      case SEParser::String: {
        _localctx = _tracker.createInstance<SEParser::SimpleTypeStringContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(86);
        match(SEParser::String);
        break;
      }

      case SEParser::Boolean: {
        _localctx = _tracker.createInstance<SEParser::SimpleTypeBooleanContext>(_localctx);
        enterOuterAlt(_localctx, 3);
        setState(87);
        match(SEParser::Boolean);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArrayContext ------------------------------------------------------------------

SEParser::ArrayContext::ArrayContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::ArrayContext::getRuleIndex() const {
  return SEParser::RuleArray;
}

void SEParser::ArrayContext::copyFrom(ArrayContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ArrayIterating2Context ------------------------------------------------------------------

SEParser::ArrayContext* SEParser::ArrayIterating2Context::array() {
  return getRuleContext<SEParser::ArrayContext>(0);
}

SEParser::SimpleTypesContext* SEParser::ArrayIterating2Context::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::ArrayIterating2Context::ArrayIterating2Context(ArrayContext *ctx) { copyFrom(ctx); }

void SEParser::ArrayIterating2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayIterating2(this);
}
void SEParser::ArrayIterating2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayIterating2(this);
}
//----------------- ArrayIterating1Context ------------------------------------------------------------------

SEParser::ArrayContext* SEParser::ArrayIterating1Context::array() {
  return getRuleContext<SEParser::ArrayContext>(0);
}

SEParser::SimpleTypesContext* SEParser::ArrayIterating1Context::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::ArrayIterating1Context::ArrayIterating1Context(ArrayContext *ctx) { copyFrom(ctx); }

void SEParser::ArrayIterating1Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayIterating1(this);
}
void SEParser::ArrayIterating1Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayIterating1(this);
}
//----------------- ArrayStartContext ------------------------------------------------------------------

SEParser::SimpleTypesContext* SEParser::ArrayStartContext::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::ArrayStartContext::ArrayStartContext(ArrayContext *ctx) { copyFrom(ctx); }

void SEParser::ArrayStartContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArrayStart(this);
}
void SEParser::ArrayStartContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArrayStart(this);
}

SEParser::ArrayContext* SEParser::array() {
   return array(0);
}

SEParser::ArrayContext* SEParser::array(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SEParser::ArrayContext *_localctx = _tracker.createInstance<ArrayContext>(_ctx, parentState);
  SEParser::ArrayContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 16;
  enterRecursionRule(_localctx, 16, SEParser::RuleArray, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    _localctx = _tracker.createInstance<ArrayStartContext>(_localctx);
    _ctx = _localctx;
    previousContext = _localctx;

    setState(91);
    simpleTypes();
    _ctx->stop = _input->LT(-1);
    setState(100);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(98);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<ArrayIterating1Context>(_tracker.createInstance<ArrayContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleArray);
          setState(93);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(94);
          match(SEParser::T__4);
          setState(95);
          simpleTypes();
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<ArrayIterating2Context>(_tracker.createInstance<ArrayContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleArray);
          setState(96);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(97);
          simpleTypes();
          break;
        }

        default:
          break;
        } 
      }
      setState(102);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- TupleContext ------------------------------------------------------------------

SEParser::TupleContext::TupleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::TupleContext::getRuleIndex() const {
  return SEParser::RuleTuple;
}

void SEParser::TupleContext::copyFrom(TupleContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- TupleIterating2Context ------------------------------------------------------------------

SEParser::ValueContext* SEParser::TupleIterating2Context::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::TupleContext* SEParser::TupleIterating2Context::tuple() {
  return getRuleContext<SEParser::TupleContext>(0);
}

SEParser::TupleIterating2Context::TupleIterating2Context(TupleContext *ctx) { copyFrom(ctx); }

void SEParser::TupleIterating2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTupleIterating2(this);
}
void SEParser::TupleIterating2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTupleIterating2(this);
}
//----------------- TupleIterating1Context ------------------------------------------------------------------

SEParser::ValueContext* SEParser::TupleIterating1Context::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::TupleContext* SEParser::TupleIterating1Context::tuple() {
  return getRuleContext<SEParser::TupleContext>(0);
}

SEParser::TupleIterating1Context::TupleIterating1Context(TupleContext *ctx) { copyFrom(ctx); }

void SEParser::TupleIterating1Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTupleIterating1(this);
}
void SEParser::TupleIterating1Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTupleIterating1(this);
}
//----------------- TupleTerminating2Context ------------------------------------------------------------------

SEParser::ValueContext* SEParser::TupleTerminating2Context::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::TupleTerminating2Context::TupleTerminating2Context(TupleContext *ctx) { copyFrom(ctx); }

void SEParser::TupleTerminating2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTupleTerminating2(this);
}
void SEParser::TupleTerminating2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTupleTerminating2(this);
}
//----------------- TupleTerminating1Context ------------------------------------------------------------------

SEParser::ValueContext* SEParser::TupleTerminating1Context::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::TupleTerminating1Context::TupleTerminating1Context(TupleContext *ctx) { copyFrom(ctx); }

void SEParser::TupleTerminating1Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTupleTerminating1(this);
}
void SEParser::TupleTerminating1Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTupleTerminating1(this);
}
SEParser::TupleContext* SEParser::tuple() {
  TupleContext *_localctx = _tracker.createInstance<TupleContext>(_ctx, getState());
  enterRule(_localctx, 18, SEParser::RuleTuple);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(114);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<SEParser::TupleTerminating1Context>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(103);
      value();
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SEParser::TupleIterating1Context>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(104);
      value();
      setState(105);
      match(SEParser::T__4);
      setState(106);
      tuple();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<SEParser::TupleIterating2Context>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(108);
      value();
      setState(109);
      tuple();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<SEParser::TupleTerminating2Context>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(111);
      value();
      setState(112);
      match(SEParser::T__4);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ComparableTypesContext ------------------------------------------------------------------

SEParser::ComparableTypesContext::ComparableTypesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t SEParser::ComparableTypesContext::getRuleIndex() const {
  return SEParser::RuleComparableTypes;
}

void SEParser::ComparableTypesContext::copyFrom(ComparableTypesContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- CompTypeAsIdentContext ------------------------------------------------------------------

tree::TerminalNode* SEParser::CompTypeAsIdentContext::Identifier() {
  return getToken(SEParser::Identifier, 0);
}

SEParser::CompTypeAsIdentContext::CompTypeAsIdentContext(ComparableTypesContext *ctx) { copyFrom(ctx); }

void SEParser::CompTypeAsIdentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCompTypeAsIdent(this);
}
void SEParser::CompTypeAsIdentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCompTypeAsIdent(this);
}
//----------------- CompTypeAsRawTypesContext ------------------------------------------------------------------

SEParser::SimpleTypesContext* SEParser::CompTypeAsRawTypesContext::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::CompTypeAsRawTypesContext::CompTypeAsRawTypesContext(ComparableTypesContext *ctx) { copyFrom(ctx); }

void SEParser::CompTypeAsRawTypesContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCompTypeAsRawTypes(this);
}
void SEParser::CompTypeAsRawTypesContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCompTypeAsRawTypes(this);
}
SEParser::ComparableTypesContext* SEParser::comparableTypes() {
  ComparableTypesContext *_localctx = _tracker.createInstance<ComparableTypesContext>(_ctx, getState());
  enterRule(_localctx, 20, SEParser::RuleComparableTypes);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(118);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SEParser::Identifier: {
        _localctx = _tracker.createInstance<SEParser::CompTypeAsIdentContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(116);
        match(SEParser::Identifier);
        break;
      }

      case SEParser::Boolean:
      case SEParser::Numeric:
      case SEParser::String: {
        _localctx = _tracker.createInstance<SEParser::CompTypeAsRawTypesContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(117);
        simpleTypes();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ModifierContext ------------------------------------------------------------------

SEParser::ModifierContext::ModifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SEParser::ModifierContext::Set() {
  return getToken(SEParser::Set, 0);
}

tree::TerminalNode* SEParser::ModifierContext::SetU() {
  return getToken(SEParser::SetU, 0);
}

tree::TerminalNode* SEParser::ModifierContext::SetForce() {
  return getToken(SEParser::SetForce, 0);
}

tree::TerminalNode* SEParser::ModifierContext::Reset() {
  return getToken(SEParser::Reset, 0);
}

tree::TerminalNode* SEParser::ModifierContext::Interpolate() {
  return getToken(SEParser::Interpolate, 0);
}

tree::TerminalNode* SEParser::ModifierContext::Get() {
  return getToken(SEParser::Get, 0);
}

tree::TerminalNode* SEParser::ModifierContext::WaitVar() {
  return getToken(SEParser::WaitVar, 0);
}

tree::TerminalNode* SEParser::ModifierContext::SaveVars() {
  return getToken(SEParser::SaveVars, 0);
}

tree::TerminalNode* SEParser::ModifierContext::RestoreVars() {
  return getToken(SEParser::RestoreVars, 0);
}


size_t SEParser::ModifierContext::getRuleIndex() const {
  return SEParser::RuleModifier;
}

void SEParser::ModifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterModifier(this);
}

void SEParser::ModifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitModifier(this);
}

SEParser::ModifierContext* SEParser::modifier() {
  ModifierContext *_localctx = _tracker.createInstance<ModifierContext>(_ctx, getState());
  enterRule(_localctx, 22, SEParser::RuleModifier);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(120);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 32704) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OpContext ------------------------------------------------------------------

SEParser::OpContext::OpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SEParser::OpContext::Op() {
  return getToken(SEParser::Op, 0);
}


size_t SEParser::OpContext::getRuleIndex() const {
  return SEParser::RuleOp;
}

void SEParser::OpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOp(this);
}

void SEParser::OpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOp(this);
}

SEParser::OpContext* SEParser::op() {
  OpContext *_localctx = _tracker.createInstance<OpContext>(_ctx, getState());
  enterRule(_localctx, 24, SEParser::RuleOp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(122);
    match(SEParser::Op);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool SEParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 8: return arraySempred(antlrcpp::downCast<ArrayContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool SEParser::arraySempred(ArrayContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 2);
    case 1: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

void SEParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  seParserInitialize();
#else
  ::antlr4::internal::call_once(seParserOnceFlag, seParserInitialize);
#endif
}
