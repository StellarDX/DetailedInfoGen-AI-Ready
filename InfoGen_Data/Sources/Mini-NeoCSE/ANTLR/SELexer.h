
// Generated from SE.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SELexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, Set = 6, SetU = 7, 
    SetForce = 8, Reset = 9, Interpolate = 10, Get = 11, WaitVar = 12, SaveVars = 13, 
    RestoreVars = 14, Boolean = 15, Op = 16, Numeric = 17, String = 18, 
    Identifier = 19, WS = 20, LINE_COMMENT = 21, BLOCK_COMMENT = 22
  };

  explicit SELexer(antlr4::CharStream *input);

  ~SELexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

