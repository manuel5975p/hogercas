#ifndef CONSTANT_FUSION_HPP
#define CONSTANT_FUSION_HPP
#include <memory>
#include <parser.hpp>
struct expression;
struct constant;
mpq_class mpq_from_float_literal(std::string str);
bool constant_equal(const constant* l, const constant* r);
std::unique_ptr<expression> fuse(const constant* first, const constant* second, operation op);
#endif