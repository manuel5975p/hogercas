#ifndef EQUATION_SOLVER_HPP
#define EQUATION_SOLVER_HPP
#include <visitor_base.hpp>
std::unique_ptr<expression> solve_single_variable(const std::unique_ptr<expression>& left,const std::unique_ptr<expression>& right, const std::string& x);
std::unique_ptr<expression> get_power(const std::string& symbol, const std::unique_ptr<expression>& product_or_division);
#endif