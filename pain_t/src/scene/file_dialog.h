#pragma once
#include <string>

class file_dialog {
public:
  static bool is_available();
  static std::string open_file();
  static std::string save_file();
};
