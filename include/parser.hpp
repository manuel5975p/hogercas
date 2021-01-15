#ifndef PARSER_HPP
#define PARSER_HPP
#include <memory>
#include <string>
#include <unordered_map>
#include <exception>
#include <cmath>
#include <vector>
#include <algorithm>
#include <tokenizer.hpp>
#include <visitor_base.hpp>
#include <gmpxx.h>
#include <mpreal.h>
#include <numeric>
#include <variant>
#include <optional>
#include <operations.hpp>
#include <initializer_list>
using scalar = mpfr::mpreal;
using mapping_type = std::unordered_map<std::string, scalar>;
struct parser_error : std::logic_error{
    std::pair<size_t, size_t> error_range;
    parser_error(const char* a) : std::logic_error(a){}
    parser_error(const std::string& a) : std::logic_error(a){}
    parser_error(std::string&& a) : std::logic_error(std::move(a)){}
    parser_error(const char* a       , const std::pair<size_t, size_t>& er) : std::logic_error(a)           , error_range(er){}
    parser_error(const std::string& a, const std::pair<size_t, size_t>& er) : std::logic_error(a)           , error_range(er){}
    parser_error(std::string&& a     , const std::pair<size_t, size_t>& er) : std::logic_error(std::move(a)), error_range(er){}
};
struct union_find{
    std::vector<size_t> vec;
    const size_t _size;
    union_find(size_t s) : vec(s, 0), _size(s){
        std::iota(vec.begin(), vec.end(), 0ull);
    }
    size_t size()const{
        return vec.size();
    }
    size_t find2(size_t i){
        size_t ui = i;
        while(vec[i] != i){
            i = vec[i];
        }
        while(vec[ui] != ui){
            size_t nui = vec[ui];
            vec[ui] = i;
            ui = nui;
        }
        return i;
    }
    size_t find(size_t i)const{
        while(vec[i] != i){
            i = vec[i];
        }
        return i;
    }
    void unite2(size_t i, size_t j){
        size_t _i = find2(i);
        size_t _j = find2(j);
        vec[_i] = _j;
    }
    void unite(size_t i, size_t j){
        size_t _i = find(i);
        size_t _j = find(j);
        vec[_i] = _j;
    }
};
enum node_id : char{
    unary_expression_e, bracket_expression_e, variable_e, derivation_operator_e, constant_e, command_e, function_e, function_argument_e, operation_expression_e, tensor_expression_e
};
struct expression{
    protected:
    expression(){/*throw std::logic_error("Pls don't construct expression base class.");*/}
    public:
    virtual scalar evaluate(const mapping_type& symbols){throw 7;}
    virtual scalar evaluate(){
        std::unordered_map<std::string, scalar> map;
        return this->evaluate(map);
    }
    virtual size_t child_count()const{throw 8;}
    virtual std::unique_ptr<expression>& get_child(size_t i){throw 9;}
    virtual std::unique_ptr<expression> clone()const{throw 10;}
    virtual node_id id()const = 0;
    virtual void print(std::ostream& ost = std::cout)const{throw 11;}
    virtual ~expression() = default;
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};
struct unary_expression : expression{
    unary_operation type;
    std::unique_ptr<expression> child;
    unary_expression(unary_operation t, std::unique_ptr<expression>&& c)
    : type(t), child(std::move(c))
    {}
    virtual scalar evaluate(const mapping_type& symbols){
        if(type == negation){
            return -(child->evaluate(symbols));
        }
        else{
            return 0;
        }
    }
    node_id id()const{return unary_expression_e;}
    virtual size_t child_count()const{
        return 1;
    }
    virtual std::unique_ptr<expression> clone()const{
        return std::make_unique<unary_expression>(type, child->clone());
    }
    virtual std::unique_ptr<expression>& get_child(size_t i){
        assert(i < child_count());
        return child;
    }
    virtual void print(std::ostream& ost = std::cout)const;
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};

