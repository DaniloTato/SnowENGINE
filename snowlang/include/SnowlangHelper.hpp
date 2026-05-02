#pragma once

#include "RuntimeValue.hpp"
#include "SnowErr.hpp"
#include "Tokenizer.hpp"
#include <typeinfo>

namespace Snowlang::SnowlangHelper {

template <typename T> struct RuntimeValueTo {

  RuntimeValueTo(const SourceSpan &span) : span(span) {}

  T operator()(const RuntimeValue &value) {
    if (!std::holds_alternative<T>(value.data)) {
      throwError(SnowErr::Phase::Evaluator,
                 "Error converting RuntimeValue into " + std::string(typeid(T).name()), span);
    }

    return std::get<T>(value.data);
  }

private:
  const SourceSpan &span;
};

inline std::string tokenTypeToString(TokenType t) {
  switch (t) {
  case TokenType::Identifier:
    return "Identifier";
  case TokenType::Number:
    return "Number";
  case TokenType::LParen:
    return "LParen";
  case TokenType::RParen:
    return "RParen";
  case TokenType::LBrace:
    return "LBrace";
  case TokenType::RBrace:
    return "RBrace";
  case TokenType::LBracket:
    return "LBracket";
  case TokenType::RBracket:
    return "RBracket";
  case TokenType::Equal:
    return "Equal";
  case TokenType::Star:
    return "Star";
  case TokenType::At:
    return "At";
  case TokenType::Dollar:
    return "Dollar";
  case TokenType::String:
    return "String";
  case TokenType::Function:
    return "Function";
  case TokenType::Comma:
    return "Comma";
  case TokenType::Plus:
    return "Plus";
  case TokenType::Minus:
    return "Minus";
  case TokenType::Slash:
    return "Slash";
  case TokenType::EqualityComparison:
    return "EqualityComparison";
  case TokenType::GreaterThan:
    return "GreaterThan";
  case TokenType::LesserThan:
    return "LesserThan";
  case TokenType::GreaterOrEqualThan:
    return "GreaterOrEqualThan";
  case TokenType::LesserOrEqualThan:
    return "LesserOrEqualThan";
  case TokenType::Sequence:
    return "Sequence";
  case TokenType::EndOfFile:
    return "EndOfFile";
  case TokenType::ImportSTMT:
    return "ImportSTMT";
  case TokenType::From:
    return "From";
  case TokenType::LetSTMT:
    return "LetSTMT";
  case TokenType::ReturnSTMT:
    return "ReturnSTMT";
  case TokenType::IfSTMT:
    return "IfSTMT";
  case TokenType::ThenSTMT:
    return "ThenSTMT";
  case TokenType::ElseSTMT:
    return "ElseSTMT";
  case TokenType::DoSTMT:
    return "DoSTMT";
  case TokenType::DoneSTMT:
    return "DoneSTMT";
  case TokenType::WhileSTMT:
    return "WhileSTMT";
  default:
    return "Unknown";
  }
}

inline std::optional<Location> extractAssignableBase(const RExprPtr &expr) {
  if (expr->kind == ExprKind::VarRef) {
    auto *v = static_cast<RVarRefExpr *>(expr.get());
    return v->location;
  }

  if (expr->kind == ExprKind::MemberAccess) {
    auto *m = static_cast<RMemberAccessExpr *>(expr.get());
    return m->baseLocation;
  }

  return std::nullopt;
}

inline bool isNumericOrBool(const RuntimeValue &v) {
  return std::holds_alternative<float>(v.data) || std::holds_alternative<bool>(v.data);
}

inline float toNumber(const RuntimeValue &v) {
  if (std::holds_alternative<float>(v.data)) {
    return std::get<float>(v.data);
  }

  if (std::holds_alternative<bool>(v.data)) {
    return std::get<bool>(v.data) ? 1.0f : 0.0f;
  }

  return 0.0f;
}

template <typename... Ts> class GetListHelper {
public:
  GetListHelper(const SourceSpan &span) : span(span) {}

  std::tuple<Ts...> operator()(const RuntimeValue &value) const {
    using List = Snowlang::RuntimeValue::List;

    const auto &list = RuntimeValueTo<List>(span)(value);

    if (list.size() != sizeof...(Ts)) {
      throwError(SnowErr::Phase::Evaluator,
                 "List must contain exactly " + std::to_string(sizeof...(Ts)) + " elements.", span);
    }

    return convertElements<Ts...>(list, std::index_sequence_for<Ts...>{});
  }

private:
  const SourceSpan &span;

  template <typename... Us, std::size_t... Is>
  std::tuple<Us...> convertElements(const RuntimeValue::List &list,
                                    std::index_sequence<Is...>) const {
    return std::make_tuple(RuntimeValueTo<Us>(span)(list[Is])...);
  }
};

template <typename... Ts> GetListHelper<Ts...> GetList(const SourceSpan &span) {
  return GetListHelper<Ts...>(span);
}

} // namespace Snowlang::SnowlangHelper