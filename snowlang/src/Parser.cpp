#include "Parser.hpp"
#include "AST.hpp"
#include "ParserPrint.hpp"
#include "SnowErr.hpp"
#include "SnowlangHelper.hpp"
#include "Tokenizer.hpp"

#include <iostream>
#include <vector>

namespace Snowlang {

// ---------------- Constructor ----------------

Parser::Parser(const std::vector<Token> &t) : tokens(t) {}

// ---------------- Program ----------------

std::vector<StmtPtr> Parser::parseProgram() { return parseStatementsUntil(TokenType::EndOfFile); }

std::vector<StmtPtr> Parser::parseAndDebugAST(SnowIO &ioInterface) {
  auto program = parseProgram();

  std::cout << "\n=== AST ===\n";
  for (size_t i = 0; i < program.size(); ++i) {
    std::cout << "TopStmt[" << i << "]:\n";
    ParserPrint::printStmt(ioInterface, program[i], 1);
  }
  std::cout << "===========\n";

  return program;
}

// ---------------- Statements ----------------

StmtPtr Parser::parseStmt() { return parseSimpleStmt(); }

std::vector<StmtPtr> Parser::parseStatementsUntil(TokenType endToken) {
  std::vector<StmtPtr> stmts;

  stmts.push_back(parseStmt());

  while (match(TokenType::Sequence) && !check(endToken)) {
    stmts.push_back(parseStmt());
  }

  return stmts;
}

StmtPtr Parser::parseSimpleStmt() {
  if (match(TokenType::ImportSTMT)) {
    ExprPtr path = parseExpr();
    return std::make_shared<ImportStmt>(path);
  }

  if (match(TokenType::ReturnSTMT)) {
    ExprPtr expr = parseExpr();
    return std::make_shared<ReturnStmt>(expr);
  }

  if (match(TokenType::DoSTMT)) {
    return parseDoStmt();
  }

  if (match(TokenType::IfSTMT)) {
    return parseIfStmt();
  }

  if (match(TokenType::WhileSTMT)) {
    return parseWhileStmt();
  }

  if (match(TokenType::ContinueSTMT)) {
    return std::make_shared<ContinueStmt>();
  }

  if (match(TokenType::BreakSTMT)) {
    return std::make_shared<BreakStmt>();
  }

  if (match(TokenType::LetSTMT)) {
    auto name = consume(TokenType::Identifier, "Expected identifier after let");

    ExprPtr initializer = nullptr;
    if (match(TokenType::Equal)) {
      initializer = parseExpr();
    }

    return std::make_shared<LetStmt>(name.text, initializer);
  }

  return std::make_shared<ExprStmt>(parseExpr());
}

StmtPtr Parser::parseIfStmt() {
  ExprPtr condition = parseExpr();

  StmtPtr thenBranch = parseStmt();

  StmtPtr elseBranch = nullptr;
  if (match(TokenType::ElseSTMT)) {
    elseBranch = parseStmt();
  }

  return std::make_shared<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch));
}

StmtPtr Parser::parseWhileStmt() {
  ExprPtr condition = parseExpr();

  StmtPtr doBranch = parseStmt();

  return std::make_shared<WhileStmt>(std::move(condition), std::move(doBranch));
}

StmtPtr Parser::parseDoStmt() {
  auto body = parseStatementsUntil(TokenType::DoneSTMT);
  consume(TokenType::DoneSTMT, "[Parser] Expected 'done'");
  return std::make_shared<DoStmt>(std::move(body));
}

// ---------------- Expressions ----------------

ExprPtr Parser::parseExpr() { return parseAssignment(); }

// ---------------- Patterns ----------------

PatternPtr Parser::parsePattern() {
  consume(TokenType::LParen, "[Parser] Expected '(' to start pattern");

  std::vector<std::string> names;

  if (!check(TokenType::RParen)) {
    names.push_back(consume(TokenType::Identifier, "[Parser] Expected identifier in pattern").text);

    while (match(TokenType::Comma)) {
      names.push_back(
          consume(TokenType::Identifier, "[Parser] Expected identifier in pattern").text);
    }
  }

  consume(TokenType::RParen, "[Parser] Expected ')' to close pattern");
  return std::make_shared<IdentListPattern>(std::move(names));
}

PatternPtr Parser::makeEmptyPattern() {
  return std::make_shared<IdentListPattern>(std::vector<std::string>());
}

// ---------------- Assignment ----------------

ExprPtr Parser::parseAssignment() {
  ExprPtr lhs = parseLogicalOr();

  if (match(TokenType::Equal)) {
    ExprPtr rhs = parseAssignment();
    return std::make_shared<AssignmentExpr>(lhs, rhs);
  }

  return lhs;
}

// ---------------- Logical and/or ----------------

