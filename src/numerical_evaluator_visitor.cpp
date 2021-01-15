#include <numerical_evaluator_visitor.hpp>
#include <parser.hpp>
#include <constant_fusion.hpp>
#include <evaluator_visitor.hpp>
numerical_evaluator_visitor::numerical_evaluator_visitor(size_t prec) : precision(prec){

}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const expression& arg){
    assert(false && "numerical evaluator of base class invoked");
    return nullptr;
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const unary_expression& arg){
    std::unique_ptr<expression> childeval = arg.child->accept(*this);
    if(auto v = dynamic_cast<constant*>(childeval.get())){
        switch(v->repr.index()){
            case 0:
                return std::make_unique<constant>(mpz_class(-std::get<0>(v->repr)));
                break;
            case 1:
                return std::make_unique<constant>(mpq_class(-std::get<1>(v->repr)));
                break;
            case 2:
                return std::make_unique<constant>(mpf_class(-std::get<2>(v->repr)));
                break;
            default:
                assert(false && "Wtf");
        }
    }
    else{
        assert(false && "Wtf");
    }
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const operation_expression& arg){
    std::unique_ptr<expression> leval = arg.left->accept(*this);
    std::unique_ptr<expression> reval = arg.right->accept(*this);
    return fuse_numerical(dynamic_cast<constant*>(leval.get()), dynamic_cast<constant*>(reval.get()), arg.m_operation);
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const bracket_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const function& arg){
    if(arg.name == "sin"){
        std::unique_ptr<expression> ceval = arg.args[0]->accept(*this);
        if(auto v = dynamic_cast<constant*>(ceval.get())){
            mpfr::mpreal mpr;
            switch(v->repr.index()){
                case 0:
                mpr = mpfr::mpreal(std::get<0>(v->repr).get_mpz_t(), precision);
                break;
                case 1:
                mpr = mpfr::mpreal(std::get<1>(v->repr).get_mpq_t(), precision);
                break;
                case 2:
                mpr = mpfr::mpreal(std::get<2>(v->repr).get_mpf_t());
                break;
            }
            mpr.set_prec(precision);
            mpr = mpfr::sin(mpr);
            mpf_class ret;
            ret.set_prec(precision);
            mpfr_get_f(ret.get_mpf_t(), mpr.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
            return std::make_unique<constant>(ret);
        }
        else{
            assert(false && "NEval didn't yield constant");
        }
    }
    else if(arg.name == "cos"){
        std::unique_ptr<expression> ceval = arg.args[0]->accept(*this);
        if(auto v = dynamic_cast<constant*>(ceval.get())){
            mpfr::mpreal mpr;
            switch(v->repr.index()){
                case 0:
                mpr = mpfr::mpreal(std::get<0>(v->repr).get_mpz_t(), precision);
                break;
                case 1:
                mpr = mpfr::mpreal(std::get<1>(v->repr).get_mpq_t(), precision);
                break;
                case 2:
                mpr = mpfr::mpreal(std::get<2>(v->repr).get_mpf_t());
                break;
            }
            mpr.set_prec(precision);
            mpr = mpfr::cos(mpr);
            mpf_class ret;
            ret.set_prec(precision);
            mpfr_get_f(ret.get_mpf_t(), mpr.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
            return std::make_unique<constant>(ret);
        }
        else{
            assert(false && "NEval didn't yield constant");
        }
    }
    else{
        throw evaluation_error("Unrecognized function: " + arg.name);
    }
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const function_argument& arg){
    assert(false && "Hmst");
    return nullptr;
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const command& arg){
    return arg.clone();
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const variable& arg){
    throw evaluation_error("Unresolved variable: " + arg.data);
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const constant& arg){
    mpfr::mpreal mpr;
    //std::cout << precision << "\n";
    switch(arg.repr.index()){
        case 0:
        mpr = mpfr::mpreal(std::get<0>(arg.repr).get_mpz_t(), precision);
        break;
        case 1:
        mpr = mpfr::mpreal(std::get<1>(arg.repr).get_mpq_t(), precision);
        break;
        case 2:
        mpr = mpfr::mpreal(std::get<2>(arg.repr).get_mpf_t());
        mpr.set_prec(precision);
        break;
    }
    mpf_class ret;
    ret.set_prec(precision);
    mpfr_get_f(ret.get_mpf_t(), mpr.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
    return std::make_unique<constant>(ret);
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const operator_expression& arg){
    return arg.clone();
}
std::unique_ptr<expression> numerical_evaluator_visitor::visit(const tensor_expression& arg){
    return arg.clone();
}