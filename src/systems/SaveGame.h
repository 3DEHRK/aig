#pragma once
#include <string>
class InputManager;
void SaveCustomBindings(const InputManager& input, const std::string& path);
bool LoadCustomBindings(InputManager& input, const std::string& path);