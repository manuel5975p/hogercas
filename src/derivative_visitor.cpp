#include <derivative_visitor.hpp>
#include <mpreal.h>
#include <functional>
#include <parser.hpp>
derivative_visitor::derivative_visitor(std::string _v) : var(std::move(_v)){}
std::unique_ptr<expression> derivative_visitor::visit(const expression& arg){
    assert(false && "Derivative of base class invoked");
    return nullptr;
}
std::unique_ptr<expression> derivative_visitor::visit(const unary_expression& arg){
    switch(arg.type){
        case negation:
            return std::make_unique<unary_expression>(negation, arg.child->accept(*this));
        default:
            throw "YEET";
    }
}
std::unique_ptr<expression> derivative_visitor::visit(const operation_expression& arg){
    switch(arg.m_operation){
        case subtraction:
        case addition:
            return std::make_unique<operation_expression>(arg.m_operation, arg.left->accept(*this), arg.right->accept(*this));
            break;
        case multiplication:
            return std::make_unique<operation_expression>(addition, std::make_unique<operation_expression>(multiplication, arg.left->clone(), arg.right->accept(*this)), std::make_unique<operation_expression>(multiplication, arg.left->accept(*this), arg.right->clone()));
            break;
        case division:
            return std::make_unique<operation_expression>(
                division,
                std::make_unique<operation_expression>(
                    subtraction,
                    std::make_unique<operation_expression>(multiplication, arg.left->accept(*this), arg.right->clone()),
                    std::make_unique<operation_expression>(multiplication, arg.left->clone(), arg.right->accept(*this))
                    ),
                std::make_unique<operation_expression>(
                    power,
                    arg.right->clone(),
                    std::make_unique<constant>("2")
                )
            );
            break;
        case power:
            assert(dynamic_cast<constant*>(arg.right.get()));
            auto v = dynamic_cast<constant*>(arg.right.get());
            return std::make_unique<operation_expression>(
                multiplication,
                std::make_unique<operation_expression>(
                    multiplication,
                    std::make_unique<operation_expression>(
                        power,
                        arg.left->clone(),
                        std::make_unique<operation_expression>(subtraction, arg.right->clone(), std::make_unique<constant>(1))
                    ),
                    arg.left->accept(*this)
                ),
                arg.right->clone()
            );
            break;
    }
    return nullptr;
}
std::unique_ptr<expression> derivative_visitor::visit(const bracket_expression& arg){
    return std::make_unique<bracket_expression>(arg.child->accept(*this));
}
std::unique_ptr<expression> derivative_visitor::visit(const function& arg){
    std::cout << arg.child_count() << "\n";
    assert(false);
    return nullptr;
}
std::unique_ptr<expression> derivative_visitor::visit(const function_argument& arg){
    std::cout << arg.child_count() << "\n";
    assert(false);
    return nullptr;
}
std::unique_ptr<expression> derivative_visitor::visit(const command& arg){
    std::cout << arg.child_count() << "\n";
    assert(false);
    return nullptr;
}
std::unique_ptr<expression> derivative_visitor::visit(const variable& arg){
    if(arg.data == var){
        return std::make_unique<constant>("1");
    }
    return std::make_unique<constant>("0");
}
std::unique_ptr<expression> derivative_visitor::visit(const constant& arg){
    return std::make_unique<constant>("0");
}
std::unique_ptr<expression> derivative_visitor::visit(const operator_expression& arg){
    return std::make_unique<constant>("0");
}
std::unique_ptr<expression> derivative_visitor::visit(const tensor_expression& arg){
    return std::make_unique<constant>("0");
}
