#ifndef DESERIALIZER_HPP
#define DESERIALIZER_HPP
#include <memory>
struct expression;
std::unique_ptr<expression> deserialize_expression(std::stringbuf&);
std::unique_ptr<expression> deserialize_unary_expression(std::stringbuf&);
std::unique_ptr<expression> deserialize_operation_expression(std::stringbuf&);
std::unique_ptr<expression> deserialize_bracket_expression(std::stringbuf&);
std::unique_ptr<expression> deserialize_function(std::stringbuf&);
std::unique_ptr<expression> deserialize_function_argument(std::stringbuf&);
std::unique_ptr<expression> deserialize_command(std::stringbuf&);
std::unique_ptr<expression> deserialize_variable(std::stringbuf&);
std::unique_ptr<expression> deserialize_constant(std::stringbuf&);
std::unique_ptr<expression> deserialize_operator_expression(std::stringbuf&);
std::unique_ptr<expression> deserialize_tensor_expression(std::stringbuf&);
#endif