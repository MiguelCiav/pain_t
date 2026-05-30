#pragma once
#include <string>

class scene;

class engine_2d;

class scene_serializer {
public:
  static bool save(scene& s, const std::string& filepath);
  static bool load_into(const std::string& filepath, scene& s, engine_2d* engine = nullptr);
};
