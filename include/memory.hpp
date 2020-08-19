#ifndef MEMORY_HPP__dfgdf
#define MEMORY_HPP__dfgdf
#include "parser.hpp"
#include <unordered_map>
#include <unordered_set>
#include <mpreal.h>
#include <notebook.hpp>
struct invalid_command : std::logic_error{
    invalid_command(const char* a) : std::logic_error(a){}
    invalid_command(const std::string& a) : std::logic_error(a){}
    invalid_command(std::string&& a) : std::logic_error(std::move(a)){}
};
extern notebook current_notebook;
//extern std::unordered_map<std::string, std::unique_ptr<expression>> variables;
//extern std::unordered_map<std::string, std::unique_ptr<expression>> functions;
inline void apply(operation_expression* exp){
    if(auto vp = dynamic_cast<variable*>(exp->left.get())){
        traverse_preorder(exp->right.get(),[&vp](expression* ex){
            if(auto x = dynamic_cast<variable*>(ex)){
                if(x->data == vp->data){
                    throw invalid_command("Variable assignment cannot contain the variable itself");
                }
            }
        });
        auto it = current_notebook.variables.find(vp->data);
        if(it != current_notebook.variables.end()){
            current_notebook.variables.erase(it);
        }
        current_notebook.variables.insert({vp->data, exp->right->clone()});
    }
    if(auto fp = dynamic_cast<function*>(exp->left.get())){
        traverse_preorder(exp->right.get(),[&fp](expression* ex){
            if(auto x = dynamic_cast<function*>(ex)){
                if(x->name == fp->name){
                    throw invalid_command("Function definition cannot contain the function itself");
                }
            }
        });
        std::unordered_map<std::string, unsigned int> arguments;
        for(unsigned int i = 0;i < fp->child_count();i++){
            if(auto v = dynamic_cast<variable*>(fp->get_child(i).get())){
                if(arguments.find(v->data) == arguments.end()){
                    arguments.insert({v->data, i});
                    continue;
                }
            }
            throw invalid_command("Function arguments in a declaration must be single and unique symbols only");
        }
        if(auto vp = dynamic_cast<variable*>(exp->right.get())){
            auto ite = arguments.find(vp->data);
            if(ite != arguments.end()){
                exp->right = std::make_unique<function_argument>(ite->second);
            }
        }
        else
        traverse_preorder(exp->right.get(),[&fp, &arguments](expression* ex){
            for(size_t i = 0;i < ex->child_count();i++){
                if(auto vp = dynamic_cast<variable*>(ex->get_child(i).get())){
                    auto ite = arguments.find(vp->data);
                    if(ite != arguments.end()){
                        ex->get_child(i) = std::make_unique<function_argument>(ite->second);
                    }
                }
            }
        });
        auto it = current_notebook.functions.find(fp->name);
        if(it != current_notebook.functions.end()){
            current_notebook.functions.erase(it);
        }
        current_notebook.functions.insert({fp->name, exp->right->clone()});
    }
}
#endif