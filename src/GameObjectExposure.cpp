#include "GameObjectExposure.hpp"
#include "GameCamera.hpp"
#include "GameState.hpp"
#include "SnowlangInstance.hpp"

namespace GameObjectExposure {

std::function<void(std::string)> logger = [](const std::string &msg) {
  Snowlang::SnowlangInstance::getLatestSnowlangInstance().io.writeLn(msg);
};

Value::Object Descriptor::describeVector2f(sf::Vector2f &v) {

  auto desc = std::make_shared<Descriptor>();

  desc->fields["x"] = makePublicField(v.x);
  desc->fields["y"] = makePublicField(v.y);

  return desc;
}

Value::Object Descriptor::describeActiveCameraList() {
  auto desc = std::make_shared<Descriptor>();

  desc->fields["main"] = makeConstFieldValue([]() -> Value {
    return Value{.value = GameState::getInstance().getMainCamera()->describe()};
  });

  if (GameState::getInstance().getUiCamera()) {
    desc->fields["ui"] = makeConstFieldValue([]() -> Value {
      return Value{.value = GameState::getInstance().getUiCamera()->describe()};
    });
  }

  desc->fields["terminal"] = makeConstFieldValue([]() -> Value {
    return Value{.value =
                     GameState::getInstance().getTerminalCamera()->describe()};
  });

  return desc;
}

Value::List Descriptor::describeRefList(std::span<Field::Ptr> fields) {
  Value::List list;

  for (auto &field : fields) {
    list.push_back(Value{.value = field});
  }

  return list;
}

} // namespace GameObjectExposure