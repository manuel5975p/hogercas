#include <simplifier_visitor.hpp>
#include <constant_fusion.hpp>
#include <parser.hpp>
std::unique_ptr<expression> simplifier_visitor::visit(const expression& arg){
    assert("Oof" && false);
}
std::unique_ptr<expression> simplifier_visitor::visit(const unary_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const operation_expression& arg){
    constant zero("0");
    constant one("1");
    std::unique_ptr<expression> leftv = arg.left->accept(*this);
    std::unique_ptr<expression> rightv = arg.right->accept(*this);
    if(arg.m_operation == addition){
        constant* argl = nullptr;
        if(argl = dynamic_cast<constant*>(leftv.get())){
            if(constant_equal(argl, &zero)){
                return rightv;
            }
            else{
                goto _else;
            }
        }
        else if(argl = dynamic_cast<constant*>(rightv.get())){
            if(constant_equal(argl, &zero)){
                return leftv;
            }
            else{
                goto _else;
            }
        }
        else{
            _else:
            return std::make_unique<operation_expression>(arg.m_operation, std::move(leftv), std::move(rightv));
        }
    }
    else if(arg.m_operation == multiplication){
        constant* argl = nullptr;
        if(argl = dynamic_cast<constant*>(leftv.get())){
            if(constant_equal(argl, &one)){
                return rightv;
            }
            else{
                goto _else2;
            }
        }
        else if(argl = dynamic_cast<constant*>(rightv.get())){
            if(constant_equal(argl, &one)){
                return leftv;
            }
            else{
                goto _else2;
            }
        }
        else{
            _else2:
            return std::make_unique<operation_expression>(arg.m_operation, std::move(leftv), std::move(rightv));
        }
    }
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const bracket_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const function& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const function_argument& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const command& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const variable& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const constant& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const operator_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> simplifier_visitor::visit(const tensor_expression& arg){
    return arg.clone();
}