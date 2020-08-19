#ifndef CONSTANT_YEET_
#define CONSTANT_YEET_
#include <visitor_base.hpp>
struct constant_yeeting_visitor : visitor_base{
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