#pragma once
#include <string>
#include <unordered_map>

#include "GameObject.hpp"
#include "GameText.hpp"
#include "QueuedManager.hpp"
#include "RenderizerParameters.hpp"

struct TextCreationRequest {
  RenderizerParameters params;
  const std::string *markup;
};

class DialogueManager : public QueuedManager<GameText, TextCreationRequest> {
public:
  DialogueManager() = default;
  DialogueManager(const DialogueManager &) = delete;
  DialogueManager &operator=(const DialogueManager &) = delete;
  DialogueManager(DialogueManager &&) = delete;
  DialogueManager &operator=(DialogueManager &&) = delete;

  bool loadDialoguesFromFile(const std::string &filename);

  [[nodiscard]] const std::string *getDialogue(const std::string &key) const;
  void assignDialogue(GameObject *object, const std::string &key);

  void onTrigger(GameObject *object);

  void attachTextParams(RenderizerParameters *params);
  [[nodiscard]] RenderizerParameters *getAttachedTextParams() const;
  void onSceneUnload() override;

  void print(const std::string markup);
  void printByKey(const std::string &key);

private:
  GameText *createFromRequest(const TextCreationRequest &req) override;
  void destroyObject(GameText *text) override;

  std::unordered_map<std::string, std::string> dialogues;
  std::unordered_map<GameObject *, std::string> assigned;

  RenderizerParameters *attachedTextParams = nullptr;
};