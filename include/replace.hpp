#ifndef REPLACE_HPP
#define REPLACE_HPP
#include <unordered_set>
#include <unordered_map>
#include <memory>
struct expression;
void replace_vars(std::unique_ptr<expression>& exp, std::unordered_set<std::string>& already);
void replace_args(std::unique_ptr<expression>& exp,std::unordered_map<unsigned, std::unique_ptr<expression>>& argnames);
void replace_funcs(std::unique_ptr<expression>& exp, std::unordered_set<std::string>& already);
void replace_funcs(std::unique_ptr<expression>& exp);
void replace_vars(std::unique_ptr<expression>& exp);
std::unique_ptr<expression> evaluate(const std::unique_ptr<expression>& exp);
#endif