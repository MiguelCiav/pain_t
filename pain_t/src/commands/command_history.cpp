#include "command_history.h"
#include "i_command.h"
#include <stack>

void command_history::add(i_command *cmd) {
  undo_stack.push(cmd);
  if (!redo_stack.empty()) {
    redo_stack = {};
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