#pragma once
#include "LambdaInstance.hpp"
#include "ObjectInstance.hpp"
#include <functional>
#include <memory>
#include <variant>
#include <vector>

namespace Snowlang {

struct RExpr;
using RExprPtr = std::shared_ptr<RExpr>;

struct RStmt;
using RStmtPtr = std::shared_ptr<RStmt>;

struct RuntimeValue {

  struct GameObjectRef {
    std::function<RuntimeValue()> getter;
    std::function<void(const RuntimeValue &)> setter;
  };

  struct Lambda {
    std::shared_ptr<LambdaInstance> instance;
    Lambda(std::shared_ptr<LambdaInstance> instance) : instance(std::move(instance)) {}
    Lambda() : instance(std::make_shared<LambdaInstance>(LambdaInstance{0, {}, {}, 0, {}})) {}
  };

  using List = std::vector<RuntimeValue>;
  using Null = std::monostate;

  using RuntimeType =
      std::variant<Null, float, bool, std::string, List, Lambda, ObjectRef, CellPtr, GameObjectRef>;

  RuntimeType data;

  RuntimeValue() : data(Null{}) {}
  RuntimeValue(float v) : data(v) {}
  RuntimeValue(bool v) : data(v) {}
  RuntimeValue(const std::string &v) : data(v) {}
  RuntimeValue(const List &v) : data(v) {}
  RuntimeValue(const Lambda &v) : data(v) {}
  RuntimeValue(const CellPtr &v) : data(v) {}
  RuntimeValue(const ObjectRef &v) : data(v) {}
  RuntimeValue(const GameObjectRef &ref) : data(ref) {}

  [[nodiscard]] std::string toString(bool showDataTypes = false) const;
  [[nodiscard]] bool isRef() const;
  [[nodiscard]] bool isNull() const;
  [[nodiscard]] RuntimeValue readRef() const;

  void writeRef(const RuntimeValue &v);

  template <typename T> [[nodiscard]] bool holds_alternative() const {
    return std::holds_alternative<T>(data);
  }

private:
  [[nodiscard]] std::string toStringImpl(bool showDataTypes, int indent) const;
  std::string objectToString(const ObjectInstance &obj, bool showDataTypes, int indentLevel);
};

using ValuePtr = std::shared_ptr<RuntimeValue>;
using RuntimeValueRef = const RuntimeValue &;

} // namespace Snowlang