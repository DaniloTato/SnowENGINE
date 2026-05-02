#include "RuntimeValue.hpp"
#include "Cell.hpp"

namespace Snowlang {

namespace {

static std::string indent(int n) {
  const std::size_t spaces = static_cast<std::size_t>(n) * 2u;
  return std::string(spaces, ' '); // NOLINT(modernize-return-braced-init-list)
}

} // namespace

std::string RuntimeValue::objectToString(const ObjectInstance &obj, bool showDataTypes,
                                         int indentLevel) {

  std::string out = "{\n";

  for (const auto &[name, value] : obj.fields) {
    out += indent(indentLevel + 1);
    out += name + " = ";
    out += value.toStringImpl(showDataTypes, indentLevel + 1);
    out += "\n";
  }

  out += indent(indentLevel) + "}";
  return out;
}

std::string RuntimeValue::toString(bool showDataTypes) const {
  return toStringImpl(showDataTypes, 0);
}

std::string RuntimeValue::toStringImpl(bool showDataTypes, int indentLevel) const {
  return std::visit(
      [&](auto &&val) -> std::string {
        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          return "<NULL>";
        }

        else if constexpr (std::is_same_v<T, float>) {
          const std::string data = std::to_string(val);
          return showDataTypes ? "<Number>: " + data : data;
        }

        else if constexpr (std::is_same_v<T, bool>) {
          const std::string data = val ? "true" : "false";
          return showDataTypes ? "<Bool>: " + data : data;
        }

        else if constexpr (std::is_same_v<T, std::string>) {
          return showDataTypes ? "<String>: " + val : val;
        }

        else if constexpr (std::is_same_v<T, List>) {
          if (!showDataTypes)
            return "<List>";
          return "<List>(" + std::to_string(val.size()) + ")";
        }

        else if constexpr (std::is_same_v<T, Lambda>) {
          if (!showDataTypes)
            return "<Lambda>";
          return "<Lambda>: " + std::to_string(val.instance->id);
        }

        else if constexpr (std::is_same_v<T, CellPtr>) {
          if (!val)
            return "<CellRef:null>";
          return showDataTypes ? "<Address> -> " +
                                     val->getCellValue().toStringImpl(showDataTypes, indentLevel)
                               : "<CellRef>";
        }

        else if constexpr (std::is_same_v<T, GameObjectRef>) {
          RuntimeValue resolved = val.getter();
          return resolved.toStringImpl(showDataTypes, indentLevel);
        }

        else if constexpr (std::is_same_v<T, ObjectRef>) {
          if (!val)
            return "<Object:null>";

          std::string out = "\\<color=lightblue\\>{\n";

          for (const auto &[name, field] : val->fields) {
            out += indent(indentLevel + 1);
            out += name + " = ";
            out += field.toStringImpl(showDataTypes, indentLevel + 1);
            out += "\n";
          }

          out += indent(indentLevel) + "}\\</color\\>";
          return out;
        }

        else {
          return "<Unknown RuntimeValue>";
        }
      },
      data);
}

bool RuntimeValue::isRef() const { return std::holds_alternative<GameObjectRef>(data); }

bool RuntimeValue::isNull() const { return std::holds_alternative<Null>(data); }

RuntimeValue RuntimeValue::readRef() const {
  if (auto *ref = std::get_if<GameObjectRef>(&data))
    return ref->getter();
  return *this;
}

void RuntimeValue::writeRef(const RuntimeValue &v) {
  if (auto *ref = std::get_if<GameObjectRef>(&data))
    ref->setter(v);
}

} // namespace Snowlang