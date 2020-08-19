#ifndef EVALUATOR_VISITOR_HPP
#define EVALUATOR_VISITOR_HPP
#include <visitor_base.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
struct evaluation_error : std::logic_error{
    std::pair<size_t, size_t> error_range;
    evaluation_error(const char* a) : std::logic_error(a), error_range(0, 0){}
    evaluation_error(const std::string& a) : std::logic_error(a), error_range(0, 0){}
    evaluation_error(std::string&& a) : std::logic_error(std::move(a)), error_range(0, 0){}
    evaluation_error(const char* a       , const std::pair<size_t, size_t>& err) : std::logic_error(a), error_range           (err){}
    evaluation_error(const std::string& a, const std::pair<size_t, size_t>& err) : std::logic_error(a), error_range           (err){}
    evaluation_error(std::string&& a     , const std::pair<size_t, size_t>& err) : std::logic_error(std::move(a)), error_range(err){}
};
struct evaluator_visitor : visitor_base{
    std::vector<std::pair<std::string, std::vector<std::unique_ptr<expression>>>> already_replaced_functions;
    std::unordered_set<std::string> already_replaced_variables;
    std::optional<std::unordered_map<std::string, std::unique_ptr<expression>>> lookup_table;
    evaluator_visitor();
    evaluator_visitor(std::unordered_map<std::string, std::unique_ptr<expression>>);
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