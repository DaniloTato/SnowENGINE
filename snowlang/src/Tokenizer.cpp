#include "Tokenizer.hpp"
#include "SnowErr.hpp"
#include <cctype>
#include <cstddef>

namespace Snowlang {

Tokenizer::Tokenizer(const std::string &source) : input(source) {}

void Tokenizer::mergeTokens(std::vector<Token> &tokenVec, TokenType left, TokenType right,
                            MergeType type) {
  for (size_t i = tokenVec.size() - 1; i > 0; --i) {
    if (tokenVec[i - 1].type == left && tokenVec[i].type == right) {
      switch (type) {
      case MergeType::Left: {
        tokenVec.erase(tokenVec.begin() + static_cast<long>(i));
        break;
      }
      case MergeType::Right: {
        tokenVec.erase(tokenVec.begin() + (long)i - 1);
        break;
      }
      }
    }
  }
}

char Tokenizer::peek() const { return isAtEnd() ? '\0' : input[pos]; }

char Tokenizer::advance() {
  char c = input[pos++];

  if (c == '\n') {
    line++;
    column = 1;
  } else {
    column++;
  }

  return c;
}

bool Tokenizer::match(char expected) {
  if (isAtEnd())
    return false;
  if (input[pos] != expected)
    return false;
  advance();
  return true;
}

bool Tokenizer::isAtEnd() const { return pos >= input.size(); }

void Tokenizer::skipWhitespace() {
  while (!isAtEnd()) {
    char c = peek();
    if (std::isspace(c))
      advance();
    else
      break;
  }
}

void Tokenizer::emit(std::vector<Token> &tokens, TokenType type, size_t startPos, size_t endPos,
                     size_t startLine, size_t startColumn, const std::string &lexeme) {
  tokens.push_back(Token{
      .type = type,
      .text = lexeme,
      .span =
          SourceSpan{.start = startPos, .end = endPos, .line = startLine, .column = startColumn}});
}

void Tokenizer::emitFlag(std::vector<Token> &tokens, size_t startPos, size_t startLine,
                         size_t startColumn) {
  size_t nameStart = pos;

  if (!std::isalpha(peek()) && peek() != '_') {
    throwError(SnowErr::Phase::Tokenizer, "expected flag name after '--'",
               SourceSpan{.start = startPos, .end = pos, .line = startLine, .column = startColumn});
  }

  while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
    advance();
  }

  std::string flagName = input.substr(nameStart, pos - nameStart);
  std::string flagValue;

  if (match('=')) {
    if (!isAtEnd() && (peek() == '{' || peek() == '(')) {
      char openChar = advance();
      char closeChar = (openChar == '{') ? '}' : ')';

      int depth = 1;
      size_t valueStart = pos;
      while (!isAtEnd() && depth > 0) {
        char c = advance();
        if (c == openChar)
          depth++;
        else if (c == closeChar)
          depth--;
      }

      if (depth != 0) {
        throwError(
            SnowErr::Phase::Tokenizer, "unmatched delimiter in flag value",
            SourceSpan{
                .start = valueStart - 1, .end = pos, .line = startLine, .column = startColumn});
      }

      flagValue =
          std::string(1, openChar) + input.substr(valueStart, pos - valueStart - 1) + closeChar;
    } else {
      size_t valueStart = pos;
      while (!isAtEnd() && !std::isspace(peek()) && peek() != ';') {
        advance();
      }
      flagValue = input.substr(valueStart, pos - valueStart);
    }
  }

  emit(tokens, TokenType::Flag, startPos, pos, startLine, startColumn,
       flagValue.empty() ? flagName : flagName + "=" + flagValue);
}

