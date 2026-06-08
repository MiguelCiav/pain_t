#include "command_history.h"
#include "i_command.h"
#include <stack>

void command_history::add(i_command *cmd) {
  undo_stack.push(cmd);
  while (!redo_stack.empty()) {
    delete redo_stack.top();
    redo_stack.pop();
  }
}
void command_history::undo() {
  if (undo_stack.empty())
    return;
  i_command *cmd = undo_stack.top();
  undo_stack.pop();
  cmd->undo();
  redo_stack.push(cmd);
}
void command_history::redo() {
  if (redo_stack.empty())
    return;
  i_command *cmd = redo_stack.top();
  redo_stack.pop();
  cmd->execute();
  undo_stack.push(cmd);
}

command_history::~command_history() {
  clear();
}

void command_history::clear() {
  while (!undo_stack.empty()) {
    delete undo_stack.top();
    undo_stack.pop();
  }
  while (!redo_stack.empty()) {
    delete redo_stack.top();
    redo_stack.pop();
  }
}