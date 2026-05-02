#pragma once

#include "AST.hpp"
#include "SnowIO.hpp"

namespace Snowlang::ParserPrint {

void printExpr(SnowIO &io, const ExprPtr &expr, size_t indent = 0);
void printStmt(SnowIO &io, const StmtPtr &stmt, size_t indent = 0);
void printPattern(SnowIO &io, const PatternPtr &pattern, size_t indent = 0);

} // namespace Snowlang::ParserPrint