ExprPtr Parser::parseLogicalOr() {
  ExprPtr expr = parseLogicalAnd();

  while (match(TokenType::LogicalOr)) {
    TokenType op = prev().type;
    ExprPtr rhs = parseLogicalAnd();
    expr = std::make_shared<BinaryExpr>(op, expr, rhs);
  }

  return expr;
}

ExprPtr Parser::parseLogicalAnd() {
  ExprPtr expr = parseComparison();

  while (match(TokenType::LogicalAnd)) {
    TokenType op = prev().type;
    ExprPtr rhs = parseComparison();
    expr = std::make_shared<BinaryExpr>(op, expr, rhs);
  }

  return expr;
}

// ---------------- Comparison ----------------

ExprPtr Parser::parseComparison() {
  ExprPtr expr = parseAdditive();

  while (true) {
    TokenType op;

    if (match(TokenType::EqualityComparison)) {
      op = TokenType::EqualityComparison;
    } else if (match(TokenType::GreaterThan)) {
      op = TokenType::GreaterThan;
    } else if (match(TokenType::LesserThan)) {
      op = TokenType::LesserThan;
    } else if (match(TokenType::GreaterOrEqualThan)) {
      op = TokenType::GreaterOrEqualThan;
    } else if (match(TokenType::LesserOrEqualThan)) {
      op = TokenType::LesserOrEqualThan;
    } else {
      break;
    }

    ExprPtr rhs = parseAdditive();
    expr = std::make_shared<BinaryExpr>(op, expr, rhs);
  }

  return expr;
}

// ---------------- Additive ----------------

ExprPtr Parser::parseAdditive() {
  ExprPtr expr = parseMultiplicative();

  while (match(TokenType::Plus) || match(TokenType::Minus)) {
    TokenType op = prev().type;
    ExprPtr rhs = parseMultiplicative();
    expr = std::make_shared<BinaryExpr>(op, expr, rhs);
  }

  return expr;
}

// ---------------- Multiplicative ----------------

ExprPtr Parser::parseMultiplicative() {
  ExprPtr expr = parseUnary();

  while (match(TokenType::Star) || match(TokenType::Slash) || match(TokenType::Modulo)) {
    TokenType op = prev().type;
    ExprPtr rhs = parseUnary();
    expr = std::make_shared<BinaryExpr>(op, expr, rhs);
  }

  return expr;
}

// ---------------- Unary ----------------

ExprPtr Parser::parseUnary() {

  if (canStartCommand()) {
    return parseCommandExpr();
  }

  if (isLambdaStart()) {
    auto params = parsePattern();
    consume(TokenType::LBrace, "Expected '{' after lambda parameters");
    auto body = parseStatementsUntil(TokenType::RBrace);
    consume(TokenType::RBrace, "Expected '}'");
    return std::make_shared<LambdaExpr>(params, std::move(body));
  }

  if (match(TokenType::Star)) {
    return std::make_shared<DerefExpr>(parseUnary());
  }

  if (match(TokenType::Identifier)) {
    return std::make_shared<ParameterExpr>(prev().text);
  }

  return parsePostfix(parseAtom());
}

//-------------- Command ----------------
ExprPtr Parser::parseCommandExpr() {
  Token name = consume(TokenType::Identifier, "Expected command name");

  std::vector<ExprPtr> args;
  std::unordered_map<std::string, ExprPtr> flags;

  while (true) {
    if (match(TokenType::Flag)) {

      auto raw = prev().text;
      auto eq = raw.find('=');

      if (eq == std::string::npos) {
        flags[raw] = std::make_shared<BoolExpr>(true);
      } else {
        std::string key = raw.substr(0, eq);
        std::string value = raw.substr(eq + 1);
        flags[key] = parseFlagValue(value);
      }

    } else if (canStartAtom()) {

      args.push_back(parseAssignment());

    } else {
      break;
    }
  }

  return std::make_shared<CommandExpr>(name.text, std::move(args), std::move(flags));
}

//-------------- Command Flags -----------
ExprPtr Parser::parseFlagValue(const std::string &text) {
  Tokenizer tokenizer(text);
  auto tokens = tokenizer.tokenize();

  Parser subParser(tokens);
  ExprPtr expr = subParser.parseExpr();

  return expr;
}

// ---------------- Postfix ----------------

ExprPtr Parser::parsePostfix(ExprPtr expr) {
  while (true) {
    if (match(TokenType::LParen) && isCallableExpr(expr)) {
      std::vector<ExprPtr> args;

      if (!check(TokenType::RParen)) {
        args.push_back(parseExpr());
        while (match(TokenType::Comma)) {
          args.push_back(parseExpr());
        }
      }

      consume(TokenType::RParen, "Expected ')'");
      expr = std::make_shared<CallExpr>(expr, std::move(args));
      continue;
    }

    if (match(TokenType::LBracket)) {
      ExprPtr index = parseExpr();
      consume(TokenType::RBracket, "Expected ']'");
      expr = std::make_shared<IndexExpr>(expr, index);
      continue;
    }

    if (match(TokenType::Dot)) {
      Token member = consume(TokenType::Identifier, "Expected member name after '.'");

      expr = std::make_shared<MemberAccessExpr>(expr, member.text);
      continue;
    }

    break;
  }

  return expr;
}

