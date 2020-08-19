#include <treeify.hpp>
#include <parser.hpp>
#include <iostream>
std::unique_ptr<expression> treeify(operation op, const std::vector<std::unique_ptr<expression>>& vec){
    assert(vec.size() && "Input cannot be ämpti");
    std::vector<std::unique_ptr<expression>> next_stage;
    std::vector<std::unique_ptr<expression>> current_stage;
    for(size_t i = 0;i < vec.size();i++){
        //vec[i]->print();
        //std::cout << " ";
        current_stage.push_back(vec[i]->clone());
    }
    //std::cout << std::endl;
    while(true){
        size_t i = 0;
        for(i;i < (current_stage.size() - (current_stage.size() % 2));i += 2){
            next_stage.push_back(
            std::make_unique<operation_expression>(op, std::move(current_stage[i]), std::move(current_stage[i + 1])));
        }
        if(current_stage.size() & 1){
            next_stage.push_back(std::move(current_stage.back()));
        }
        if(next_stage.size() == 1){
            return std::move(next_stage.front());
            //return ret;
        }
        //std::terminate();
        //std::cout << next_stage.size() << "\n";
        current_stage = std::move(next_stage);
        next_stage.clear();
    }
}