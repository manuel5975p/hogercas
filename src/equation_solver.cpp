#include <equation_solver.hpp>
#include <evaluator_visitor.hpp>
#include <expansion_visitor.hpp>
#include <simplifier_visitor.hpp>
#include <parser.hpp>
#include <flatten.hpp>
#include <treeify.hpp>
#include <evaluator_visitor.hpp>
#include <unordered_map>
#include <expression_compare.hpp>
#include <memory.hpp>
struct up_ex_hash{
    size_t operator()(const std::unique_ptr<expression>& ex)const{
        std::stringstream sstr;
        return std::hash<std::string>()(sstr.str());
    }
};
struct up_ex_compare{
    bool operator()(const std::unique_ptr<expression>& l, const std::unique_ptr<expression>& r)const{
        return expression_equal(l, r);
    }
};
std::unique_ptr<expression> solve_single_variable(const std::unique_ptr<expression>& left,const std::unique_ptr<expression>& right, const std::string& x){
    expansion_visitor exv;
    evaluator_visitor evv;
    std::unordered_map<std::unique_ptr<expression>, std::vector<std::unique_ptr<expression>>, up_ex_hash,up_ex_compare> x_factor;

    std::unique_ptr<expression> left_expanded = left->accept(evv)->accept(exv);
    std::unique_ptr<expression> right_expanded = right->accept(evv)->accept(exv);
    std::vector<std::unique_ptr<expression>> lflat;
    std::vector<std::unique_ptr<expression>> rflat;
    if(dynamic_cast<operation_expression*>(left_expanded.get()))
    lflat = flatten(addition, left_expanded);
    else
    lflat.push_back(std::move(left_expanded));
    if(dynamic_cast<operation_expression*>(right_expanded.get()))
    rflat = flatten(addition, right_expanded);
    else
    rflat.push_back(std::move(right_expanded));
    std::vector<std::unique_ptr<expression>> no_x;
    std::unordered_map<std::string, std::unique_ptr<expression>> replmap;
    for(auto& p : current_notebook.variables){
        replmap.insert(std::make_pair(p.first, p.second->clone()));
    }
    simplifier_visitor sv;
    replmap.insert(std::make_pair(x, std::make_unique<constant>("1")));
    evaluator_visitor ev(std::move(replmap));
    for(size_t i = 0;i < lflat.size();i++){
        std::unique_ptr<expression> mn_power = get_power(x, lflat[i]);
        std::unique_ptr<expression> mn_power_no_x = lflat[i]->accept(ev)->accept(sv);
        x_factor[mn_power->clone()].push_back(std::move(mn_power_no_x));
    }

    for(size_t i = 0;i < rflat.size();i++){
        std::unique_ptr<expression> mn_power = get_power(x, rflat[i]);
        std::unique_ptr<expression> mn_power_no_x = rflat[i]->accept(ev)->accept(sv);
        x_factor[mn_power->clone()].push_back(std::make_unique<unary_expression>(negation, std::move(mn_power_no_x)));
    }
    auto zero = std::make_unique<constant>("0");
    auto one = std::make_unique<constant>("1");
    evaluator_visitor ev2;
    if((x_factor.size() == 2) && (x_factor.find(zero->clone()) != x_factor.end()) && (x_factor.find(one->clone()) != x_factor.end())){
        auto a = treeify(addition, x_factor.find(one->clone())->second)->accept(ev2);
        auto b = treeify(addition, x_factor.find(zero->clone())->second)->accept(ev2);
        if(!expression_equal(a, std::make_unique<constant>("0")))
        return std::make_unique<operation_expression>(division, std::make_unique<unary_expression>(negation, std::move(b)), std::move(a))->accept(ev)->accept(sv);
    }
    throw invalid_command("The equation has been a hoger");
    return zero->clone();
}
std::unique_ptr<expression> get_power(const std::string& symbol, const std::unique_ptr<expression>& product_or_division){
    std::vector<std::unique_ptr<expression>> flat;
    if(dynamic_cast<operation_expression*>(product_or_division.get()))
    flat = flatten(multiplication, product_or_division);
    else
    flat.push_back(product_or_division->clone());
    std::vector<std::unique_ptr<expression>> powers;
    for(auto& unqptr : flat){
        if(auto v = dynamic_cast<operation_expression*>(unqptr.get())){
            if(v->m_operation == power){
                if(auto varptr = dynamic_cast<variable*>(v->left.get())){
                    if(varptr->data == symbol){
                        powers.push_back(v->right->clone());
                    }
                }
            }
        }
        else if(auto v = dynamic_cast<variable*>(unqptr.get())){
            if(v->data == symbol)
            powers.push_back(std::make_unique<constant>("1"));
        }
    }
    if(powers.size() == 0){
        return std::make_unique<constant>("0");
    }
    simplifier_visitor sv;
    evaluator_visitor ev;
    std::unique_ptr<expression> tree = treeify(addition, powers);
    return tree->accept(ev)->accept(sv);
}