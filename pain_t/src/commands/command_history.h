#pragma once
#include "i_command.h"
#include <stack>

class command_history {
private:
  std::stack<i_command *> undo_stack;
  std::stack<i_command *> redo_stack;

public:
  ~command_history() = default;
  void add(i_command *cmd);
  void undo();
  void redo();
  bool can_undo() { return !undo_stack.empty(); }
  bool can_redo() { return !redo_stack.empty(); }
};