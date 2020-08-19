#include <replace.hpp>
#include <parser.hpp>
#include <traverse.hpp>
#include <memory.hpp>
#include <unordered_set>
#include <mpreal.h>
void replace_vars(std::unique_ptr<expression>& exp, std::unordered_set<std::string>& already){
    for(size_t i = 0;i < exp->child_count();i++){
        std::string remove = "";
        if(auto v = dynamic_cast<variable*>(exp->get_child(i).get())){
            auto it = current_notebook.variables.find(v->data);
            auto it2 = already.find(v->data);
            if(it2 != already.end()){
                throw std::logic_error(std::string("Symbol ") + v->data + " appears in the substitution of itself");
            }
            already.insert(v->data);
            remove = v->data;
            if(it != current_notebook.variables.end()){
                exp->get_child(i) = it->second->clone();
            }
        }
        replace_vars(exp->get_child(i), already);
        if(remove != ""){
            already.erase(already.find(remove));
        }
    }
}
void replace_args(std::unique_ptr<expression>& exp,std::unordered_map<unsigned, std::unique_ptr<expression>>& argnames){
    if(auto fa = dynamic_cast<function_argument*>(exp.get())){
        auto it = argnames.find(fa->number);
        exp = it->second->clone();
        return;
    }
    for(size_t i = 0;i < exp->child_count();i++){
        if(auto v = dynamic_cast<function_argument*>(exp->get_child(i).get())){
            auto it = argnames.find(v->number);
            exp->get_child(i) = it->second->clone();
        }
        else{
            replace_args(exp->get_child(i), argnames);
        }
    }
}
void replace_funcs(std::unique_ptr<expression>& exp, std::unordered_set<std::string>& already){
    for(size_t i = 0;i < exp->child_count();i++){
        std::string remove = "";
        std::unordered_set<std::string> localready;
        while(auto v = dynamic_cast<function*>(exp->get_child(i).get())){
            auto it = current_notebook.functions.find(v->name);
            if(it == current_notebook.functions.end())break;
            auto it2 = already.find(v->name);
            if(it2 != already.end()){
                throw std::logic_error(std::string("Function ") + v->name + " appears in the substitution of itself");
            }
            already.insert(v->name);
            remove = v->name;
            if(it != current_notebook.functions.end()){
                std::unordered_map<unsigned, std::unique_ptr<expression>> argument_replacement;
                for(size_t i = 0;i < v->child_count();i++){
                    argument_replacement.insert(std::make_pair(i, v->get_child(i)->clone()));
                }
                exp->get_child(i) = it->second->clone();
                already.insert(it->first);
                localready.insert(it->first);
                replace_args(exp->get_child(i), argument_replacement);
            }
        }
        replace_funcs(exp->get_child(i), already);
        for(auto p : localready){
            already.erase(already.find(p));
        }
    }
}
void replace_funcs(std::unique_ptr<expression>& exp){
    std::unordered_set<std::string> s;
    replace_funcs(exp, s);
}
void replace_vars(std::unique_ptr<expression>& exp){
    std::unordered_set<std::string> s;
    replace_vars(exp, s);
}
std::unique_ptr<expression> evaluate(const std::unique_ptr<expression>& exp){
    std::unique_ptr<expression> local = exp->clone();
    std::unordered_set<expression*> constant_nodes;
    if(auto v = dynamic_cast<variable*>(local.get())){
        auto it = current_notebook.variables.find(v->data);
        if(it != current_notebook.variables.end()){
            local = it->second->clone();
        }
    }
    if(auto v = dynamic_cast<constant*>(local.get())){
        return local;
    }
    std::unique_ptr<expression> brack = std::make_unique<bracket_expression>(std::move(local));
    replace_funcs(brack);
    replace_vars(brack);
    local = std::move(brack->get_child(0));
    traverse_postorder2(local.get(), [&constant_nodes](expression* ex, expression* parent){
        if(ex->child_count() == 0){
            if(dynamic_cast<constant*>(ex)){
                constant_nodes.insert(ex);
            }
        }
        else{
            if(auto fp = dynamic_cast<function*>(ex)){
                //TODO: Lookup for known functions, such as sin, cos, tan etc.
                if(current_notebook.functions.find(fp->name) == current_notebook.functions.end() && fp->name != "sin"){
                    goto nofound;
                }
            }
            for(size_t i = 0;i < ex->child_count();i++){
                if(constant_nodes.find(ex->get_child(i).get()) == constant_nodes.end()){
                    goto nofound;
                }
            }
            constant_nodes.insert(ex);
            nofound:
            (void)0;
        }
    });
    traverse_postorder2(local.get(),  [&constant_nodes](expression* ex, expression* parent){
        for(size_t i = 0;i < ex->child_count();i++){
            if(constant_nodes.find(ex->get_child(i).get()) != constant_nodes.end()){
                std::string newv = ex->get_child(i)->evaluate().toString();
                ex->get_child(i) = std::make_unique<constant>(newv);
                constant_nodes.insert(ex->get_child(i).get());
            }
        }
    });
    if(constant_nodes.find(local.get()) != constant_nodes.end()){
        std::string newv = local->evaluate().toString();
        local = std::make_unique<constant>(newv);
    }
    return local;
}