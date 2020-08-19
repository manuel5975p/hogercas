#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <utility>
#include <type_traits>
struct tokenizer_error : std::logic_error{
    std::pair<size_t, size_t> error_range;
    tokenizer_error(const char* a) : std::logic_error(a), error_range(0, 0){}
    tokenizer_error(const std::string& a) : std::logic_error(a), error_range(0, 0){}
    tokenizer_error(std::string&& a) : std::logic_error(std::move(a)), error_range(0, 0){}
    tokenizer_error(const char* a       , const std::pair<size_t, size_t>& err) : std::logic_error(a), error_range           (err){}
    tokenizer_error(const std::string& a, const std::pair<size_t, size_t>& err) : std::logic_error(a), error_range           (err){}
    tokenizer_error(std::string&& a     , const std::pair<size_t, size_t>& err) : std::logic_error(std::move(a)), error_range(err){}
};
enum token_type : int{
    PAR_LEFT, PAR_RIGHT,BRACKET_LEFT, BRACKET_RIGHT, IDENTIFIER, PLUS, MINUS, MUL, DIV, POW, LITERAL, SEPARATOR, ASSIGN, CURLY_LEFT, CURLY_RIGHT
};
bool is_par(token_type tt);
bool is_opener(token_type tt);
bool is_closer(token_type tt);
unsigned par_type(token_type tt);
struct token{
    token_type type;
    size_t opos;
    std::string data;
};
std::pair<token, size_t> match_at(const std::string& str, size_t pos);
std::vector<token> tokenize(const std::string& str, size_t pos = 0);
#endif