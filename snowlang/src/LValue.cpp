#include "LValue.hpp"
#include "SnowErr.hpp"

namespace Snowlang {

void LValue::LValueWrite(RuntimeValue value, SnowlangInstance &snowlang) {
  switch (kind) {

  case Kind::Variable: {
    try {
      snowlang.memory.write(location, value);
    } catch (const SnowErr::MemoryErr &e) {
      throwError(SnowErr::Phase::Memory, "[LValueWrite] invalid variable access", SourceSpan{},
                 e.what());
    }
    break;
  }

  case Kind::ObjectField: {
    if (!object) {
      throwError(SnowErr::Phase::Evaluator, "[LValueWrite] null object reference", SourceSpan{});
    }

    auto it = object->fields.find(field);
    if (it == object->fields.end()) {
      throwError(SnowErr::Phase::Evaluator, "[LValueWrite] object has no field '" + field + "'",
                 SourceSpan{});
    }

    RuntimeValue &target = it->second;

    if (std::holds_alternative<RuntimeValue::GameObjectRef>(target.data)) {
      auto &ref = std::get<RuntimeValue::GameObjectRef>(target.data);
      ref.setter(value);
      return;
    }

    target = value;
    break;
  }

  case Kind::ListElement: {
    RuntimeValue container = base->LValueRead(snowlang, SourceSpan{});

    auto &list = std::get<RuntimeValue::List>(container.data);
    list[index] = value;

    base->LValueWrite(container, snowlang);
    break;
  }
  }
}

RuntimeValue LValue::LValueRead(SnowlangInstance &snowlang, const SourceSpan &span) const {
  switch (kind) {

  case Kind::Variable: {
    try {
      return snowlang.memory.read(location);
    } catch (const SnowErr::MemoryErr &e) {
      throwError(SnowErr::Phase::Memory, "[LValueRead] invalid variable access", span, e.what());
    }
  }

  case Kind::ObjectField: {
    if (!object) {
      throwError(SnowErr::Phase::Evaluator, "[LValueRead] null object reference", span);
    }

    auto it = object->fields.find(field);
    if (it == object->fields.end()) {
      throwError(SnowErr::Phase::Evaluator, "[LValueRead] object has no field '" + field + "'",
                 span);
    }

    RuntimeValue &val = it->second;

    if (std::holds_alternative<RuntimeValue::GameObjectRef>(val.data)) {
      return std::get<RuntimeValue::GameObjectRef>(val.data).getter();
    }

    return val;
  }

  case Kind::ListElement: {
    RuntimeValue container = base->LValueRead(snowlang, span);

    if (!std::holds_alternative<RuntimeValue::List>(container.data)) {
      throwError(SnowErr::Phase::Evaluator, "[LValueRead] indexing non-list", span);
    }

    auto &list = std::get<RuntimeValue::List>(container.data);

    if (index < 0 || index >= static_cast<int>(list.size())) {
      throwError(SnowErr::Phase::Evaluator, "[LValueRead] list index out of bounds", span,
                 "index=" + std::to_string(index) + ", size=" + std::to_string(list.size()));
    }

    RuntimeValue &elem = list[index];
    if (std::holds_alternative<RuntimeValue::GameObjectRef>(elem.data)) {
      return std::get<RuntimeValue::GameObjectRef>(elem.data).getter();
    }

    return elem;
  }
  }

  return {};
}

} // namespace Snowlang