std::vector<Token> Tokenizer::tokenize() {
  std::vector<Token> tokens;

  while (!isAtEnd()) {
    skipWhitespace();
    if (isAtEnd())
      break;

    size_t tokenStartPos = pos;
    size_t tokenStartLine = line;
    size_t tokenStartColumn = column;

    char c = advance();

    if (c == '"') {
      size_t contentStart = pos;

      while (!isAtEnd() && peek() != '"')
        advance();

      if (isAtEnd()) {
        throwError(SnowErr::Phase::Tokenizer, "unterminated string literal",
                   SourceSpan{.start = tokenStartPos,
                              .end = pos,
                              .line = tokenStartLine,
                              .column = tokenStartColumn});
      }

      std::string value = input.substr(contentStart, pos - contentStart);
      advance(); // closing quote

      emit(tokens, TokenType::String, tokenStartPos, pos, tokenStartLine, tokenStartColumn, value);
      continue;
    }

    switch (c) {
    case '(':
      emit(tokens, TokenType::LParen, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "(");
      continue;
    case ')':
      emit(tokens, TokenType::RParen, tokenStartPos, pos, tokenStartLine, tokenStartColumn, ")");
      continue;
    case '{':
      emit(tokens, TokenType::LBrace, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "{");
      continue;
    case '}':
      emit(tokens, TokenType::RBrace, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "}");
      continue;
    case '[':
      emit(tokens, TokenType::LBracket, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "[");
      continue;
    case ']':
      emit(tokens, TokenType::RBracket, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "]");
      continue;
    case ',':
      emit(tokens, TokenType::Comma, tokenStartPos, pos, tokenStartLine, tokenStartColumn, ",");
      continue;
    case '*':
      emit(tokens, TokenType::Star, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "*");
      continue;
    case '+':
      emit(tokens, TokenType::Plus, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "+");
      continue;
    case '/':
      emit(tokens, TokenType::Slash, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "/");
      continue;
    case '@':
      emit(tokens, TokenType::At, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "@");
      continue;
    case '$':
      emit(tokens, TokenType::Dollar, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "$");
      continue;
    case ';':
      emit(tokens, TokenType::Sequence, tokenStartPos, pos, tokenStartLine, tokenStartColumn, ";");
      continue;
    case '.':
      emit(tokens, TokenType::Dot, tokenStartPos, pos, tokenStartLine, tokenStartColumn, ".");
      continue;
    case '%':
      emit(tokens, TokenType::Modulo, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "%");
      continue;
    default:
      break;
    }

    if (c == '-') {
      if (match('-')) {
        emitFlag(tokens, tokenStartPos, tokenStartLine, tokenStartColumn);
      } else {
        emit(tokens, TokenType::Minus, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "-");
      }
      continue;
    }

    if (c == '=') {
      if (match('=')) {
        emit(tokens, TokenType::EqualityComparison, tokenStartPos, pos, tokenStartLine,
             tokenStartColumn, "==");
      } else {
        emit(tokens, TokenType::Equal, tokenStartPos, pos, tokenStartLine, tokenStartColumn, "=");
      }
      continue;
    }

    if (c == '>') {
      if (match('=')) {
        emit(tokens, TokenType::GreaterOrEqualThan, tokenStartPos, pos, tokenStartLine,
             tokenStartColumn, ">=");
      } else {
        emit(tokens, TokenType::GreaterThan, tokenStartPos, pos, tokenStartLine, tokenStartColumn,
             ">");
      }
      continue;
    }

    if (c == '<') {
      if (match('=')) {
        emit(tokens, TokenType::LesserOrEqualThan, tokenStartPos, pos, tokenStartLine,
             tokenStartColumn, "<=");
      } else {
        emit(tokens, TokenType::LesserThan, tokenStartPos, pos, tokenStartLine, tokenStartColumn,
             "<");
      }
      continue;
    }

    if (c == '|') {
      if (match('|')) {
        emit(tokens, TokenType::LogicalOr, tokenStartPos, pos, tokenStartLine, tokenStartColumn,
             "||");
      } else {
        throwError(SnowErr::Phase::Tokenizer, "unexpected single '|'",
                   SourceSpan{.start = tokenStartPos,
                              .end = pos,
                              .line = tokenStartLine,
                              .column = tokenStartColumn});
      }
      continue;
    }

    if (c == '&') {
      if (match('&')) {
        emit(tokens, TokenType::LogicalAnd, tokenStartPos, pos, tokenStartLine, tokenStartColumn,
             "&&");
      } else {
        throwError(SnowErr::Phase::Tokenizer, "unexpected single '&'",
                   SourceSpan{.start = tokenStartPos,
                              .end = pos,
                              .line = tokenStartLine,
                              .column = tokenStartColumn});
      }
      continue;
    }

    if (std::isdigit(c)) {
      while (!isAtEnd() && (std::isdigit(peek()) || peek() == '.'))
        advance();

      emit(tokens, TokenType::Number, tokenStartPos, pos, tokenStartLine, tokenStartColumn,
           input.substr(tokenStartPos, pos - tokenStartPos));
      continue;
    }

    if (std::isalpha(c) || c == '_') {
      while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_'))
        advance();

      std::string text = input.substr(tokenStartPos, pos - tokenStartPos);

      TokenType type = text == "from"       ? TokenType::From
                       : text == "let"      ? TokenType::LetSTMT
                       : text == "import"   ? TokenType::ImportSTMT
                       : text == "return"   ? TokenType::ReturnSTMT
                       : text == "if"       ? TokenType::IfSTMT
                       : text == "then"     ? TokenType::ThenSTMT
                       : text == "else"     ? TokenType::ElseSTMT
                       : text == "do"       ? TokenType::DoSTMT
                       : text == "done"     ? TokenType::DoneSTMT
                       : text == "continue" ? TokenType::ContinueSTMT
                       : text == "break"    ? TokenType::BreakSTMT
                       : text == "while"    ? TokenType::WhileSTMT
                       : text == "obj"      ? TokenType::Obj
                       : text == "end"      ? TokenType::EndSTMT
                       : text == "true"     ? TokenType::True
                       : text == "false"    ? TokenType::False
                       : text == "this"     ? TokenType::This
                                            : TokenType::Identifier;

      emit(tokens, type, tokenStartPos, pos, tokenStartLine, tokenStartColumn, text);
      continue;
    }

    throwError(
        SnowErr::Phase::Tokenizer, "unexpected character",
        SourceSpan{
            .start = tokenStartPos, .end = pos, .line = tokenStartLine, .column = tokenStartColumn},
        {std::string("character: '") + c + "'"});
  }

  emit(tokens, TokenType::EndOfFile, pos, pos, line, column, "EOF");
  mergeTokens(tokens, TokenType::DoSTMT, TokenType::LBracket, MergeType::Left);
  mergeTokens(tokens, TokenType::RBracket, TokenType::DoneSTMT, MergeType::Right);
  mergeTokens(tokens, TokenType::Obj, TokenType::LBracket, MergeType::Left);
  mergeTokens(tokens, TokenType::RBracket, TokenType::EndSTMT, MergeType::Right);
  return tokens;
}

} // namespace Snowlang