struct bracket_expression : expression{
    std::unique_ptr<expression> child;
    bracket_expression(std::unique_ptr<expression> c) : child(std::move(c)){

    }
    virtual scalar evaluate(const mapping_type& symbols){
        return child->evaluate(symbols);
    }
    node_id id()const{return bracket_expression_e;}
    virtual size_t child_count()const{
        return 1;
    }
    virtual std::unique_ptr<expression>& get_child(size_t i){
        assert(i < child_count());
        return child;
    }
    virtual std::unique_ptr<expression> clone()const{
        return std::make_unique<bracket_expression>(child->clone());
    }
    virtual void print(std::ostream& ost = std::cout)const{
        ost << '(';
        child->print(ost);
        ost << ')';
    }
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};

struct variable : expression{
    std::string data;
    variable(std::string d) : data(std::move(d)){}
    virtual scalar evaluate(const mapping_type& symbols){
        auto it = symbols.find(data);
        if(it == symbols.end()){
            throw parser_error(std::string("Symbol ") + data +  " not defined");
        }
        return it->second;
    }
    virtual size_t child_count()const{
        return 0;
    }
    node_id id()const{return variable_e;}
    virtual std::unique_ptr<expression>& get_child(size_t i){
        //#pragma GCC diagnostic ignored "-Wreturn-type"
        assert(false && "This node has no children");
        throw 123;
    }
    virtual std::unique_ptr<expression> clone()const{
        return std::make_unique<variable>(data);
    }
    virtual void print(std::ostream& ost = std::cout)const{
        ost << data;
    }
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};
struct operator_expression : expression{
    std::unique_ptr<expression> child;
    operator_expression(std::unique_ptr<expression> a)
    : child(std::move(a))
    {

    }
    virtual scalar evaluate(const mapping_type& symbols){
        assert(false && "Evaluation for operator base not supproted");
        return 0;
    }
    virtual size_t child_count()const{
        return 1;
    }
    virtual std::unique_ptr<expression>& get_child(size_t i){
        assert(i < child_count());
        return child;
    }
    //virtual std::unique_ptr<expression> clone()const{
        //return std::make_unique<operator_expression>(child->clone());
    //}
    virtual void print(std::ostream& ost = std::cout)const{
        assert(false && "OOF");
    }
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};

struct derivation_operator : operator_expression{
    std::string dsymbol;
    node_id id()const{return derivation_operator_e;}
    derivation_operator(std::unique_ptr<expression> c, std::string dx) : operator_expression(std::move(c)), dsymbol(std::move(dx)){

    }

};

struct constant : expression{
    std::string actual_if_float;
    std::variant<mpz_class, mpq_class, mpf_class> repr;
    constant(std::variant<mpz_class, mpq_class, mpf_class> _r)
    {
        repr = _r;
    }
    constant(std::string data){
        //std::cout << "Constructing constant: " << data << "\n";
        assert((data.find('.') == std::string::npos) || (data.find('.') == std::string::npos));
        if(data.find('.') != std::string::npos){
            repr = mpf_class(data);
            actual_if_float = std::move(data);
        }
        else if(data.find('/') != std::string::npos){
            repr = mpq_class(data);
            actual_if_float = std::move(data);
        }
        else{
            repr = mpz_class(data);
        }
    }
    
