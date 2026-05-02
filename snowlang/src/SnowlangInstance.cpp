#include "SnowlangInstance.hpp"

#include "Debug.hpp"
#include "Parser.hpp"
#include "Resolver.hpp"
#include "SnowErr.hpp"
#include "Tokenizer.hpp"

#include "GameObjectExposure.hpp"
#include "GameState.hpp"

#include <fstream>
#include <sstream>

namespace Snowlang {

SnowlangInstance::SnowlangInstance(SnowIO &ioInterface)
    : memory(this), resolver(this), evaluator(this), io(ioInterface), debug(ioInterface) {
  Commands::DefineCommands(*this);
  bindGameState();
  latestInstance = std::make_unique<SnowlangInstance>(*this);
}

std::unique_ptr<SnowlangInstance> SnowlangInstance::latestInstance;

SnowlangInstance &SnowlangInstance::getLatestSnowlangInstance() { return *latestInstance; }

void SnowlangInstance::run(const std::string &source) {
  if (source.empty()) {
    return;
  }

  cacheSource(source);
  try {
    Tokenizer tokenizer(source);
    auto tokens = tokenizer.tokenize();
    Parser parser(tokens);

    std::vector<StmtPtr> ast;
    if (Debug::enabled) {
      ast = parser.parseAndDebugAST(io);
    } else {
      ast = parser.parseProgram();
    }

    for (const auto &stmt : ast) {
      RStmtPtr resolved = resolver.resolveStmt(stmt);
      evaluator.execStmt(resolved);
    }
  } catch (const SnowErr &err) {
    io.writeLn(err.format(cachedSource));
  }
}

std::string SnowlangInstance::readFile(const std::string &path) const {
  std::ifstream file(path, std::ios::in);
  if (!file.is_open()) {
    throwError(SnowErr::Phase::Evaluator, "[IO] Could not open file: " + path, SourceSpan{});
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void SnowlangInstance::cacheSource(const std::string &source) { cachedSource = source; }

SnowlangInstance::Module SnowlangInstance::loadModule(const std::string &path) {
  std::string source = readFile(path);
  cacheSource(source);

  // im sure there must be a more elegant solution to this
  // I'll improve it later.
  auto previousGlobals = resolver.getGlobalSymbols();

  Tokenizer tokenizer(source);
  Parser parser(tokenizer.tokenize());
  auto ast = parser.parseProgram();

  std::vector<RStmtPtr> resolved;
  resolved.reserve(ast.size());
  for (auto &stmt : ast) {
    resolved.push_back(resolver.resolveStmt(stmt));
  }

  for (auto &stmt : resolved) {
    evaluator.execStmt(stmt);
  }

  Module m;
  for (auto &[name, slot] : resolver.getGlobalSymbols()) {
    if (!previousGlobals.count(name)) {
      m.exportSlots[name] = slot;
    }
  }

  return m;
}

void SnowlangInstance::update(double deltaTime) {
  for (auto &task : scheduledTasks) {

    if (task.isPaused()) {
      continue;
    }

    task.accumulator += deltaTime;

    while (task.accumulator >= task.interval) {
      task.accumulator -= task.interval;

      try {
        evaluator.runLambda(task.lambda);
      } catch (SnowErr e) {
        io.writeLn(e.format(cachedSource));
      }
    }
  }
}

void SnowlangInstance::scheduleTask(ScheduledTask &&task) {
  scheduledTasks.push_back(std::move(task));
}

bool SnowlangInstance::killTask(size_t id) {
  auto it = std::ranges::remove_if(scheduledTasks, [&](const ScheduledTask &t) {
              return t.id == id;
            }).begin();

  bool removed = it != scheduledTasks.end();
  scheduledTasks.erase(it, scheduledTasks.end());
  return removed;
}

void SnowlangInstance::killAllTasks() { scheduledTasks.clear(); }

bool SnowlangInstance::togglePauseTask(size_t id) {
  auto it = std::ranges::remove_if(scheduledTasks, [&](const ScheduledTask &t) {
              return t.id == id;
            }).begin();

  if (it == scheduledTasks.end()) {
    return false;
  }

  it->togglePause();
  return true;
}

size_t SnowlangInstance::taskCount() const { return scheduledTasks.size(); }

size_t SnowlangInstance::getNewTaskId() { return nextTaskId; }

const std::vector<ScheduledTask> &SnowlangInstance::getTasks() const { return scheduledTasks; }

RuntimeValue SnowlangInstance::adaptValue(const GameObjectExposure::Value &v) {

  return std::visit(
      [&](auto &&arg) -> RuntimeValue {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, bool> ||
                      std::is_same_v<T, std::string>) {

          return RuntimeValue(arg);
        } else if constexpr (std::is_same_v<T, GameObjectExposure::Value::List>) {
          RuntimeValue::List runtimeList;
          for (const auto &elem : arg) {
            runtimeList.push_back(adaptValue(elem));
          }
          return {runtimeList};

        } else if constexpr (std::is_same_v<T, GameObjectExposure::Value::Ref>) {
          return RuntimeValue::GameObjectRef{
              .getter = [this, field = arg]() { return adaptValue(field->getValue()); },
              .setter = [this,
                         field = arg](const RuntimeValue &rv) { field->setValue(adaptBack(rv)); }};

        } else if constexpr (std::is_same_v<T, GameObjectExposure::Value::Object>) {
          return RuntimeValue(adaptDescriptor(*arg));
        }
      },
      v.value);
}

ObjectRef SnowlangInstance::adaptDescriptor(const GameObjectExposure::Descriptor &desc) {

  auto obj = std::make_shared<ObjectInstance>();

  for (const auto &[name, fieldPtr] : desc.fields) {

    auto value = fieldPtr->getValue();

    if (std::holds_alternative<GameObjectExposure::Value::Object>(value.value)) {
      obj->fields[name] =
          RuntimeValue(adaptDescriptor(*std::get<GameObjectExposure::Value::Object>(value.value)));
    } else {
      obj->fields[name] = RuntimeValue::GameObjectRef{
          .getter = [this, fieldPtr]() { return adaptValue(fieldPtr->getValue()); },
          .setter = [this,
                     fieldPtr](const RuntimeValue &rv) { fieldPtr->setValue(adaptBack(rv)); }};
    }
  }

  return obj;
}

GameObjectExposure::Value SnowlangInstance::adaptBack(const RuntimeValue &rv) {
  return std::visit(
      [&](auto &&arg) -> GameObjectExposure::Value {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, bool> ||
                      std::is_same_v<T, std::string>) {

          return GameObjectExposure::Value{arg};

        } else if constexpr (std::is_same_v<T, RuntimeValue::List>) {

          GameObjectExposure::Value::List list;
          list.reserve(arg.size());

          for (const auto &elem : arg) {
            list.push_back(adaptBack(elem));
          }

          return GameObjectExposure::Value{.value = list};

        } else {
          throwError(SnowErr::Phase::GameObjRefLoading, "Unsupported assignment at adaptBack()",
                     SourceSpan{});
        }
      },
      rv.data);
}

void SnowlangInstance::bindGameState() {
  auto &gs = GameState::getInstance();
  const auto &desc = gs.getExposedGameState();

  auto snowlangGameState = RuntimeValue(adaptDescriptor(desc));

  resolver.resolveVarCreation(
      "state", SourceSpan{} /*Dummy Source Span (Only has error throwing purposes)*/);
  // Location Assumes no other vars have been declared beforehand.
  memory.write(Location{Location::Type::Global, 0}, snowlangGameState);
}

} // namespace Snowlang