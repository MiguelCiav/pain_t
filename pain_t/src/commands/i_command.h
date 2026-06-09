#pragma once

class i_command {
public:
  virtual ~i_command() = default;
  virtual void execute() = 0;
  virtual void undo() = 0;
};