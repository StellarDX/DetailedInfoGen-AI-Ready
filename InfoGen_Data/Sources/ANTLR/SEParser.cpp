
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
      "start", "table", "entry", "key", "valueGroup", "boolBlock", "boolOp", 
      "value", "subTable", "simpleTypes", "numeric", "variable", "array", 
      "tuple"
    },
    std::vector<std::string>{
      "", "", "", "", "'('", "')'", "'{'", "'}'", "','"
    },
    std::vector<std::string>{
      "", "WS", "LINE_COMMENT", "BLOCK_COMMENT", "LPAREN", "RPAREN", "LBRACE", 
      "RBRACE", "COMMA", "BOOL", "OP", "IDENT", "STRING", "INTEGER", "DEC_FLOAT", 
      "HEX_FLOAT"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,15,113,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,1,0,1,0,
  	1,0,1,1,5,1,33,8,1,10,1,12,1,36,9,1,1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2,45,
  	8,2,1,3,1,3,1,4,4,4,50,8,4,11,4,12,4,51,1,4,3,4,55,8,4,1,5,1,5,1,5,1,
  	5,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,7,3,7,74,8,7,1,8,
  	1,8,1,8,1,8,1,9,1,9,1,9,1,9,3,9,84,8,9,1,10,1,10,1,11,1,11,1,12,1,12,
  	3,12,92,8,12,1,12,5,12,95,8,12,10,12,12,12,98,9,12,1,13,1,13,3,13,102,
  	8,13,1,13,5,13,105,8,13,10,13,12,13,108,9,13,1,13,3,13,111,8,13,1,13,
  	0,0,14,0,2,4,6,8,10,12,14,16,18,20,22,24,26,0,1,1,0,13,15,113,0,28,1,
  	0,0,0,2,34,1,0,0,0,4,44,1,0,0,0,6,46,1,0,0,0,8,54,1,0,0,0,10,56,1,0,0,
  	0,12,60,1,0,0,0,14,73,1,0,0,0,16,75,1,0,0,0,18,83,1,0,0,0,20,85,1,0,0,
  	0,22,87,1,0,0,0,24,89,1,0,0,0,26,99,1,0,0,0,28,29,3,2,1,0,29,30,5,0,0,
  	1,30,1,1,0,0,0,31,33,3,4,2,0,32,31,1,0,0,0,33,36,1,0,0,0,34,32,1,0,0,
  	0,34,35,1,0,0,0,35,3,1,0,0,0,36,34,1,0,0,0,37,38,3,6,3,0,38,39,3,8,4,
  	0,39,45,1,0,0,0,40,41,3,6,3,0,41,42,3,10,5,0,42,45,1,0,0,0,43,45,3,6,
  	3,0,44,37,1,0,0,0,44,40,1,0,0,0,44,43,1,0,0,0,45,5,1,0,0,0,46,47,5,11,
  	0,0,47,7,1,0,0,0,48,50,3,14,7,0,49,48,1,0,0,0,50,51,1,0,0,0,51,49,1,0,
  	0,0,51,52,1,0,0,0,52,55,1,0,0,0,53,55,3,16,8,0,54,49,1,0,0,0,54,53,1,
  	0,0,0,55,9,1,0,0,0,56,57,5,6,0,0,57,58,3,12,6,0,58,59,5,7,0,0,59,11,1,
  	0,0,0,60,61,3,18,9,0,61,62,5,10,0,0,62,63,3,18,9,0,63,13,1,0,0,0,64,74,
  	3,18,9,0,65,66,5,4,0,0,66,67,3,24,12,0,67,68,5,5,0,0,68,74,1,0,0,0,69,
  	70,5,6,0,0,70,71,3,26,13,0,71,72,5,7,0,0,72,74,1,0,0,0,73,64,1,0,0,0,
  	73,65,1,0,0,0,73,69,1,0,0,0,74,15,1,0,0,0,75,76,5,6,0,0,76,77,3,2,1,0,
  	77,78,5,7,0,0,78,17,1,0,0,0,79,84,3,20,10,0,80,84,5,12,0,0,81,84,5,9,
  	0,0,82,84,3,22,11,0,83,79,1,0,0,0,83,80,1,0,0,0,83,81,1,0,0,0,83,82,1,
  	0,0,0,84,19,1,0,0,0,85,86,7,0,0,0,86,21,1,0,0,0,87,88,5,11,0,0,88,23,
  	1,0,0,0,89,96,3,18,9,0,90,92,5,8,0,0,91,90,1,0,0,0,91,92,1,0,0,0,92,93,
  	1,0,0,0,93,95,3,18,9,0,94,91,1,0,0,0,95,98,1,0,0,0,96,94,1,0,0,0,96,97,
  	1,0,0,0,97,25,1,0,0,0,98,96,1,0,0,0,99,106,3,14,7,0,100,102,5,8,0,0,101,
  	100,1,0,0,0,101,102,1,0,0,0,102,103,1,0,0,0,103,105,3,14,7,0,104,101,
  	1,0,0,0,105,108,1,0,0,0,106,104,1,0,0,0,106,107,1,0,0,0,107,110,1,0,0,
  	0,108,106,1,0,0,0,109,111,5,8,0,0,110,109,1,0,0,0,110,111,1,0,0,0,111,
  	27,1,0,0,0,11,34,44,51,54,73,83,91,96,101,106,110
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


//----------------- StartContext ------------------------------------------------------------------

SEParser::StartContext::StartContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SEParser::TableContext* SEParser::StartContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

tree::TerminalNode* SEParser::StartContext::EOF() {
  return getToken(SEParser::EOF, 0);
}


size_t SEParser::StartContext::getRuleIndex() const {
  return SEParser::RuleStart;
}

void SEParser::StartContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStart(this);
}

