#ifndef FLATTEN_HPP
#define FLATTEN_HPP
#include <vector>
#include <memory>
#include <operations.hpp>
struct expression;
struct operation_expression;
std::vector<std::unique_ptr<expression>> flatten(const operation_expression*);
std::vector<std::unique_ptr<expression>> flatten(const std::unique_ptr<expression>&);
std::vector<std::unique_ptr<expression>> flatten(operation op, const operation_expression*);
std::vector<std::unique_ptr<expression>> flatten(operation op, const std::unique_ptr<expression>&);
#endif