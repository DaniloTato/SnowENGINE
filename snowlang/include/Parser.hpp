#pragma once
#include <string>
#include <vector>

#include "AST.hpp"
#include "SnowIO.hpp"
#include "Tokenizer.hpp"

namespace Snowlang {

class Parser {
  std::vector<Token> tokens;
  size_t pos = 0;

public:
  explicit Parser(const std::vector<Token> &t);

  std::vector<StmtPtr> parseProgram();
  std::vector<StmtPtr> parseAndDebugAST(SnowIO &ioInterface);

private:
  // ---------------- Statements ----------------
  StmtPtr parseStmt();
  std::vector<StmtPtr> parseStatementsUntil(TokenType endToken);
  StmtPtr parseSimpleStmt();
  StmtPtr parseIfStmt();
  StmtPtr parseWhileStmt();
  StmtPtr parseDoStmt();

  // ---------------- Expressions ----------------
  ExprPtr parseExpr();
  ExprPtr parseCommandExpr();
  ExprPtr parseAssignment();
  ExprPtr parseLogicalOr();
  ExprPtr parseLogicalAnd();
  ExprPtr parseComparison();
  ExprPtr parseAdditive();
  ExprPtr parseMultiplicative();
  ExprPtr parseUnary();
  ExprPtr parsePostfix(ExprPtr expr);
  ExprPtr parseObjectExpr();

  // ---------------- Patterns ----------------
  PatternPtr parsePattern();
  PatternPtr makeEmptyPattern();

  // ---------------- Atoms ----------------
  ExprPtr parseAtom();

  // ---------------- Lambdas -------------
  ExprPtr parseArglessLambda();

  // ---------------- Utilities ----------------
  bool isLambdaStart();
  [[nodiscard]] bool canStartAtom() const;
  [[nodiscard]] bool canStartAtom(TokenType t) const;
  [[nodiscard]] bool isAtEnd() const;
  [[nodiscard]] const Token &peek(size_t offset = 0) const;
  [[nodiscard]] bool canStartCommand() const;
  [[nodiscard]] bool isCommandTerminator() const;
  bool isCallableExpr(const ExprPtr &expr);
  ExprPtr parseFlagValue(const std::string &text);

  bool match(TokenType t);
  [[nodiscard]] bool check(TokenType t) const;
  [[nodiscard]] const Token &prev() const;
  const Token &consume(TokenType t, const std::string &msg);
  ExprPtr parseCommandOrExpr();
};

} // namespace Snowlang