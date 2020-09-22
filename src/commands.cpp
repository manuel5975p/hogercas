#include <commands.hpp>
#include <colors.hpp>
#include <memory.hpp>
#include <optional>
#include <equation_solver.hpp>
#include <evaluator_visitor.hpp>
#include <numerical_evaluator_visitor.hpp>
#include <cassert>
command_result::command_result() : output(std::nullopt), message(std::nullopt){}
command_result::command_result(std::string str) : output(std::nullopt), message(std::move(str)){}
command_result::command_result(std::unique_ptr<expression> exp) : output(std::move(exp)), message(std::nullopt){}
command_result::command_result(std::unique_ptr<expression> exp, std::string str) : output(std::move(exp)), message(std::move(str)){}
command_result execute(command* cmd){
    std::string warning = "";
    if(cmd->cmd == CLEAR){
        for(auto& cptr : cmd->args){
            if(auto v = dynamic_cast<variable*>(cptr.get())){
                if(current_notebook.variables.find(v->data) != current_notebook.variables.end()){
                    current_notebook.variables.erase(current_notebook.variables.find(v->data));
                }
                else{
                    warning += FRED("Warning:");
                    warning += (" Variable ");
                    warning += v->data + " isn't defined";
                    if(current_notebook.functions.find(v->data) != current_notebook.functions.end()){
                        warning += ", but there is a function with that name";
                    }
                }
            }
            else if(auto v = dynamic_cast<function*>(cptr.get())){
                if(current_notebook.functions.find(v->name) != current_notebook.functions.end()){
                    current_notebook.functions.erase(current_notebook.functions.find(v->name));
                }
                else{
                    warning += FRED("Warning:");
                    warning += (" Function ");
                    warning += v->name + " isn't defined";
                    if(current_notebook.variables.find(v->name) != current_notebook.variables.end()){
                        warning += ", but there is a variable with that name";
                    }
                }
            }
        }
        if(warning.empty()){
            return command_result();
        }
        return command_result(std::move(warning));
    }
    if(cmd->cmd == SOLVE){
        if(cmd->args.size() != 3){
            throw invalid_command("Solve needs 3 arguments");
        }
        if(!dynamic_cast<variable*>(cmd->args.back().get())){
            throw invalid_command("Last argument of solve must be a variable");
        }
        std::unique_ptr<expression> sol = solve_single_variable(cmd->args[0], cmd->args[1], dynamic_cast<variable*>(cmd->args[2].get())->data);
        return command_result(std::move(sol));
        //sol->print();
        //std::cout << std::endl;
    }
    if(cmd->cmd == NUMERIC){
        if(cmd->child_count() != 1){
            throw invalid_command("N needs 1 argument");
        }
        evaluator_visitor evalvis;
        numerical_evaluator_visitor nevalvis(1024);
        std::unique_ptr<expression> evaled = cmd->get_child(0)->accept(evalvis);
        std::unique_ptr<expression> numeric_result = evaled->accept(nevalvis);
        return command_result(std::move(numeric_result));
        //numeric_result->print();
        //std::cout << std::endl;
    }
    return command_result();
}