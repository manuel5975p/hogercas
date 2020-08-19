#include <evaluator_visitor.hpp>
#include <mpreal.h>
#include <memory.hpp>
#include <replace.hpp>
#include <signal.h>
#include <derivative_visitor.hpp>
#include <constant_fusion.hpp>
#include <constant_yeeting_visitor.hpp>
#include <expansion_visitor.hpp>
#include <expression_compare.hpp>
bool is_integer(const std::string& literal){
    for(char c : literal){
        switch(c){
            case '.':
                return false;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            default:
                throw std::logic_error("That literal is fucked");
        }
    }
    return true;
}
inline mpq_class mpq_from_float_literal(std::string str){
    size_t it = str.find('.');
    assert(str.size() - it > 1 && "That literal is fucked");
    size_t pow10 = (str.size() - it) - 1;
    str.erase(it, 1);
    str.erase(0, str.find_first_not_of('0'));
    mpz_class denom = 10;
    mpz_pow_ui(denom.get_mpz_t(), denom.get_mpz_t(), pow10);
    return mpq_class(mpz_class(str), denom);
}
evaluator_visitor::evaluator_visitor() : lookup_table(std::nullopt){

}
evaluator_visitor::evaluator_visitor(std::unordered_map<std::string, std::unique_ptr<expression>> arg) : lookup_table(std::move(arg)){

}
std::unique_ptr<expression> evaluator_visitor::visit(const expression& arg){
    assert(false && "evaluator of base class invoked");
    return nullptr;
}
std::unique_ptr<expression> evaluator_visitor::visit(const unary_expression& arg){
    std::unique_ptr<unary_expression> arg_v = std::make_unique<unary_expression>(arg.type, arg.child->accept(*this));
    switch(arg_v->type){
        case negation:
            if(auto c = dynamic_cast<unary_expression*>(arg_v->child.get())){
                if(c->type == negation){
                    return c->child->accept(*this);
                }
            }
            if(auto c = dynamic_cast<constant*>(arg_v->child.get())){
                switch(c->repr.index()){
                    case 0:
                        return std::make_unique<constant>(mpz_class(-std::get<0>(c->repr)));
                    case 1:
                        return std::make_unique<constant>(mpq_class(-std::get<1>(c->repr)));
                    case 2:
                        return std::make_unique<constant>(mpf_class(-std::get<2>(c->repr)));
                }
            }
            return std::make_unique<unary_expression>(negation, arg_v->child->accept(*this));
        default:
            throw "YEET";
    }
}
std::unique_ptr<expression> evaluator_visitor::visit(const operation_expression& arg){
    std::unique_ptr<expression> left_c = arg.left->accept(*this);
    std::unique_ptr<expression> right_c = arg.right->accept(*this);
    constant_yeeting_visitor cyv;
    expansion_visitor exv;
    if(auto left = dynamic_cast<const constant*>(left_c.get())){
        if(auto right = dynamic_cast<const constant*>(right_c.get())){
            return fuse(left, right, arg.m_operation);
        }
        else{
            goto aels;
        }
    }
    else{
        aels:
        return std::make_unique<operation_expression>(arg.m_operation, std::move(left_c), std::move(right_c))->accept(exv)->accept(cyv);
    }
}
std::unique_ptr<expression> evaluator_visitor::visit(const bracket_expression& arg){
    return arg.child->accept(*this);
}
std::unique_ptr<expression> evaluator_visitor::visit(const function& arg){
    if(current_notebook.functions.find(arg.name) != current_notebook.functions.end()){
        //TODO: Make this faster (O(N) -> O(1))
        for(size_t i = 0;i < already_replaced_functions.size();i++){
            if(already_replaced_functions[i].first == arg.name){
                bool issame = true;
                if(already_replaced_functions[i].second.size() == arg.args.size())
                    for(size_t j = 0;j < already_replaced_functions[i].second.size();j++){
                        if(!expression_equal(already_replaced_functions[i].second[j], arg.args[j])){
                            issame = false;
                        }
                    }
                else
                    issame = false;
                
                if(issame){
                    throw evaluation_error("Function " + arg.name + " appears in the substitution of itself");
                }
            }
        }
        //if(already_replaced_functions.find(arg.name) != already_replaced_functions.end()){
        //    throw evaluation_error("Function " + arg.name + " appears in the substitution of itself");
        //}
        std::unique_ptr<expression> freplace = current_notebook.functions.at(arg.name)->clone();
        std::unordered_map<unsigned, std::unique_ptr<expression>> argument_replacement;
        for(size_t i = 0;i < arg.child_count();i++){
            argument_replacement.insert(std::make_pair(i, const_cast<function&>(arg).get_child(i)->clone()));
        }
        replace_args(freplace, argument_replacement);
        std::unique_ptr<function> argclone(dynamic_cast<function*>(arg.clone().release()));
        already_replaced_functions.push_back(std::make_pair(arg.name, std::move(argclone->args)));
        auto ret = freplace->accept(*this);
        //already_replaced_functions.erase(already_replaced_functions.find(arg.name));
        already_replaced_functions.pop_back();
        return ret;
    }
    else{
        std::vector<std::unique_ptr<expression>> vc;
        for(size_t i = 0;i < arg.child_count();i++){
            vc.push_back(const_cast<function&>(arg).get_child(i)->accept(*this));
        }
        return std::make_unique<function>(arg.name, std::move(vc));
    }
}
std::unique_ptr<expression> evaluator_visitor::visit(const function_argument& arg){
    std::cout << arg.child_count() << "\n";
    assert(false && "Function arguments should not be visited by an evaluator");
    return nullptr;
}
std::unique_ptr<expression> evaluator_visitor::visit(const command& arg){
    //std::cout << arg.child_count() << "\n";
    assert(false && "Commands should not be visited by an evaluator");
    return nullptr;
}
std::unique_ptr<expression> evaluator_visitor::visit(const variable& arg){
    std::unordered_map<std::string, std::unique_ptr<expression>>& lookup_ref
    = lookup_table.has_value() ? lookup_table.value() : current_notebook.variables;

    if(lookup_ref.find(arg.data) != lookup_ref.end()){
        if(already_replaced_variables.find(arg.data) != already_replaced_variables.end()){
            throw evaluation_error("Variable " + arg.data + " appears in the substitution of itself");
        }
        already_replaced_variables.insert(arg.data);
        auto ret = lookup_ref.at(arg.data)->clone()->accept(*this);
        already_replaced_variables.erase(arg.data);
        return ret;
    }
    return arg.clone();
}
std::unique_ptr<expression> evaluator_visitor::visit(const constant& arg){
    return arg.clone();
}
std::unique_ptr<expression> evaluator_visitor::visit(const operator_expression& arg){
    if(auto v = dynamic_cast<const derivation_operator*>(&arg)){
        derivative_visitor dv(v->dsymbol);
        return v->child->accept(dv);
    }
    return nullptr;
}
std::unique_ptr<expression> evaluator_visitor::visit(const tensor_expression& arg){
    return arg.clone();
}