// ---------------- Atoms ----------------

ExprPtr Parser::parseAtom() {

  if (match(TokenType::Dollar)) {
    auto name = consume(TokenType::Identifier, "Expected identifier");
    return std::make_shared<LoadExpr>(name.text);
  }

  if (match(TokenType::At)) {
    auto name = consume(TokenType::Identifier, "Expected identifier");
    return std::make_shared<AddressExpr>(name.text);
  }

  if (match(TokenType::This)) {
    return std::make_shared<ThisExpr>();
  }

  if (match(TokenType::True)) {
    return std::make_shared<BoolExpr>(true);
  }

  if (match(TokenType::False)) {
    return std::make_shared<BoolExpr>(false);
  }

  if (match(TokenType::Number))
    return std::make_shared<NumberExpr>(std::stof(prev().text));

  if (match(TokenType::String))
    return std::make_shared<StringExpr>(prev().text);

  if (match(TokenType::Identifier))
    return std::make_shared<ParameterExpr>(prev().text);

  if (match(TokenType::LParen)) {
    ExprPtr expr = parseExpr();
    consume(TokenType::RParen, "Expected ')'");
    return expr;
  }

  if (match(TokenType::Obj)) {
    return parseObjectExpr();
  }

  if (match(TokenType::LBrace))
    return parseArglessLambda();

  throwError(SnowErr::Phase::Parser,
             "unexpected token: " + SnowlangHelper::tokenTypeToString(peek().type), peek().span);
}

// ---------------- Utilities ----------------

bool Parser::canStartAtom() const { return canStartAtom(peek().type); }

bool Parser::canStartAtom(TokenType t) const {
  return t == TokenType::Identifier || t == TokenType::Number || t == TokenType::String ||
         t == TokenType::At || t == TokenType::Dollar || t == TokenType::True ||
         t == TokenType::False || t == TokenType::This || t == TokenType::Star ||
         t == TokenType::LParen || t == TokenType::LBrace || t == TokenType::LBracket;
}

//---------- Lambdas -----------

ExprPtr Parser::parseArglessLambda() {
  auto body = parseStatementsUntil(TokenType::RBrace);
  consume(TokenType::RBrace, "Expected '}'");
  return std::make_shared<LambdaExpr>(makeEmptyPattern(), std::move(body));
}

//------------ Utilities ------------

bool Parser::isLambdaStart() {
  if (!check(TokenType::LParen))
    return false;

  size_t save = pos;
  try {
    parsePattern();
    bool ok = check(TokenType::LBrace);
    pos = save;
    return ok;
  } catch (...) {
    pos = save;
    return false;
  }
}

bool Parser::isAtEnd() const { return pos >= tokens.size(); }

bool Parser::match(TokenType t) {
  if (check(t)) {
    ++pos;
    return true;
  }
  return false;
}

bool Parser::check(TokenType t) const { return pos < tokens.size() && tokens[pos].type == t; }

const Token &Parser::prev() const { return tokens[pos - 1]; }

const Token &Parser::consume(TokenType t, const std::string &msg) {
  if (check(t)) {
    return tokens[pos++];
  }

  const Token &errTok = peek(0);
  throwError(SnowErr::Phase::Parser, msg, errTok.span);
}

const Token &Parser::peek(size_t offset) const {
  if (pos + offset >= tokens.size())
    return tokens.back();
  return tokens[pos + offset];
}

bool Parser::canStartCommand() const {
  if (!check(TokenType::Identifier))
    return false;

  return canStartAtom(peek(1).type) || peek(1).type == TokenType::Flag;
}

bool Parser::isCallableExpr(const ExprPtr &expr) {
  return dynamic_cast<LoadExpr *>(expr.get()) || dynamic_cast<LambdaExpr *>(expr.get()) ||
         dynamic_cast<CallExpr *>(expr.get()) || dynamic_cast<MemberAccessExpr *>(expr.get());
}

//-------Objects-------

ExprPtr Parser::parseObjectExpr() {
  std::vector<ObjectExpr::Field> fields;

  while (!check(TokenType::EndSTMT)) {
    // field name
    Token name = consume(TokenType::Identifier, "Expected identifier in object field");

    consume(TokenType::Equal, "Expected '=' after object field name");

    ExprPtr value = parseExpr();

    fields.emplace_back(name.text, value);

    consume(TokenType::Sequence, "Expected ';' after object field");
  }

  consume(TokenType::EndSTMT, "Expected 'end' to close object");

  return std::make_shared<ObjectExpr>(std::move(fields));
}

} // namespace Snowlang