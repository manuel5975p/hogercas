#include <expansion_visitor.hpp>
#include <parser.hpp>
#include <memory>
#include <vector>
#include <evaluator_visitor.hpp>
#include <treeify.hpp>
std::unique_ptr<expression> expansion_visitor::visit(const expression& arg){
    assert(false && "You binged the bong");
}
std::unique_ptr<expression> expansion_visitor::visit(const unary_expression& arg){
    return std::make_unique<unary_expression>(negation, arg.child->accept(*this));
}
std::unique_ptr<expression> expansion_visitor::visit(const operation_expression& _arg){
    std::unique_ptr<operation_expression> _arg_ = std::make_unique<operation_expression>(
        _arg.m_operation,
        _arg.left->accept(*this),
        _arg.right->accept(*this)
    );
    const operation_expression& arg = *_arg_;
    if(arg.m_operation == multiplication){
        auto left_op = dynamic_cast<operation_expression*>(arg.left.get());
        if(left_op)
        if((left_op->m_operation != addition) && (left_op->m_operation != subtraction)){
            left_op = nullptr;
        }
        auto right_op = dynamic_cast<operation_expression*>(arg.right.get());
        if(right_op)
        if((right_op->m_operation != addition) && (right_op->m_operation != subtraction)){
            right_op = nullptr;
        }
        if(left_op && right_op){
            if((left_op->m_operation == subtraction || left_op->m_operation == addition) && (right_op->m_operation == addition || right_op->m_operation == subtraction)){
                std::vector<std::unique_ptr<expression>> expansion_parts;
                int lrflag = left_op->m_operation == subtraction ? -1 : 1;
                int rrflag = right_op->m_operation == subtraction ? -1 : 1;
                expansion_parts.reserve(4);
                expansion_parts.push_back(std::make_unique<operation_expression>(
                    multiplication,
                    left_op->left->clone(),
                    right_op->left->clone()
                ));
                expansion_parts.push_back(std::make_unique<operation_expression>(
                    multiplication,
                    left_op->left->clone(),
                    right_op->right->clone()
                ));
                expansion_parts.push_back(std::make_unique<operation_expression>(
                    multiplication,
                    left_op->right->clone(),
                    right_op->left->clone()
                ));
                expansion_parts.push_back(std::make_unique<operation_expression>(
                    multiplication,
                    left_op->right->clone(),
                    right_op->right->clone()
                ));
                if(lrflag == -1){
                    expansion_parts[2] = std::make_unique<unary_expression>(negation, std::move(expansion_parts[2]));
                }
                if(rrflag == -1){
                    expansion_parts[1] = std::make_unique<unary_expression>(negation, std::move(expansion_parts[1]));
                }
                if(rrflag * lrflag == -1){
                    expansion_parts[3] = std::make_unique<unary_expression>(negation, std::move(expansion_parts[3]));
                }
                evaluator_visitor evv;
                for(size_t i = 0;i < expansion_parts.size();i++){
                    expansion_parts[i] = expansion_parts[i]->accept(evv)->accept(*this);
                }
                return treeify(addition, expansion_parts);
            }
            return arg.clone();
        }
        else if(left_op || right_op){
            operation_expression* op_one = left_op ? left_op : right_op;
            expression* not_op_one = left_op ? arg.right.get() : arg.left.get();
            if(op_one->m_operation == addition || op_one->m_operation == subtraction){
                std::vector<std::unique_ptr<operation_expression>> expansion_parts;
                std::vector<std::unique_ptr<expression>> expansion_parts_e(2);
                int rflag;
                expansion_parts.reserve(2);
                expansion_parts.push_back(std::make_unique<operation_expression>(
                    multiplication,
                    not_op_one->clone(),
                    op_one->left->clone()
                ));
                expansion_parts.push_back(std::make_unique<operation_expression>(
                    multiplication,
                    not_op_one->clone(),
                    op_one->right->clone()
                ));
                if(op_one == left_op){
                    std::swap(expansion_parts[0]->left, expansion_parts[0]->right);
                    std::swap(expansion_parts[1]->left, expansion_parts[1]->right);
                }
                evaluator_visitor evv;
                for(size_t i = 0;i < expansion_parts.size();i++){
                    if(op_one->m_operation == subtraction && i == 1){
                        expansion_parts_e[i] = std::make_unique<unary_expression>(negation, std::move(expansion_parts[1]));
                        expansion_parts_e[i] = expansion_parts_e[i]->accept(evv)->accept(*this);
                    }
                    else
                    expansion_parts_e[i] = expansion_parts[i]->accept(evv)->accept(*this);
                }
                return treeify(addition, expansion_parts_e);
                //return std::make_unique<operation_expression>(op_one->m_operation, std::move(expansion_parts_e[0]), std::move(expansion_parts_e[1]))->accept(evv);
            }
        }
        return arg.clone();
    }
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const bracket_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const function& arg){
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const function_argument& arg){
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const command& arg){
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const variable& arg){
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const constant& arg){
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const operator_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> expansion_visitor::visit(const tensor_expression& arg){
    return arg.clone();
}