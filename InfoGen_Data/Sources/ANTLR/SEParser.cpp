
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
      "simpleTypes", "array", "tuple", "modifier", "op"
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
  	4,1,22,106,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
  	1,0,1,0,1,0,3,0,35,8,0,1,0,1,0,1,0,3,0,40,8,0,1,1,1,1,1,2,1,2,1,2,1,2,
  	1,2,3,2,49,8,2,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,
  	5,1,5,1,5,1,5,3,5,68,8,5,1,6,1,6,1,6,1,6,1,7,1,7,1,8,1,8,1,8,1,8,1,8,
  	1,8,1,8,1,8,5,8,84,8,8,10,8,12,8,87,9,8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,
  	9,1,9,1,9,1,9,3,9,100,8,9,1,10,1,10,1,11,1,11,1,11,0,1,16,12,0,2,4,6,
  	8,10,12,14,16,18,20,22,0,2,2,0,15,15,17,18,1,0,6,14,106,0,39,1,0,0,0,
  	2,41,1,0,0,0,4,48,1,0,0,0,6,50,1,0,0,0,8,54,1,0,0,0,10,67,1,0,0,0,12,
  	69,1,0,0,0,14,73,1,0,0,0,16,75,1,0,0,0,18,99,1,0,0,0,20,101,1,0,0,0,22,
  	103,1,0,0,0,24,25,3,2,1,0,25,26,3,4,2,0,26,35,1,0,0,0,27,28,3,2,1,0,28,
  	29,5,1,0,0,29,30,3,6,3,0,30,31,5,2,0,0,31,35,1,0,0,0,32,35,3,8,4,0,33,
  	35,3,2,1,0,34,24,1,0,0,0,34,27,1,0,0,0,34,32,1,0,0,0,34,33,1,0,0,0,35,
  	36,1,0,0,0,36,37,3,0,0,0,37,40,1,0,0,0,38,40,1,0,0,0,39,34,1,0,0,0,39,
  	38,1,0,0,0,40,1,1,0,0,0,41,42,5,19,0,0,42,3,1,0,0,0,43,44,3,10,5,0,44,
  	45,3,4,2,0,45,49,1,0,0,0,46,49,3,10,5,0,47,49,3,12,6,0,48,43,1,0,0,0,
  	48,46,1,0,0,0,48,47,1,0,0,0,49,5,1,0,0,0,50,51,3,14,7,0,51,52,3,22,11,
  	0,52,53,3,14,7,0,53,7,1,0,0,0,54,55,3,20,10,0,55,56,5,19,0,0,56,57,3,
  	14,7,0,57,9,1,0,0,0,58,68,3,14,7,0,59,60,5,3,0,0,60,61,3,16,8,0,61,62,
  	5,4,0,0,62,68,1,0,0,0,63,64,5,1,0,0,64,65,3,18,9,0,65,66,5,2,0,0,66,68,
  	1,0,0,0,67,58,1,0,0,0,67,59,1,0,0,0,67,63,1,0,0,0,68,11,1,0,0,0,69,70,
  	5,1,0,0,70,71,3,0,0,0,71,72,5,2,0,0,72,13,1,0,0,0,73,74,7,0,0,0,74,15,
  	1,0,0,0,75,76,6,8,-1,0,76,77,3,14,7,0,77,85,1,0,0,0,78,79,10,2,0,0,79,
  	80,5,5,0,0,80,84,3,14,7,0,81,82,10,1,0,0,82,84,3,14,7,0,83,78,1,0,0,0,
  	83,81,1,0,0,0,84,87,1,0,0,0,85,83,1,0,0,0,85,86,1,0,0,0,86,17,1,0,0,0,
  	87,85,1,0,0,0,88,100,3,10,5,0,89,90,3,10,5,0,90,91,5,5,0,0,91,92,3,18,
  	9,0,92,100,1,0,0,0,93,94,3,10,5,0,94,95,3,18,9,0,95,100,1,0,0,0,96,97,
  	3,10,5,0,97,98,5,5,0,0,98,100,1,0,0,0,99,88,1,0,0,0,99,89,1,0,0,0,99,
  	93,1,0,0,0,99,96,1,0,0,0,100,19,1,0,0,0,101,102,7,1,0,0,102,21,1,0,0,
  	0,103,104,5,16,0,0,104,23,1,0,0,0,7,34,39,48,67,83,85,99
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

SEParser::TableContext* SEParser::TableContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

