#include <constant_yeeting_visitor.hpp>
#include <parser.hpp>
#include <flatten.hpp>
#include <functional>
#include <constant_fusion.hpp>
#include <evaluator_visitor.hpp>
#include <treeify.hpp>
std::unique_ptr<expression> constant_yeeting_visitor::visit(const expression& arg){
    throw 5;
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const unary_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const operation_expression& arg){
    
    if(precedence(arg.m_operation) == precedence(addition) || precedence(arg.m_operation) == precedence(multiplication)){
        std::vector<std::unique_ptr<expression>> _flat = flatten(&arg);
        if(_flat.size() <= 2){
            return arg.clone();
        }
        //for(auto& x : _flat){
        //    x->print();
        //    std::cout << ", ";
        //}
        std::vector<std::unique_ptr<expression>> flat;
        evaluator_visitor evv;
        bool inited(false);
        std::unique_ptr<constant> cnst;
        for(size_t i = 0;i < _flat.size();i++){
            _flat[i] = _flat[i]->accept(evv);
            assert(_flat[i].get());
            if(auto v = dynamic_cast<const constant*>(_flat[i].get())){
                //v->print();
                if(!inited){
                    cnst.reset(dynamic_cast<constant*>(v->clone().release()));
                    inited = true;
                }
                else{
                    cnst.reset(dynamic_cast<constant*>(fuse(dynamic_cast<constant*>(cnst.get()), v, get_real(arg.m_operation)).release()));
                }
                //std::cout << std::get<0>(v->repr).get_si() << "  ";
                //std::cout << std::get<0>(cnst->repr).get_si() << "\n";
            }
            else{
                flat.push_back(_flat[i]->clone());
            }
        }
        //std::cout << std::endl;
        if(inited)
            return std::make_unique<operation_expression>(get_real(arg.m_operation), treeify(get_real(arg.m_operation), flat), std::move(cnst));
        else
            return treeify(get_real(arg.m_operation), flat);
    
    }
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const bracket_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const function& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const function_argument& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const command& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const variable& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const constant& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const operator_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> constant_yeeting_visitor::visit(const tensor_expression& arg){
    return arg.clone();
}