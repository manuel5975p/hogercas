#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include <optional>
#include <string>
#include <memory>
#include <optional>
struct command;
struct expression;
struct command_result{
    std::optional<std::unique_ptr<expression>> output;
    std::optional<std::string> message;
    command_result();
    command_result(std::string);
    command_result(std::unique_ptr<expression>);
    command_result(std::unique_ptr<expression>, std::string);
};
command_result execute(command* cmd);
#endif