#ifndef VISITOR_HPP_B
#define VISITOR_HPP_B
#define visit__decl(CLASSNAME) virtual std::unique_ptr<expression> visit(const CLASSNAME& arg) = 0;
#include <memory>
struct expression;
struct unary_expression;
struct operation_expression;
struct bracket_expression;
struct function;
struct function_argument;
struct command;
struct variable;
struct constant;
struct operator_expression;
struct tensor_expression;
struct visitor_base{
    visit__decl(expression)
    visit__decl(unary_expression)
    visit__decl(operation_expression)
    visit__decl(bracket_expression)
    visit__decl(function)
    visit__decl(function_argument)
    visit__decl(command)
    visit__decl(variable)
    visit__decl(constant)
    visit__decl(operator_expression)
    visit__decl(tensor_expression)
};
#endif