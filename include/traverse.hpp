#ifndef TRAVERSE_HPP
#define TRAVERSE_HPP
#include "parser.hpp"
template<typename Func>
void traverse_preorder(expression* exp, Func f){
    f(exp);
    for(int i = 0;i < exp->child_count();i++){
        traverse_preorder<Func>((exp->get_child(i)).get(), f);
    }
}
template<typename Func>
void traverse_preorder2(expression* exp, Func f, expression* parent = nullptr){
    f(exp);
    for(int i = 0;i < exp->child_count();i++){
        traverse_preorder2<Func>((exp->get_child(i)).get(), f, exp);
    }
}

template<typename Func>
void traverse_postorder(expression* exp, Func f){
    for(int i = 0;i < exp->child_count();i++){
        traverse_postorder<Func>((exp->get_child(i)).get(), f);
    }
    f(exp);
}

template<typename Func>
void traverse_postorder2(expression* exp, Func f, expression* parent = nullptr){
    for(int i = 0;i < exp->child_count();i++){
        traverse_postorder2<Func>((exp->get_child(i)).get(), f, exp);
    }
    f(exp, parent);
}
#endif