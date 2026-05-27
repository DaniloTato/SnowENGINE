#include "DialogueManager.hpp"
#include "QueuedManager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

bool DialogueManager::loadDialoguesFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "DialogueManager: Could not open file " << filename << "\n";
    return false;
  }

  dialogues.clear();

  std::string line;
  std::string currentKey;
  std::stringstream currentDialogue;

  while (std::getline(file, line)) {
    if (line.starts_with("[Key=")) {
      if (!currentKey.empty()) {
        dialogues[currentKey] = currentDialogue.str();
        currentDialogue.str("");
      }
      size_t eq = line.find('=');
      size_t end = line.find(']');
      currentKey = line.substr(eq + 1, end - eq - 1);
    } else {
      currentDialogue << line << "\n";
    }
  }

  if (!currentKey.empty())
    dialogues[currentKey] = currentDialogue.str();

  return true;
}

const std::string *DialogueManager::getDialogue(const std::string &key) const {
  auto it = dialogues.find(key);
  if (it == dialogues.end())
    return nullptr;
  return &it->second;
}

void DialogueManager::assignDialogue(GameObject *object,
                                     const std::string &key) {
  assigned[object] = key;
}

void DialogueManager::onTrigger(GameObject *object) {
  auto it = assigned.find(object);
  if (it == assigned.end())
    return;

  const std::string *markup = getDialogue(it->second);
  if (!markup || !attachedTextParams)
    return;

  createQueue.push_back({*attachedTextParams, markup});
}

void DialogueManager::attachTextParams(RenderizerParameters *params) {
  attachedTextParams = params;
}

GameText *DialogueManager::createFromRequest(const TextCreationRequest &req) {
  auto *gt = new GameText(req.params);
  gt->loadFromMarkup(*req.markup);
  return gt;
}

void DialogueManager::destroyObject(GameText *text) { delete text; }

void DialogueManager::onSceneUnload() {
  assigned.clear();
  delete attachedTextParams;
  attachedTextParams = nullptr;

  QueuedManager<GameText, TextCreationRequest>::onSceneUnload();
}

RenderizerParameters *DialogueManager::getAttachedTextParams() const {
  return attachedTextParams;
}

void DialogueManager::print(const std::string markup) {
  const std::string *string = &markup;
  createQueue.push_back({*attachedTextParams, string});
}

void DialogueManager::printByKey(const std::string &key) {
  const std::string *markup = getDialogue(key);
  createQueue.push_back({*attachedTextParams, markup});
}