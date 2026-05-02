#pragma once
#include <functional>
#include <span>
#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

class GameCamera;

namespace GameObjectExposure {

struct Descriptor;
struct Value;

struct Field {
  std::function<Value()> getValue;
  std::function<void(const Value &)> setValue;

  using Ptr = std::shared_ptr<Field>;
};

struct Value {
  using Object = std::shared_ptr<Descriptor>;
  using List = std::vector<Value>;
  using Ref = Field::Ptr;
  std::variant<float, bool, std::string, Object, List, Ref> value;
};

struct Descriptor {
  std::unordered_map<std::string, Field::Ptr> fields;
  static Value::Object describeVector2f(sf::Vector2f &v);
  static Value::Object describeActiveCameraList();
  static Value::List describeRefList(std::span<Field::Ptr> fields);
};

extern std::function<void(std::string)> logger;

// templates

template <typename T> Value toValue(T v);

template <> inline Value toValue<float>(float v) { return Value{v}; }

template <> inline Value toValue<bool>(bool v) { return Value{v}; }

template <> inline Value toValue<Value::Object>(Value::Object v) {
  return Value{std::move(v)};
}

template <> inline Value toValue<Value::List>(Value::List v) {
  return Value{std::move(v)};
}

template <> inline Value toValue<int>(int v) {
  return Value{static_cast<float>(v)};
}

template <> inline Value toValue<unsigned int>(unsigned int v) {
  return Value{static_cast<float>(v)};
}

template <> inline Value toValue<std::string>(std::string v) {
  return Value{std::move(v)};
}

template <typename T> inline T fromValue(const Value &v) {
  if (auto ptr = std::get_if<T>(&v.value)) {
    return *ptr;
  } else {
    throw std::runtime_error(
        "Type mismatch in fromValue: expected type does not match actual");
  }
}

template <> inline int fromValue<int>(const Value &v) {
  if (auto f = std::get_if<float>(&v.value)) {
    return static_cast<int>(*f);
  }

  throw std::runtime_error("Type mismatch in fromValue<int>: expected float");
}

template <> inline unsigned int fromValue<unsigned int>(const Value &v) {
  if (auto f = std::get_if<float>(&v.value)) {
    return static_cast<unsigned int>(*f);
  }

  throw std::runtime_error("Type mismatch in fromValue<int>: expected float");
}

template <typename T, typename Getter, typename Setter>
Field::Ptr makeField(Getter &&getter, Setter &&setter) {
  return std::make_shared<Field>(Field{
      .getValue =
          [g = std::forward<Getter>(getter)]() { return toValue<T>(g()); },
      .setValue =
          [s = std::forward<Setter>(setter)](const Value &v) {
            try {
              s(fromValue<T>(v));
            } catch (const std::exception &e) {
              logger(std::string("Warning: failed to set field value: ") +
                     e.what());
            }
          }});
}

template <typename Setter, typename Getter>
Field::Ptr makeFieldValue(Getter &&getter, Setter &&setter) {
  return std::make_shared<Field>(Field{
      .getValue = [g = std::forward<Getter>(getter)]() -> Value { return g(); },
      .setValue =
          [s = std::forward<Setter>(setter)](const Value &v) {
            try {
              s(v);
            } catch (const std::exception &e) {
              logger(std::string("Warning: failed to set field value: ") +
                     e.what());
            }
          }});
}

template <typename T> Field::Ptr makePublicField(T &var) {
  return std::make_shared<Field>(Field{
      .getValue = [&var]() { return toValue<T>(var); },
      .setValue =
          [&var](const Value &v) {
            try {
              var = fromValue<T>(v);
            } catch (const std::exception &e) {
              logger(std::string("Warning: failed to set public field: ") +
                     e.what());
            }
          }});
}

template <typename T, typename Getter>
Field::Ptr makeUnmutableField(Getter &&getter) {
  return std::make_shared<Field>(Field{
      .getValue =
          [g = std::forward<Getter>(getter)]() { return toValue<T>(g()); },
      .setValue = [](const Value &) {}});
}

template <typename Getter> Field::Ptr makeConstFieldValue(Getter &&getter) {
  return std::make_shared<Field>(Field{
      .getValue = [g = std::forward<Getter>(getter)]() -> Value { return g(); },
      .setValue = [](const Value &) {}});
}

} // namespace GameObjectExposure