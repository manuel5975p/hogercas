#include <commands.hpp>
#include <colors.hpp>
#include <memory.hpp>
#include <optional>
#include <equation_solver.hpp>
std::optional<std::string> execute(command* cmd){
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
            return std::nullopt;
        }
        return warning;
    }
if(cmd->cmd == SOLVE){
    if(cmd->args.size() != 3){
        throw invalid_command("Solve needs 3 arguments");
    }
    if(!dynamic_cast<variable*>(cmd->args.back().get())){
        throw invalid_command("Last argument of solve must be a variable");
    }
    std::unique_ptr<expression> sol = solve_single_variable(cmd->args[0], cmd->args[1], dynamic_cast<variable*>(cmd->args[2].get())->data);
    sol->print();
    std::cout << std::endl;
}
    return std::nullopt;
}