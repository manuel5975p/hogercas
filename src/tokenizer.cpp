#include <tokenizer.hpp>
#include <iostream>
                //Char length of that token
std::pair<token, size_t> match_at(const std::string& str, size_t pos){
    if((str[pos] >= 'A' && str[pos] <= 'Z') || ((str[pos] >= 'a' && str[pos] <= 'z'))){
        std::string in(str.begin() + pos, std::find_if_not(str.begin() + pos, str.end(), [](const char& c){
            return (c >= 'A' && c <= 'Z') || ((c >= 'a' && c <= 'z'));
        }));
        size_t size = in.size();
        return {token{IDENTIFIER, pos, in}, size};
    }
    if(str[pos] >= '0' && str[pos] <= '9'){
        unsigned int pc = 0;
        std::string in(str.begin() + pos, std::find_if_not(str.begin() + pos, str.end(), 
        [&pc](const char& c){
            return (c >= '0' && c <= '9') || (c == '.' && (++pc));
        }));
        if(pc > 1){
            throw tokenizer_error("More than one dot in literal", std::make_pair(pos, pos + in.size()));
        }
        size_t size = in.size();
        return {token{LITERAL, pos, std::move(in)},size};
    }
    switch(str[pos]){
        case '(':
            return {token{PAR_LEFT,pos},1};
            break;
        case ')':
            return {token{PAR_RIGHT,pos},1};
            break;
        case '[':
            return {token{BRACKET_LEFT,pos},1};
            break;
        case ']':
            return {token{BRACKET_RIGHT,pos},1};
            break;
        case '{':
            return {token{CURLY_LEFT, pos}, 1};
        case '}':
            return {token{CURLY_RIGHT, pos}, 1};
        case '+':
            return {token{PLUS,pos},1};
            break;
        case '-':
            return {token{MINUS,pos},1};
            break;
        case '*':
            return {token{MUL,pos},1};
            break;
        case '/':
            return {token{DIV,pos},1};
            break;
        case '^':
            return {token{POW,pos},1};
            break;
        case ',':
            return {token{SEPARATOR,pos},1};
            break;
        
        case ':':
            if(pos < str.size() - 1){
                if(str[pos + 1] == '='){
                    return {token{ASSIGN,pos},2};
                }
            }
        
        default:
            throw tokenizer_error(std::string("No matching token found: "), std::make_pair(pos, std::max(pos, (size_t)1)));
        case '=':
           throw tokenizer_error(std::string("'=' is not a token, did you mean ':='?"), std::make_pair(pos, pos)); 
    }
    
}
std::vector<token> tokenize(const std::string& str, size_t pos){
    std::vector<token> tokens;
    if(str.size() == 0){
        throw tokenizer_error("Empty string");
    }
    while(true){
        while(str[pos] == ' '){
            pos++;
            if(pos == str.size())goto end;
        }
        std::pair<token, size_t> next = match_at(str, pos);
        tokens.push_back(std::move(next.first));
        pos += next.second;
        if(pos == str.size())break;
    }
    end:
    return tokens;
}
unsigned par_type(token_type tt){
    switch(tt){
        case PAR_LEFT:return 0;break;
        case PAR_RIGHT:return 0;break;
        case BRACKET_LEFT:return 1;break;
        case BRACKET_RIGHT:return 1;break;
        case CURLY_LEFT:return 2;break;
        case CURLY_RIGHT:return 2;break;
        default:
        assert(false && "Input is not a parenthesis-like token");
        return 34987;
    }
}
bool is_par(token_type tt){
    return (tt == PAR_LEFT)||
           (tt == PAR_RIGHT)||
           (tt == BRACKET_LEFT)||
           (tt == BRACKET_RIGHT)||
           (tt == CURLY_LEFT)||
           (tt == CURLY_RIGHT);
}
bool is_opener(token_type tt){
    return (tt == PAR_LEFT) ||(tt == BRACKET_LEFT) || (tt == CURLY_LEFT);
}
bool is_closer(token_type tt){
    return (tt == PAR_RIGHT) ||(tt == BRACKET_RIGHT) || (tt == CURLY_RIGHT);
}