#include <expression_compare.hpp>
#include <parser.hpp>
#include <constant_fusion.hpp>
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const expression& arg){
    assert(false);
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const unary_expression& arg){
    expressions.push_back(&arg);
    arg.child->accept(*this);
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const operation_expression& arg){
    expressions.push_back(&arg);
    arg.left->accept(*this);
    arg.right->accept(*this);
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const bracket_expression& arg){
    assert(false);
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const function& arg){
    expressions.push_back(&arg);
    for(auto& x : arg.args){
        x->accept(*this);
    }
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const function_argument& arg){
    expressions.push_back(&arg);
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const command& arg){
    expressions.push_back(&arg);
    for(auto& x : arg.args){
        x->accept(*this);
    }
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const variable& arg){
    expressions.push_back(&arg);
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const constant& arg){
    expressions.push_back(&arg);
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const operator_expression& arg){
    expressions.push_back(&arg);
    arg.child->accept(*this);
    return nullptr;
}
std::unique_ptr<expression> preorder_enumerator_visitor::visit(const tensor_expression& arg){
    expressions.push_back(&arg);
    for(size_t i = 0;i < arg.child_count();i++)
    const_cast<tensor_expression&>(arg).get_child(i)->accept(*this);
    return nullptr;
}





to_enumeration_comparer_visitor::to_enumeration_comparer_visitor(std::vector<const expression*> exps) : expressions(std::move(exps)), exp_it(expressions.cbegin()), outflag(true){

}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const expression& arg){
    assert(false);
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const unary_expression& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const unary_expression* const>(*exp_it)){
        if(v->type != arg.type){
            outflag = false;
            return nullptr;
        }
        else{
            exp_it++;
            arg.child->accept(*this);
            return nullptr;
        }
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const operation_expression& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const operation_expression* const>(*exp_it)){
        if(v->m_operation != arg.m_operation){
            outflag = false;
            return nullptr;
        }
        else{
            exp_it++;
            arg.left->accept(*this);
            arg.right->accept(*this);
            return nullptr;
        }
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const bracket_expression& arg){
    assert(false);
    if(!outflag)return nullptr;
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const function& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const function* const>(*exp_it)){
        if(v->name != arg.name){
            outflag = false;
            return nullptr;
        }
        else{
            exp_it++;
            for(auto& _arg : arg.args){
                _arg->accept(*this);
            }
            return nullptr;
        }
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const function_argument& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const function_argument* const>(*exp_it)){
        if(v->number != arg.number){
            outflag = false;
            return nullptr;
        }
        else{
            exp_it++;
            return nullptr;
        }
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const command& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const command* const>(*exp_it)){
        if(v->cmd != arg.cmd){
            outflag = false;
            return nullptr;
        }
        else{
            exp_it++;
            for(auto& _arg : arg.args){
                _arg->accept(*this);
            }
            return nullptr;
        }
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const variable& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const variable* const>(*exp_it)){
        if(v->data != arg.data){
            outflag = false;
            return nullptr;
        }
        else{
            exp_it++;
            return nullptr;
        }
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const constant& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const constant* const>(*exp_it)){
        if(!constant_equal(v, &arg)){
            outflag = false;
            return nullptr;
        }
        else{
            exp_it++;
            return nullptr;
        }
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const operator_expression& arg){
    if(!outflag)return nullptr;
    if(const auto v = dynamic_cast<const operator_expression* const>(*exp_it)){
        //TODO: Decide between operator types
        exp_it++;
        arg.child->accept(*this);
        return nullptr;
    }
    else{
        outflag = false;
        return nullptr;
    }
}
std::unique_ptr<expression> to_enumeration_comparer_visitor::visit(const tensor_expression& arg){
    return nullptr;
}
bool expression_equal(const std::unique_ptr<expression>& e1, const std::unique_ptr<expression>& e2){
    preorder_enumerator_visitor prev;
    e1->accept(prev);
    to_enumeration_comparer_visitor tecv(std::move(prev.expressions));
    e2->accept(tecv);
    return tecv.outflag;
}
