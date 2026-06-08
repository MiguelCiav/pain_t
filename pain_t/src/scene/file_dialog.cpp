#include "file_dialog.h"
#include <cstring>

#ifdef _WIN32
#include <commdlg.h>
#include <windows.h>
#pragma comment(lib, "comdlg32.lib")

bool file_dialog::is_available() { return true; }

std::string file_dialog::open_file() {
  char filename[260] = "";
  OPENFILENAMEA ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFilter = "pain_t files (*.p_t)\0*.p_t\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = filename;
  ofn.nMaxFile = sizeof(filename);
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrDefExt = "p_t";

  if (GetOpenFileNameA(&ofn)) {
    return std::string(filename);
  }
  return "";
}

std::string file_dialog::save_file() {
  char filename[260] = "";
  OPENFILENAMEA ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFilter = "pain_t files (*.p_t)\0*.p_t\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = filename;
  ofn.nMaxFile = sizeof(filename);
  ofn.Flags =
      OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  ofn.lpstrDefExt = "p_t";

  if (GetSaveFileNameA(&ofn)) {
    return std::string(filename);
  }
  return "";
}

#else // Linux / macOS / Posix

#include <array>
#include <cstdio>
#include <memory>

static std::string run_command(const std::string &cmd) {
  std::array<char, 128> buffer;
  std::string result;
  auto deleter = [](FILE *f) {
    if (f)
      pclose(f);
  };
  std::unique_ptr<FILE, decltype(deleter)> pipe(popen(cmd.c_str(), "r"),
                                                deleter);
  if (!pipe) {
    return "";
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
    result.pop_back();
  }
  return result;
}

bool file_dialog::is_available() {
  std::string check = run_command("which zenity 2>/dev/null");
  return !check.empty();
}

std::string file_dialog::open_file() {
  if (!is_available()) {
    return "";
  }
  std::string cmd =
      "zenity --file-selection --file-filter=\"pain_t files (*.p_t) | *.p_t\" "
      "--file-filter=\"All files | *\" 2>/dev/null";
  return run_command(cmd);
}

std::string file_dialog::save_file() {
  if (!is_available()) {
    return "";
  }
  std::string cmd = "zenity --file-selection --save --confirm-overwrite "
                    "--file-filter=\"pain_t files (*.p_t) | *.p_t\" "
                    "--file-filter=\"All files | *\" 2>/dev/null";
  return run_command(cmd);
}

#endif