    constant(std::string n, std::string d){
        repr = mpq_class(mpz_class(n), mpz_class(d));
    }
    constant(const mpz_class& mpz_v){
        //std::cout << "Constructing dconstant: " << mpz_v << "\n";
        repr = mpz_v;
    }
    constant(const mpf_class& mpf_v){
        repr = mpf_v;
    }
    constant(const mpq_class& mpq_v){
        repr = mpq_v;
    }
    constant(int i_v){
        repr = mpz_class(i_v);
    }
    node_id id()const{return constant_e;}
    virtual scalar evaluate(const mapping_type& symbols){
        if constexpr(std::is_same_v<scalar, float>){
            return std::stof(to_string());
        }
        else if constexpr(std::is_same_v<scalar, double>){
            return std::stod(to_string());
        }
        else{
            return scalar(to_string());
        }
    }
    virtual size_t child_count()const{
        return 0;
    }
    virtual std::unique_ptr<expression>& get_child(size_t i){
        //#pragma GCC diagnostic ignored "-Wreturn-type"
        assert(false && "This node has no children");
        throw 123;
    }
    virtual std::unique_ptr<expression> clone()const{
        return std::make_unique<constant>(*this);
    }
    std::string to_string(){
        std::stringstream ss;
        switch(repr.index()){
            case 0:
                return std::get<0>(repr).get_str();
                break;
            case 1:
                return std::get<1>(repr).get_str();
                break;
            case 2:
                ss << std::get<2>(repr);
                return ss.str();
                break;
            default:
                throw std::logic_error("Yeet");
        }
    }
    virtual void print(std::ostream& ost = std::cout)const{
        
        switch(repr.index()){
            case 0:
                ost << std::get<0>(repr);
                break;
            case 1:
                ost << std::get<1>(repr);
                break;
            case 2:
                ost.precision(std::get<2>(repr).get_prec());
                ost << std::get<2>(repr);
                //ost << " (Precision = " << std::get<2>(repr).get_prec() << ")";
                break;
        }
    }
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
    
};
enum command_type : uint64_t{
    CLEAR, SOLVE, NUMERIC
};
inline std::string cmd_string(command_type type){
    switch(type){
        case CLEAR:
            return std::string("Clear");
        case SOLVE:
            return std::string("Solve");
        case NUMERIC:
            return std::string("N");
    }
    assert(false && "Invalid command type");
    return std::string("");
}
inline std::optional<command_type> cmd_type(const std::string& r){
    //TODO Fix this shit
    if(r == "Clear"){
        return CLEAR;
    }
    else if(r == "Solve"){
        return SOLVE;
    }
    else if(r == "N"){
        return NUMERIC;
    }
    return std::nullopt;
}
struct command : expression{
    command_type cmd;
    std::vector<std::unique_ptr<expression>> args;
    command(command_type c, std::vector<std::unique_ptr<expression>> a)
    : cmd(c),args(std::move(a))
    {
    }
    virtual size_t child_count()const{
        return args.size();
    }
    virtual std::unique_ptr<expression>& get_child(size_t i){
        assert(i < child_count());
        return args[i];
    }
    virtual std::unique_ptr<expression> clone()const{
        std::vector<std::unique_ptr<expression>> vec;
        for(size_t i = 0;i < args.size();i++){
            vec.push_back(std::move(args[i]->clone()));
        }
        return std::make_unique<command>(cmd, std::move(vec));
    }
    node_id id()const{return command_e;}
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
    virtual void print(std::ostream& ost = std::cout)const{
        ost << cmd_string(cmd);
        ost << '[';
        for(size_t i = 0;i < args.size();i++){
            args[i]->print(ost);
            if(i < args.size() - 1){
                ost << ", ";
            }
        }
        ost << ']';
    }
};
struct tensor_expression : expression{
    size_t dimensions;
    std::vector<size_t> extents;
    std::vector<std::unique_ptr<expression>> entries;
    tensor_expression(const std::initializer_list<size_t>& ext) : extents(ext), entries(std::accumulate(extents.begin(), extents.end(), size_t(1), [](size_t l, size_t r){return l * r;})){

    }
    tensor_expression(const std::vector<size_t>& ext) : extents(ext), entries(std::accumulate(extents.begin(), extents.end(), size_t(1), [](size_t l, size_t r){return l * r;})){

    }
    tensor_expression(const std::initializer_list<size_t>& ext, const std::unique_ptr<expression>& exp) : extents(ext), entries(std::accumulate(extents.begin(), extents.end(), size_t(1), [](size_t l, size_t r){return l * r;})){
        std::generate(entries.begin(), entries.end(), [&exp]{return exp->clone();});
    }
    tensor_expression(const std::vector<size_t>& ext, const std::unique_ptr<expression>& exp) : extents(ext), entries(std::accumulate(extents.begin(), extents.end(), size_t(1), [](size_t l, size_t r){return l * r;})){
        std::generate(entries.begin(), entries.end(), [&exp]{return exp->clone();});
    }
    virtual scalar evaluate(const mapping_type& symbols){
        return 0;
    }
    virtual scalar evaluate(){
        std::unordered_map<std::string, scalar> map;
        return this->evaluate(map);
    }
    virtual size_t child_count()const{
        return std::accumulate(extents.begin(), extents.end(), size_t(1), [](size_t l, size_t r){return l * r;});
    }
    virtual std::unique_ptr<expression>& get_child(size_t i){
        return entries[i];
    }
    virtual std::unique_ptr<expression> clone()const{
        std::unique_ptr<tensor_expression> ret = std::make_unique<tensor_expression>(extents);
        std::transform(entries.begin(), entries.end(), ret->entries.begin(), [](const std::unique_ptr<expression>& in){
            if(in.get()){
                return in->clone();
            }
            return std::unique_ptr<expression>(nullptr);
        });
        return ret;
    }
    virtual node_id id()const{
        return tensor_expression_e;
    }
    virtual void print(std::ostream& ost = std::cout){
        for(size_t i = 0;i < entries.size();i++){
            entries[i]->print(ost);
            ost << ", ";
        }
    }
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};
struct function : expression{
    std::string name;
    std::vector<std::unique_ptr<expression>> args;
    function(std::string n, std::vector<std::unique_ptr<expression>> a)
    : name(std::move(n)),args(std::move(a))
    {
    }
    virtual scalar evaluate(const mapping_type& symbols){
        throw std::logic_error("Function name " + name + " not supported");
    }
    virtual size_t child_count()const{
        return args.size();
    }
    virtual std::unique_ptr<expression>& get_child(size_t i){
        assert(i < child_count());
        return args[i];
    }
    virtual std::unique_ptr<expression> clone()const{
        std::vector<std::unique_ptr<expression>> vec;
        for(size_t i = 0;i < args.size();i++){
            vec.push_back(std::move(args[i]->clone()));
        }
        return std::make_unique<function>(name, std::move(vec));
    }
    virtual void print(std::ostream& ost = std::cout)const{
        ost << name;
        ost << '(';
        for(size_t i = 0;i < args.size();i++){
            args[i]->print(ost);
            if(i < args.size() - 1){
                ost << ", ";
            }
        }
        ost << ')';
    }
    node_id id()const{return function_e;}
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};
struct function_argument : expression{
    unsigned int number;
    function_argument(unsigned int n) : number(n){}
    virtual void print(std::ostream& ost = std::cout)const{
        ost << "arg(" << number << ")";
    }
    virtual size_t child_count()const{
        return 0;
    }
    virtual std::unique_ptr<expression> clone()const{
        return std::make_unique<function_argument>(number);
    }
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
    node_id id()const{return function_argument_e;}
};
struct operation_expression : expression{
    operation m_operation;
    operation_expression(operation op) : m_operation(op){

    }
    operation_expression(operation op, std::unique_ptr<expression>&& l, std::unique_ptr<expression>&& r)
     : m_operation(op), left(std::move(l)), right(std::move(r)){

    }
    std::unique_ptr<expression> left;
    std::unique_ptr<expression> right;
    virtual scalar evaluate(const mapping_type& symbols){
        using std::pow;
        switch(m_operation){
            case addition:
                return left->evaluate(symbols) + right->evaluate(symbols);
                break;
            case subtraction:
                return left->evaluate(symbols) - right->evaluate(symbols);
                break;
            case multiplication:
                return left->evaluate(symbols) * right->evaluate(symbols);
                break;
            case division:
                return left->evaluate(symbols) / right->evaluate(symbols);
                break;
            case power:
                return pow(left->evaluate(symbols), right->evaluate(symbols));
                break;
            
        }
        throw parser_error("Operation type of this node is crap (Probably an assignment)");
    }
    virtual size_t child_count()const{
        return 2;
    }
    node_id id()const{return operation_expression_e;}
    virtual std::unique_ptr<expression>& get_child(size_t i){
        assert(i < child_count());
        return i == 0 ? left : right;
    }
    virtual std::unique_ptr<expression> clone()const{
        return std::make_unique<operation_expression>(m_operation, left->clone(), right->clone());
    }
    virtual void print(std::ostream& ost = std::cout)const{
        assert(left.get() && "Left is nullptr");
        assert(right.get() && "Right is nullptr");
        bool f1 = false;
        if(auto v = dynamic_cast<operation_expression*>(left.get())){
            if(precedence(v->m_operation) > precedence(m_operation) || (v->m_operation == power && m_operation == power)){
                ost << "(";
                f1 = true;
            }
            
        }
        left->print(ost);
        if(f1)ost << ")";
        f1 = false;
        switch(m_operation){
            case addition:
                ost << " + ";
                break;
            case subtraction:
                ost << " - ";
                break;
            case multiplication:
                ost << " * ";
                break;
            case division:
                ost << " / ";
                break;
            case power:
                ost << " ^ ";
                break;
            case assignment:
                ost << " := ";
                break;
            
        }
        if(auto v = dynamic_cast<operation_expression*>(right.get())){
            if(precedence(v->m_operation) > precedence(m_operation)){
                ost << "(";
                f1 = true;
            }
            else if((m_operation == division || m_operation == subtraction) && precedence(v->m_operation) >= precedence(m_operation)){
                ost << "(";
                f1 = true;
            }
        }
        
        right->print(ost);
        if(f1)ost << ")";
        f1 = false;
    }
    virtual std::unique_ptr<expression> accept(visitor_base& visitor)const{
        return visitor.visit(*this);
    }
};
inline void unary_expression::print(std::ostream& ost)const{
    if(type == negation){
        ost << '-';
        bool f = false;
        if(dynamic_cast<operation_expression*>(child.get())){
            ost << '(';
            f = true;
        }
        child->print(ost);
        if(f){
            ost << ')';
        }
    }
}
template<typename token_iterator>
std::unique_ptr<expression> parse(token_iterator begin, token_iterator end);
template<typename token_iterator>
std::unique_ptr<expression> parse_tensor(token_iterator begin, token_iterator end);
template<typename iterator>
iterator find_counter(iterator it, iterator limit){
    int c = 1;
    size_t stpos = it->opos;
    size_t endpos = it->opos;
    auto itype = it->type;
    assert(itype == PAR_LEFT || itype == CURLY_LEFT || itype == BRACKET_LEFT);
    token_type counter = token_type((int)itype + 1);
    ++it;
    while(it < limit){
        if(it->type == itype)c++;
        if(it->type == counter)c--;
        if(c == 0)return it;
        endpos = it->opos;
        ++it;
    }
    throw parser_error("No counter parenthesis-like token found", std::make_pair(stpos, endpos + 1));
}
template<typename iterator>
iterator find_next_stop(iterator begin, iterator limit, token_type opener, token_type closer){
    int opencount[3] = {0,0,0};
    while(begin < limit){
        if(is_opener(begin->type)){
            opencount[par_type(begin->type)]++;
        }
        if(is_closer(begin->type)){
            //std::for_each(opencount, opencount+ 3, [](int a){std::cout << a << ", ";});
            if(std::accumulate(opencount, opencount + 3, 0,[](int a, int b){return a | b;})){
                opencount[par_type(begin->type)]--;
                if(std::find_if(opencount, opencount + 3, [](int a){return a < 0;}) != opencount + 3){
                    throw parser_error("Too many parenthesises closing here", {begin->opos, begin->opos});
                }
            }
            else{
                return begin;
            }

        }
        if(begin->type == SEPARATOR){
            if(!std::accumulate(opencount, opencount + 3, 0,[](int a, int b){return a | b;})){
                return begin;
            }
        }
        begin++;
    }
    return begin;
}
template<typename iterator>
std::unique_ptr<expression> consume_one(iterator& pos, iterator limit){
    token red = *pos;
    if(red.type == PAR_LEFT){
        std::unique_ptr<expression> ret = parse(pos + 1, find_counter(pos, limit));
        pos = find_counter(pos, limit);
        pos++;
        return std::move(ret);
    }
    if(red.type == CURLY_LEFT){
        std::unique_ptr<expression> ret = parse_tensor(pos + 1, find_counter(pos, limit));
        pos = find_counter(pos, limit);
        pos++;
        return std::move(ret);
    }
    if(red.type == MINUS){
        pos++;
        std::unique_ptr<expression> ret = std::make_unique<unary_expression>(negation, consume_one(pos, limit));
        return ret;
    }
    else{
        if(red.type == LITERAL){
            pos++;
            return std::make_unique<constant>(std::move(red.data));
        }
        if(red.type == IDENTIFIER){
            pos++;
            if(pos < limit){
                token follow = *pos;
                if(follow.type == PAR_LEFT){
                    std::vector<std::unique_ptr<expression>> args;
                    size_t errpos = pos->opos;
                    pos++;
                    while(true){
                        iterator next_stop = find_next_stop(pos, limit, PAR_LEFT, PAR_RIGHT);
                        //iterator next_stop = std::find_if(pos, limit, [](const auto& x){return (x.type == PAR_RIGHT) || (x.type == SEPARATOR);});
                        if(next_stop >= limit){
                            throw parser_error("No closing function parenthesis found", std::make_pair(errpos, errpos));
                        }
                        if(pos == next_stop){
                            if(pos->type == PAR_RIGHT && args.size() == 0){
                                break;
                            }
                            throw parser_error("One function argument is empty", std::make_pair(errpos, errpos + 2));
                        }
                        std::unique_ptr<expression> ret = parse(pos, next_stop);
                        args.push_back(std::move(ret));
                        pos = next_stop;
                        errpos = pos->opos;
                        if(pos->type == PAR_RIGHT){
                            break;
                        }
                        pos++;
                    }
                    
                    //pos = find_counter(pos, limit);
                    pos++;
                    return std::make_unique<function>(red.data, std::move(args));
                }
                if(follow.type == BRACKET_LEFT){
                    std::vector<std::unique_ptr<expression>> args;
                    pos++;
                    while(true){
                        iterator next_stop = find_next_stop(pos, limit, BRACKET_LEFT, BRACKET_RIGHT);
                        //iterator next_stop = std::find_if(pos, limit, [](const auto& x){return (x.type == BRACKET_RIGHT) || (x.type == SEPARATOR);});
                        if(next_stop == limit){
                            throw parser_error("No closing command bracket found", std::make_pair(pos->opos, pos->opos));
                        }
                        if(pos == next_stop){
                            throw parser_error("One command argument is empty", std::make_pair(pos->opos, pos->opos));
                        }
                        std::unique_ptr<expression> ret = parse(pos, next_stop);
                        args.push_back(std::move(ret));
                        pos = next_stop;
                        if(pos->type == BRACKET_RIGHT){
                            break;
                        }
                        pos++;
                    }
                    pos++;
                    if(!cmd_type(red.data)){
                        throw parser_error(std::string("Invalid command: ") + red.data, {red.opos,red.opos});
                    }
                    return std::make_unique<command>(cmd_type(red.data).value(), std::move(args));
                }
            }
            return std::make_unique<variable>(std::move(red.data));
        }
        throw parser_error("Cannot consume one here: Unable to can", {pos->opos, pos->opos});
    }
}
template<typename token_iterator>
std::unique_ptr<expression> parse(token_iterator begin, token_iterator end){
    std::vector<std::unique_ptr<expression>> current_level;
    std::vector<std::pair<operation, size_t>> current_level_ops;
    bool expect_op = false;
    for(auto it = begin;it < end;){
        const token& red = *it;
        if(!expect_op){
            std::unique_ptr<expression> ptr = consume_one(it, end);
            current_level.push_back(std::move(ptr));
            expect_op = true;
        }
        else if(expect_op){
            if(red.type == PLUS || red.type == MINUS || red.type == MUL || red.type == DIV || red.type == POW || red.type == ASSIGN){
                current_level_ops.push_back({operation_type(red.type), current_level_ops.size()});
                it++;
                expect_op = false;
            }
            else if(red.type == PAR_RIGHT){
                throw parser_error("Too many closing parentheses", {red.opos, red.opos});
            }
            else{
                throw parser_error("Unexpected token", {red.opos, red.opos});
            }
        }
    }
    if(!expect_op){
        throw parser_error("Expression is either empty or ends with operator", {(end - 1)->opos, (end - 1)->opos});
    }
    std::stable_sort(current_level_ops.begin(), current_level_ops.end(), [](const std::pair<operation, size_t>& a1, const std::pair<operation, size_t>& a2){return precedence(a1.first) < precedence(a2.first);});
    union_find uf(current_level.size());
    for(size_t i = 0;i < current_level_ops.size();i++){
        std::unique_ptr<expression> newex = std::make_unique<operation_expression>
        (current_level_ops[i].first,std::move(current_level[uf.find2(current_level_ops[i].second)]), std::move(current_level[uf.find2(current_level_ops[i].second + 1)]));
        uf.unite2(current_level_ops[i].second, current_level_ops[i].second + 1);
        current_level[uf.find(current_level_ops[i].second)] = std::move(newex);
    }
    for(size_t i = 1;i < uf.size();i++){
        assert(uf.find(i) == uf.find(0));
    }
    return std::move(current_level[uf.find(0)]);
}
template<typename token_iterator>
std::unique_ptr<expression> parse_tensor(token_iterator begin, token_iterator end){
    std::vector<std::unique_ptr<expression>> current_level;
    while(true){
        token_iterator next_stop = find_next_stop(begin, end, CURLY_LEFT, CURLY_RIGHT);
        
        //if(std::find_if(begin, next_stop + 1,[](const token& a){return a.type == CURLY_RIGHT || a.type == CURLY_LEFT;}) != next_stop + 1){
            if(begin->type == CURLY_LEFT && next_stop->type == CURLY_RIGHT){
                current_level.push_back(parse_tensor(begin + 1, next_stop));
            }
            else{
                current_level.push_back(parse(begin, next_stop));
                //current_level.back()->print();
                //std::cout << " : " << scope << "\n";
            }
        //}
        begin = next_stop + 1;
        if(begin >= end){
            if(next_stop->type == SEPARATOR){
                throw parser_error("Superfluous comma", {next_stop->opos, next_stop->opos});
            }
            break;
        }
    }
    if(auto v = dynamic_cast<tensor_expression*>(current_level.front().get())){
        std::vector<size_t> bextents = v->extents;
        if(current_level.size() > 1)
        for(size_t it = 1;it < current_level.size();it++){
            if(auto v2 = dynamic_cast<const tensor_expression*>(current_level[it].get())){
                if(v2->extents.size() == bextents.size()){
                    for(size_t i = 0;i < bextents.size();i++){
                        if(v2->extents[i] != bextents[i]){
                            throw parser_error("Ill-Shaped Tensor", {0, 0});
                        }
                    }
                }
                else{
                    throw parser_error("Ill-Shaped Tensor", {0,0});
                }
            }
            else{
                throw parser_error("Ill-Shaped Tensor", {0,0});
            }
        }
        bextents.insert(bextents.begin(), current_level.size());
        std::unique_ptr<tensor_expression> retval = std::make_unique<tensor_expression>(bextents);
        auto it = retval->entries.begin();
        for(size_t i = 0;i < current_level.size();i++){
            it = std::move(dynamic_cast<tensor_expression*>(current_level[i].get())->entries.begin(),
                           dynamic_cast<tensor_expression*>(current_level[i].get())->entries.end(),
                           it);
        }
        return retval;
    }
    else{
        if(current_level.size() > 1)
        for(size_t it = 1;it < current_level.size();it++){
            if(auto v2 = dynamic_cast<tensor_expression*>(current_level[it].get())){
                throw parser_error("Ill-Shaped Tensor", {0,0});
            }
        }
        std::vector<size_t> trivial_extents{current_level.size()};
        std::unique_ptr<tensor_expression> retval = std::make_unique<tensor_expression>(trivial_extents);
        std::move(current_level.begin(), current_level.end(), retval->entries.begin());
        return retval;
    }
}
inline bool check(std::unique_ptr<expression>& exp);
inline std::unique_ptr<expression> parse(const std::string& str){
    std::vector<token> xff = tokenize(str);
    std::unique_ptr<expression> ret = parse(xff.begin(), xff.end());
    if(!check(ret)){
        throw parser_error("Invalid expression tree: Assignments are broken");
    }
    return ret;
}
#include "traverse.hpp"
inline bool check(std::unique_ptr<expression>& exp){
    expression* e = exp.get();
    unsigned int ac = 0;
    traverse_preorder(e, [&ac](expression* ex){if(auto v = dynamic_cast<operation_expression*>(ex)){
        if(v->m_operation == operation::assignment){
            ac++;
        }
    }});
    if(ac > 1)return false;
    if(ac == 1){
        if(auto v = dynamic_cast<operation_expression*>(e)){
            if(v->m_operation == operation::assignment){
                if(dynamic_cast<function*>(v->left.get()) || dynamic_cast<variable*>(v->left.get())){
                    return true;
                }
            }
        }
        return false;
    }
    return true;
}
#endif