SEParser::KeyContext* SEParser::TableContext::key() {
  return getRuleContext<SEParser::KeyContext>(0);
}

SEParser::ValueGroupContext* SEParser::TableContext::valueGroup() {
  return getRuleContext<SEParser::ValueGroupContext>(0);
}

SEParser::BoolOpContext* SEParser::TableContext::boolOp() {
  return getRuleContext<SEParser::BoolOpContext>(0);
}

SEParser::VariableOpContext* SEParser::TableContext::variableOp() {
  return getRuleContext<SEParser::VariableOpContext>(0);
}


size_t SEParser::TableContext::getRuleIndex() const {
  return SEParser::RuleTable;
}

void SEParser::TableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTable(this);
}

void SEParser::TableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTable(this);
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
    setState(39);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SEParser::Set:
      case SEParser::SetU:
      case SEParser::SetForce:
      case SEParser::Reset:
      case SEParser::Interpolate:
      case SEParser::Get:
      case SEParser::WaitVar:
      case SEParser::SaveVars:
      case SEParser::RestoreVars:
      case SEParser::Identifier: {
        enterOuterAlt(_localctx, 1);
        setState(34);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
        case 1: {
          setState(24);
          key();
          setState(25);
          valueGroup();
          break;
        }

        case 2: {
          setState(27);
          key();
          setState(28);
          match(SEParser::T__0);
          setState(29);
          boolOp();
          setState(30);
          match(SEParser::T__1);
          break;
        }

        case 3: {
          setState(32);
          variableOp();
          break;
        }

        case 4: {
          setState(33);
          key();
          break;
        }

        default:
          break;
        }
        setState(36);
        table();
        break;
      }

      case SEParser::T__1: {
        enterOuterAlt(_localctx, 2);

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

//----------------- KeyContext ------------------------------------------------------------------

SEParser::KeyContext::KeyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SEParser::KeyContext::Identifier() {
  return getToken(SEParser::Identifier, 0);
}


size_t SEParser::KeyContext::getRuleIndex() const {
  return SEParser::RuleKey;
}

void SEParser::KeyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKey(this);
}

void SEParser::KeyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKey(this);
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
    enterOuterAlt(_localctx, 1);
    setState(41);
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

SEParser::ValueContext* SEParser::ValueGroupContext::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::ValueGroupContext* SEParser::ValueGroupContext::valueGroup() {
  return getRuleContext<SEParser::ValueGroupContext>(0);
}

SEParser::SubTableContext* SEParser::ValueGroupContext::subTable() {
  return getRuleContext<SEParser::SubTableContext>(0);
}


size_t SEParser::ValueGroupContext::getRuleIndex() const {
  return SEParser::RuleValueGroup;
}

void SEParser::ValueGroupContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValueGroup(this);
}

void SEParser::ValueGroupContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValueGroup(this);
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
    setState(48);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(43);
      value();
      setState(44);
      valueGroup();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(46);
      value();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(47);
      subTable();
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

std::vector<SEParser::SimpleTypesContext *> SEParser::BoolOpContext::simpleTypes() {
  return getRuleContexts<SEParser::SimpleTypesContext>();
}

SEParser::SimpleTypesContext* SEParser::BoolOpContext::simpleTypes(size_t i) {
  return getRuleContext<SEParser::SimpleTypesContext>(i);
}

SEParser::OpContext* SEParser::BoolOpContext::op() {
  return getRuleContext<SEParser::OpContext>(0);
}


size_t SEParser::BoolOpContext::getRuleIndex() const {
  return SEParser::RuleBoolOp;
}

void SEParser::BoolOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBoolOp(this);
}

void SEParser::BoolOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBoolOp(this);
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
    enterOuterAlt(_localctx, 1);
    setState(50);
    simpleTypes();
    setState(51);
    op();
    setState(52);
    simpleTypes();
   
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

SEParser::ModifierContext* SEParser::VariableOpContext::modifier() {
  return getRuleContext<SEParser::ModifierContext>(0);
}

tree::TerminalNode* SEParser::VariableOpContext::Identifier() {
  return getToken(SEParser::Identifier, 0);
}

SEParser::SimpleTypesContext* SEParser::VariableOpContext::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}


size_t SEParser::VariableOpContext::getRuleIndex() const {
  return SEParser::RuleVariableOp;
}

void SEParser::VariableOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableOp(this);
}

void SEParser::VariableOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableOp(this);
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
    enterOuterAlt(_localctx, 1);
    setState(54);
    modifier();
    setState(55);
    match(SEParser::Identifier);
    setState(56);
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

