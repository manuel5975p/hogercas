#ifndef NUMERICAL_EVALUATOR_VISITOR_HPP
#define NUMERICAL_EVALUATOR_VISITOR_HPP
#include <visitor_base.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
struct numerical_evaluator_visitor : visitor_base{
    numerical_evaluator_visitor();
    virtual std::unique_ptr<expression> visit(const expression& arg);
    virtual std::unique_ptr<expression> visit(const unary_expression& arg);
    virtual std::unique_ptr<expression> visit(const operation_expression& arg);
    virtual std::unique_ptr<expression> visit(const bracket_expression& arg);
    virtual std::unique_ptr<expression> visit(const function& arg);
    virtual std::unique_ptr<expression> visit(const function_argument& arg);
    virtual std::unique_ptr<expression> visit(const command& arg);
    virtual std::unique_ptr<expression> visit(const variable& arg);
    virtual std::unique_ptr<expression> visit(const constant& arg);
    virtual std::unique_ptr<expression> visit(const operator_expression& arg);
    virtual std::unique_ptr<expression> visit(const tensor_expression& arg);
};
#endif