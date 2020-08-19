#ifndef TREEIFY_HPP
#define TREEIFY_HPP
#include <memory>
#include <vector>
#include <operations.hpp>
struct expression;
struct operation_expression;

std::unique_ptr<expression> treeify(operation op, const std::vector<std::unique_ptr<expression>>& vec);

#endif