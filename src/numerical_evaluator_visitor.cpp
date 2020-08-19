#include <numerical_evaluator_visitor.hpp>
#include <parser.hpp>
#include <constant_fusion.hpp>
numerical_evaluator_visitor::numerical_evaluator_visitor(){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const expression& arg){
    assert(false && "numerical evaluator of base class invoked");
    return nullptr;
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const unary_expression& arg){
    std::unique_ptr<expression> childeval = arg.child->accept(*this);
    if(auto v = dynamic_cast<constant*>(childeval.get())){
        switch(v->repr.index()){
            case 0:
                return std::make_unique<constant>(mpz_class(-std::get<0>(v->repr)));
                break;
            case 1:
                return std::make_unique<constant>(mpq_class(-std::get<1>(v->repr)));
                break;
            case 2:
                return std::make_unique<constant>(mpf_class(-std::get<2>(v->repr)));
                break;
        }
    }
    else{
        assert(false && "Wtf");
    }
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const operation_expression& arg){
    std::unique_ptr<expression> leval = arg.left->accept(*this);
    std::unique_ptr<expression> reval = arg.right->accept(*this);
    return fuse(dynamic_cast<constant*>(leval.get()), dynamic_cast<constant*>(reval.get()), arg.m_operation);
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const bracket_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const function& arg){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const function_argument& arg){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const command& arg){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const variable& arg){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const constant& arg){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const operator_expression& arg){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const tensor_expression& arg){

}