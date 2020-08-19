#include <flatten.hpp>
#include <parser.hpp>
#include <cassert>
#include <deque>
#include <vector>
#include <evaluator_visitor.hpp>
#include <constant_fusion.hpp>
std::vector<std::unique_ptr<expression>> flatten(operation op, const operation_expression* exp){
    if(precedence(op) != precedence(exp->m_operation)){
        std::vector<std::unique_ptr<expression>> kak;
        kak.push_back(exp->clone());
        return kak;
    }
    return flatten(exp);
}
std::vector<std::unique_ptr<expression>> flatten(const operation_expression* exp){
    operation op = exp->m_operation;
    std::deque<std::pair<bool, const expression*>> to_process;
    std::vector<std::pair<size_t, std::unique_ptr<expression>>> retval;
    to_process.push_back(std::make_pair(false, exp));
    std::unordered_map<const expression*, size_t> order_map;
    size_t order_index = 0;
    traverse_preorder(const_cast<expression*>((const expression*)exp),[&order_map, &order_index](expression* exp){
        order_map[exp] = order_index++;
    });
    while(!to_process.empty()){
        const expression* fromt = to_process.front().second;
        bool sign = to_process.front().first;
        to_process.pop_front();
        if(auto v = dynamic_cast<const operation_expression*>(fromt)){
            //std::cout << (v->m_operation == division) << "\n";
            if(precedence(v->m_operation) == precedence(op)){
                to_process.push_back(std::make_pair(sign,v->left.get()));
                to_process.push_back(std::make_pair(sign ^ ((v->m_operation == subtraction) || (v->m_operation == division)), v->right.get()));
            }
            else{
                goto _else;
            }
        }
        else{
            _else:
            if(precedence(op) == precedence(addition)){
                retval.push_back({order_map.at(fromt),sign ? std::make_unique<unary_expression>(negation, fromt->clone()) : fromt->clone()});
            }
            if(precedence(op) == precedence(multiplication)){
                evaluator_visitor evv;
                if(auto cfromt = dynamic_cast<const constant*>(fromt)){
                    retval.push_back({order_map.at(fromt),sign ? fuse(dynamic_cast<const constant*>(fromt->clone().get()), std::make_unique<constant>("-1").get(), power) : fromt->clone()});
                }
                else{
                    retval.push_back({order_map.at(fromt),sign ? std::make_unique<operation_expression>(power, fromt->clone(), std::make_unique<constant>("-1"))->accept(evv) : fromt->clone()});
                }
            }
        }
    }
    for(auto& x : retval){
        //x->print();
        //std::cout << std::endl;
    }
    //std::cout << std::endl;
    std::sort(retval.begin(), retval.end(), [](const std::pair<size_t, std::unique_ptr<expression>>& l, const std::pair<size_t, std::unique_ptr<expression>>& r){
        return l.first < r.first;
    });
    std::vector<std::unique_ptr<expression>> real_retval;
    //exp->print();
    //std::cout << std::endl;
    std::transform(retval.begin(), retval.end(), std::back_inserter(real_retval), [](std::pair<size_t, std::unique_ptr<expression>>& inp){/*inp.second->print(); std::cout << ", ";*/ return std::move(inp.second);});
    //std::cout << std::endl;
    //std::cout << std::endl;
    return real_retval;
}
std::vector<std::unique_ptr<expression>> flatten(const std::unique_ptr<expression>& exp){
    if(auto v = dynamic_cast<const operation_expression*>(exp.get())){
        return flatten(v);
    }
    else{
        assert(false && "Bigoof");
    }
}
std::vector<std::unique_ptr<expression>> flatten(operation op, const std::unique_ptr<expression>& exp){
    if(auto v = dynamic_cast<const operation_expression*>(exp.get())){
        return flatten(op, v);
    }
    else{
        assert(false && "Bigoof");
    }
}