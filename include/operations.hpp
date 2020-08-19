#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP
#include <cassert>
#include <tokenizer.hpp>
enum operation{
    addition, subtraction, multiplication, division, power, assignment
};
inline unsigned int precedence(operation op){
    if(op == assignment){
        return 4;
    }
    if(op == addition){
        return 3;
    }
    if(op == subtraction){
        return 3;
    }
    if(op == multiplication){
        return 2;
    }
    if(op == division){
        return 2;
    }
    if(op == power){
        return 1;
    }
    assert(false && "No operation given");
    return 0;
}
enum unary_operation{
    negation
};
template<typename char_sequence_t>
operation operation_type(const char_sequence_t& str){
    throw std::exception();
}
template<>
inline operation operation_type<token_type>(const token_type& operat){
    switch(operat){
        case PLUS:
            return addition;
            break;
        case MINUS:
            return subtraction;
            break;
        case MUL:
            return multiplication;
            break;
        case DIV:
            return division;
            break;
        case POW:
            return power;
            break;
        case ASSIGN:
        return assignment;
            break;
    }
    throw std::exception();
}
template<>
inline operation operation_type<char>(const char& str){
    switch(str){
        case '+':
            return addition;
            break;
        case '-':
            return subtraction;
            break;
        case '*':
            return multiplication;
            break;
        case '/':
            return division;
            break;
        case '^':
            return power;
            break;
    }
    assert(false);
}
inline operation get_real(operation op){
    switch(op){
        case addition:
            return addition;
        case subtraction:
            return addition;
        case multiplication:
            return multiplication;
        case division:
            return multiplication;
        case power:
        case assignment:
        default:
            assert(false && "oof");
            return addition;
            
    }
    return addition;
}
#endif