void SEParser::StartContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStart(this);
}

SEParser::StartContext* SEParser::start() {
  StartContext *_localctx = _tracker.createInstance<StartContext>(_ctx, getState());
  enterRule(_localctx, 0, SEParser::RuleStart);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(28);
    table();
    setState(29);
    match(SEParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TableContext ------------------------------------------------------------------

SEParser::TableContext::TableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SEParser::EntryContext *> SEParser::TableContext::entry() {
  return getRuleContexts<SEParser::EntryContext>();
}

SEParser::EntryContext* SEParser::TableContext::entry(size_t i) {
  return getRuleContext<SEParser::EntryContext>(i);
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
  enterRule(_localctx, 2, SEParser::RuleTable);
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
    setState(34);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SEParser::IDENT) {
      setState(31);
      entry();
      setState(36);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EntryContext ------------------------------------------------------------------

SEParser::EntryContext::EntryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SEParser::KeyContext* SEParser::EntryContext::key() {
  return getRuleContext<SEParser::KeyContext>(0);
}

SEParser::ValueGroupContext* SEParser::EntryContext::valueGroup() {
  return getRuleContext<SEParser::ValueGroupContext>(0);
}

SEParser::BoolBlockContext* SEParser::EntryContext::boolBlock() {
  return getRuleContext<SEParser::BoolBlockContext>(0);
}


size_t SEParser::EntryContext::getRuleIndex() const {
  return SEParser::RuleEntry;
}

void SEParser::EntryContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEntry(this);
}

void SEParser::EntryContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEntry(this);
}

SEParser::EntryContext* SEParser::entry() {
  EntryContext *_localctx = _tracker.createInstance<EntryContext>(_ctx, getState());
  enterRule(_localctx, 4, SEParser::RuleEntry);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(44);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(37);
      key();
      setState(38);
      valueGroup();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(40);
      key();
      setState(41);
      boolBlock();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(43);
      key();
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

tree::TerminalNode* SEParser::KeyContext::IDENT() {
  return getToken(SEParser::IDENT, 0);
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
  enterRule(_localctx, 6, SEParser::RuleKey);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(46);
    match(SEParser::IDENT);
   
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

std::vector<SEParser::ValueContext *> SEParser::ValueGroupContext::value() {
  return getRuleContexts<SEParser::ValueContext>();
}

SEParser::ValueContext* SEParser::ValueGroupContext::value(size_t i) {
  return getRuleContext<SEParser::ValueContext>(i);
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
  enterRule(_localctx, 8, SEParser::RuleValueGroup);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    setState(54);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(49); 
      _errHandler->sync(this);
      alt = 1;
      do {
        switch (alt) {
          case 1: {
                setState(48);
                value();
                break;
              }

        default:
          throw NoViableAltException(this);
        }
        setState(51); 
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
      } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(53);
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

//----------------- BoolBlockContext ------------------------------------------------------------------

SEParser::BoolBlockContext::BoolBlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SEParser::BoolBlockContext::LBRACE() {
  return getToken(SEParser::LBRACE, 0);
}

SEParser::BoolOpContext* SEParser::BoolBlockContext::boolOp() {
  return getRuleContext<SEParser::BoolOpContext>(0);
}

tree::TerminalNode* SEParser::BoolBlockContext::RBRACE() {
  return getToken(SEParser::RBRACE, 0);
}


size_t SEParser::BoolBlockContext::getRuleIndex() const {
  return SEParser::RuleBoolBlock;
}

void SEParser::BoolBlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBoolBlock(this);
}

void SEParser::BoolBlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBoolBlock(this);
}

SEParser::BoolBlockContext* SEParser::boolBlock() {
  BoolBlockContext *_localctx = _tracker.createInstance<BoolBlockContext>(_ctx, getState());
  enterRule(_localctx, 10, SEParser::RuleBoolBlock);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(56);
    match(SEParser::LBRACE);
    setState(57);
    boolOp();
    setState(58);
    match(SEParser::RBRACE);
   
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

tree::TerminalNode* SEParser::BoolOpContext::OP() {
  return getToken(SEParser::OP, 0);
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
  enterRule(_localctx, 12, SEParser::RuleBoolOp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(60);
    simpleTypes();
    setState(61);
    match(SEParser::OP);
    setState(62);
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

tree::TerminalNode* SEParser::ValueContext::LPAREN() {
  return getToken(SEParser::LPAREN, 0);
}

SEParser::ArrayContext* SEParser::ValueContext::array() {
  return getRuleContext<SEParser::ArrayContext>(0);
}

tree::TerminalNode* SEParser::ValueContext::RPAREN() {
  return getToken(SEParser::RPAREN, 0);
}

tree::TerminalNode* SEParser::ValueContext::LBRACE() {
  return getToken(SEParser::LBRACE, 0);
}

SEParser::TupleContext* SEParser::ValueContext::tuple() {
  return getRuleContext<SEParser::TupleContext>(0);
}

tree::TerminalNode* SEParser::ValueContext::RBRACE() {
  return getToken(SEParser::RBRACE, 0);
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
  enterRule(_localctx, 14, SEParser::RuleValue);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(73);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SEParser::BOOL:
      case SEParser::IDENT:
      case SEParser::STRING:
      case SEParser::INTEGER:
      case SEParser::DEC_FLOAT:
      case SEParser::HEX_FLOAT: {
        enterOuterAlt(_localctx, 1);
        setState(64);
        simpleTypes();
        break;
      }

      case SEParser::LPAREN: {
        enterOuterAlt(_localctx, 2);
        setState(65);
        match(SEParser::LPAREN);
        setState(66);
        array();
        setState(67);
        match(SEParser::RPAREN);
        break;
      }

      case SEParser::LBRACE: {
        enterOuterAlt(_localctx, 3);
        setState(69);
        match(SEParser::LBRACE);
        setState(70);
        tuple();
        setState(71);
        match(SEParser::RBRACE);
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

tree::TerminalNode* SEParser::SubTableContext::LBRACE() {
  return getToken(SEParser::LBRACE, 0);
}

SEParser::TableContext* SEParser::SubTableContext::table() {
  return getRuleContext<SEParser::TableContext>(0);
}

tree::TerminalNode* SEParser::SubTableContext::RBRACE() {
  return getToken(SEParser::RBRACE, 0);
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
  enterRule(_localctx, 16, SEParser::RuleSubTable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    match(SEParser::LBRACE);
    setState(76);
    table();
    setState(77);
    match(SEParser::RBRACE);
   
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

SEParser::NumericContext* SEParser::SimpleTypesContext::numeric() {
  return getRuleContext<SEParser::NumericContext>(0);
}

tree::TerminalNode* SEParser::SimpleTypesContext::STRING() {
  return getToken(SEParser::STRING, 0);
}

tree::TerminalNode* SEParser::SimpleTypesContext::BOOL() {
  return getToken(SEParser::BOOL, 0);
}

SEParser::VariableContext* SEParser::SimpleTypesContext::variable() {
  return getRuleContext<SEParser::VariableContext>(0);
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
  enterRule(_localctx, 18, SEParser::RuleSimpleTypes);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(83);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SEParser::INTEGER:
      case SEParser::DEC_FLOAT:
      case SEParser::HEX_FLOAT: {
        enterOuterAlt(_localctx, 1);
        setState(79);
        numeric();
        break;
      }

      case SEParser::STRING: {
        enterOuterAlt(_localctx, 2);
        setState(80);
        match(SEParser::STRING);
        break;
      }

      case SEParser::BOOL: {
        enterOuterAlt(_localctx, 3);
        setState(81);
        match(SEParser::BOOL);
        break;
      }

      case SEParser::IDENT: {
        enterOuterAlt(_localctx, 4);
        setState(82);
        variable();
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

//----------------- NumericContext ------------------------------------------------------------------

SEParser::NumericContext::NumericContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SEParser::NumericContext::INTEGER() {
  return getToken(SEParser::INTEGER, 0);
}

tree::TerminalNode* SEParser::NumericContext::DEC_FLOAT() {
  return getToken(SEParser::DEC_FLOAT, 0);
}

tree::TerminalNode* SEParser::NumericContext::HEX_FLOAT() {
  return getToken(SEParser::HEX_FLOAT, 0);
}


size_t SEParser::NumericContext::getRuleIndex() const {
  return SEParser::RuleNumeric;
}

void SEParser::NumericContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumeric(this);
}

void SEParser::NumericContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumeric(this);
}

SEParser::NumericContext* SEParser::numeric() {
  NumericContext *_localctx = _tracker.createInstance<NumericContext>(_ctx, getState());
  enterRule(_localctx, 20, SEParser::RuleNumeric);
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
    setState(85);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 57344) != 0))) {
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

//----------------- VariableContext ------------------------------------------------------------------

SEParser::VariableContext::VariableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SEParser::VariableContext::IDENT() {
  return getToken(SEParser::IDENT, 0);
}


size_t SEParser::VariableContext::getRuleIndex() const {
  return SEParser::RuleVariable;
}

void SEParser::VariableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariable(this);
}

void SEParser::VariableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<SEListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariable(this);
}

SEParser::VariableContext* SEParser::variable() {
  VariableContext *_localctx = _tracker.createInstance<VariableContext>(_ctx, getState());
  enterRule(_localctx, 22, SEParser::RuleVariable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(87);
    match(SEParser::IDENT);
   
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

std::vector<SEParser::SimpleTypesContext *> SEParser::ArrayContext::simpleTypes() {
  return getRuleContexts<SEParser::SimpleTypesContext>();
}

SEParser::SimpleTypesContext* SEParser::ArrayContext::simpleTypes(size_t i) {
  return getRuleContext<SEParser::SimpleTypesContext>(i);
}

std::vector<tree::TerminalNode *> SEParser::ArrayContext::COMMA() {
  return getTokens(SEParser::COMMA);
}

tree::TerminalNode* SEParser::ArrayContext::COMMA(size_t i) {
  return getToken(SEParser::COMMA, i);
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
  ArrayContext *_localctx = _tracker.createInstance<ArrayContext>(_ctx, getState());
  enterRule(_localctx, 24, SEParser::RuleArray);
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
    setState(89);
    simpleTypes();
    setState(96);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 64256) != 0)) {
      setState(91);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == SEParser::COMMA) {
        setState(90);
        match(SEParser::COMMA);
      }
      setState(93);
      simpleTypes();
      setState(98);
      _errHandler->sync(this);
      _la = _input->LA(1);
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

std::vector<SEParser::ValueContext *> SEParser::TupleContext::value() {
  return getRuleContexts<SEParser::ValueContext>();
}

SEParser::ValueContext* SEParser::TupleContext::value(size_t i) {
  return getRuleContext<SEParser::ValueContext>(i);
}

std::vector<tree::TerminalNode *> SEParser::TupleContext::COMMA() {
  return getTokens(SEParser::COMMA);
}

tree::TerminalNode* SEParser::TupleContext::COMMA(size_t i) {
  return getToken(SEParser::COMMA, i);
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
  enterRule(_localctx, 26, SEParser::RuleTuple);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(99);
    value();
    setState(106);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(101);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == SEParser::COMMA) {
          setState(100);
          match(SEParser::COMMA);
        }
        setState(103);
        value(); 
      }
      setState(108);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx);
    }
    setState(110);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SEParser::COMMA) {
      setState(109);
      match(SEParser::COMMA);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void SEParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  seParserInitialize();
#else
  ::antlr4::internal::call_once(seParserOnceFlag, seParserInitialize);
#endif
}