SEParser::SimpleTypesContext* SEParser::ValueContext::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::ArrayContext* SEParser::ValueContext::array() {
  return getRuleContext<SEParser::ArrayContext>(0);
}

SEParser::TupleContext* SEParser::ValueContext::tuple() {
  return getRuleContext<SEParser::TupleContext>(0);
}


size_t SEParser::ValueContext::getRuleIndex() const {
  return SEParser::RuleValue;
}

void SEParser::ValueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValue(this);
}

void SEParser::ValueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValue(this);
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
    setState(67);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SEParser::Boolean:
      case SEParser::Numeric:
      case SEParser::String: {
        enterOuterAlt(_localctx, 1);
        setState(58);
        simpleTypes();
        break;
      }

      case SEParser::T__2: {
        enterOuterAlt(_localctx, 2);
        setState(59);
        match(SEParser::T__2);
        setState(60);
        array(0);
        setState(61);
        match(SEParser::T__3);
        break;
      }

      case SEParser::T__0: {
        enterOuterAlt(_localctx, 3);
        setState(63);
        match(SEParser::T__0);
        setState(64);
        tuple();
        setState(65);
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

SEParser::TableContext* SEParser::SubTableContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}


size_t SEParser::SubTableContext::getRuleIndex() const {
  return SEParser::RuleSubTable;
}

void SEParser::SubTableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSubTable(this);
}

void SEParser::SubTableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSubTable(this);
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
    enterOuterAlt(_localctx, 1);
    setState(69);
    match(SEParser::T__0);
    setState(70);
    table();
    setState(71);
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

tree::TerminalNode* SEParser::SimpleTypesContext::Numeric() {
  return getToken(SEParser::Numeric, 0);
}

tree::TerminalNode* SEParser::SimpleTypesContext::String() {
  return getToken(SEParser::String, 0);
}

tree::TerminalNode* SEParser::SimpleTypesContext::Boolean() {
  return getToken(SEParser::Boolean, 0);
}


size_t SEParser::SimpleTypesContext::getRuleIndex() const {
  return SEParser::RuleSimpleTypes;
}

void SEParser::SimpleTypesContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleTypes(this);
}

void SEParser::SimpleTypesContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleTypes(this);
}

SEParser::SimpleTypesContext* SEParser::simpleTypes() {
  SimpleTypesContext *_localctx = _tracker.createInstance<SimpleTypesContext>(_ctx, getState());
  enterRule(_localctx, 14, SEParser::RuleSimpleTypes);
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
    setState(73);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 425984) != 0))) {
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

//----------------- ArrayContext ------------------------------------------------------------------

SEParser::ArrayContext::ArrayContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SEParser::SimpleTypesContext* SEParser::ArrayContext::simpleTypes() {
  return getRuleContext<SEParser::SimpleTypesContext>(0);
}

SEParser::ArrayContext* SEParser::ArrayContext::array() {
  return getRuleContext<SEParser::ArrayContext>(0);
}


size_t SEParser::ArrayContext::getRuleIndex() const {
  return SEParser::RuleArray;
}

void SEParser::ArrayContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArray(this);
}

void SEParser::ArrayContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArray(this);
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
    setState(76);
    simpleTypes();
    _ctx->stop = _input->LT(-1);
    setState(85);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(83);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ArrayContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleArray);
          setState(78);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(79);
          match(SEParser::T__4);
          setState(80);
          simpleTypes();
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ArrayContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleArray);
          setState(81);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(82);
          simpleTypes();
          break;
        }

        default:
          break;
        } 
      }
      setState(87);
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

SEParser::ValueContext* SEParser::TupleContext::value() {
  return getRuleContext<SEParser::ValueContext>(0);
}

SEParser::TupleContext* SEParser::TupleContext::tuple() {
  return getRuleContext<SEParser::TupleContext>(0);
}


size_t SEParser::TupleContext::getRuleIndex() const {
  return SEParser::RuleTuple;
}

void SEParser::TupleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTuple(this);
}

void SEParser::TupleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTuple(this);
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
    setState(99);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(88);
      value();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(89);
      value();
      setState(90);
      match(SEParser::T__4);
      setState(91);
      tuple();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(93);
      value();
      setState(94);
      tuple();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(96);
      value();
      setState(97);
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
  enterRule(_localctx, 20, SEParser::RuleModifier);
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
    setState(101);
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
  enterRule(_localctx, 22, SEParser::RuleOp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(103);
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
