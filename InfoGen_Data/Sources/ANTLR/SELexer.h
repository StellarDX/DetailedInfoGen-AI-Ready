
// Generated from SE.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  SELexer : public antlr4::Lexer {
public:
  enum {
    WS = 1, LINE_COMMENT = 2, BLOCK_COMMENT = 3, LPAREN = 4, RPAREN = 5, 
    LBRACE = 6, RBRACE = 7, COMMA = 8, BOOL = 9, OP = 10, IDENT = 11, STRING = 12, 
    INTEGER = 13, DEC_FLOAT = 14, HEX_FLOAT = 15
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

