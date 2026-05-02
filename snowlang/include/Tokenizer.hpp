#pragma once

#include <string>
#include <vector>

namespace Snowlang {

enum class TokenType : std::uint8_t {
  Identifier,
  Number,
  LParen,
  RParen,
  LBrace,
  RBrace,
  LBracket,
  RBracket,
  Equal,
  Star,
  At,
  Dollar,
  String,
  Function,
  Comma,
  Plus,
  Minus,
  Slash,
  EqualityComparison,
  GreaterThan,
  LesserThan,
  GreaterOrEqualThan,
  LesserOrEqualThan,
  Sequence,
  EndOfFile,
  ImportSTMT,
  From,
  LetSTMT,
  ReturnSTMT,
  IfSTMT,
  ThenSTMT,
  ElseSTMT,
  DoSTMT,
  DoneSTMT,
  WhileSTMT,
  BreakSTMT,
  ContinueSTMT,
  Obj,
  EndSTMT,
  Dot,
  This,
  True,
  False,
  Flag,
  LogicalAnd,
  LogicalOr,
  Modulo
};

struct SourceSpan {
  size_t start;
  size_t end;
  size_t line;
  size_t column;
};

struct Token {
  TokenType type;
  std::string text;
  SourceSpan span;
};

class Tokenizer {
public:
  explicit Tokenizer(const std::string &source);
  std::vector<Token> tokenize();

private:
  const std::string &input;

  size_t pos = 0;
  size_t line = 1;
  size_t column = 1;

  [[nodiscard]] char peek() const;
  char advance();
  bool match(char expected);
  [[nodiscard]] bool isAtEnd() const;

  enum class MergeType : std::uint8_t { Left, Right };

  void mergeTokens(std::vector<Token> &tokenVec, TokenType left, TokenType right,
                   MergeType mergeDir);

  void emit(std::vector<Token> &tokens, TokenType type, size_t startPos, size_t endPos,
            size_t startLine, size_t startColumn, const std::string &lexeme);

  void emitFlag(std::vector<Token> &tokens, size_t startPos, size_t startLine, size_t startColumn);

  void skipWhitespace();
};

} // namespace Snowlang