#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <optional>
#include <string>
struct command;
std::optional<std::string> execute(command* cmd);
#endif