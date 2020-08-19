#include <constant_fusion.hpp>
#include <evaluator_visitor.hpp>
#include <mpreal.h>
mpq_class mpq_from_float_literal(std::string str){
    size_t it = str.find('.');
    assert(str.size() - it > 1 && "That literal is fucked");
    size_t pow10 = (str.size() - it) - 1;
    str.erase(it, 1);
    str.erase(0, str.find_first_not_of('0'));
    mpz_class denom = 10;
    mpz_pow_ui(denom.get_mpz_t(), denom.get_mpz_t(), pow10);
    return mpq_class(mpz_class(str), denom);
}
std::unique_ptr<expression> fuse(const constant* first,const constant* second, operation op){
    //std::cout << "Fusing ";
    //std::cout << std::get<0>(first->repr) << " and " << std::get<0>(second->repr) << "\n";
    if(auto left = dynamic_cast<const constant*>(first)){
        if(auto right = dynamic_cast<const constant*>(second)){
            size_t guederswitsch = left->repr.index() * 3 + right->repr.index();
            switch(guederswitsch){
                case 0: //Int <> Int
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpz_class(std::get<0>(left->repr) + std::get<0>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpz_class(std::get<0>(left->repr) - std::get<0>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpz_class(std::get<0>(left->repr) * std::get<0>(right->repr)));
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr), std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(mpq_class(std::get<0>(left->repr), std::get<0>(right->repr))).canonicalize());
                            break;
                        case power:
                            {
                                if(!std::get<0>(right->repr).fits_sint_p() || std::abs(std::get<0>(right->repr).get_si()) > 100000000ull){
                                    throw evaluation_error("Exponent " + std::get<0>(right->repr).get_str() + " is too big");
                                }
                                mpz_class bing(0);
                                mpz_pow_ui(bing.get_mpz_t(), std::get<0>(left->repr).get_mpz_t(),std::get<0>(right->repr).get_ui());
                                if(std::get<0>(right->repr).get_si() < 0){
                                    return std::make_unique<constant>(mpq_class(1, bing));
                                }
                                return std::make_unique<constant>(bing);
                            }
                            break;
                    }
                    break;
                case 1: //Int <> Rational
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) + std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) + std::get<1>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) - std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) - std::get<1>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) * std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) * std::get<1>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) / std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) / std::get<1>(right->repr)).canonicalize());
                            break;
                        case power:
                            return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            break;
                    }
                    break;
                case 2: //Int <> Float
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) + std::get<2>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) - std::get<2>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) * std::get<2>(right->repr)));
                            break;
                        case division:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) / std::get<2>(right->repr)));
                            break;
                        case power:
                            return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            break;
                    }
                    break;
                case 3: //Rational <> Int
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) + std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) + std::get<0>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) - std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) - std::get<0>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) * std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) * std::get<0>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) / std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) / std::get<0>(right->repr)).canonicalize());
                            break;
                        case power:
                            if(!std::get<0>(right->repr).fits_ulong_p() || std::get<0>(right->repr).get_ui() > 100000000ull){
                                throw evaluation_error("Exponent " + std::get<0>(right->repr).get_str() + " is too big");
                            }
                            {
                                std::uint64_t exp = std::get<0>(right->repr).get_ui();
                                mpq_class powered(1, 1);
                                mpz_pow_ui(powered.get_num().get_mpz_t(), std::get<1>(left->repr).get_num().get_mpz_t(), exp);
                                mpz_pow_ui(powered.get_den().get_mpz_t(), std::get<1>(left->repr).get_den().get_mpz_t(), exp);
                                return std::make_unique<constant>(powered);
                            }
                            break;
                    }
                    break;
                case 4: //Rational <> Rational
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) + std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) + std::get<1>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) - std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) - std::get<1>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) * std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) * std::get<1>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) / std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) / std::get<1>(right->repr)).canonicalize());
                            break;
                        case power:
                            return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            break;
                    }
                    break;
                case 5: //Rational <> Float
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) + (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) + (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) - (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) - (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) * (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) * (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) / (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) / (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case power:
                            return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            break;
                    }
                    break;
                case 6: //Float <> Int
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) + std::get<0>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) - std::get<0>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) * std::get<0>(right->repr)));
                            break;
                        case division:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) / std::get<0>(right->repr)));
                            break;
                        case power:
                            return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            break;
                    }
                    break;
                case 7: //Float <> Rational
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) + std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) + std::get<1>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) - std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) - std::get<1>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) * std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) * std::get<1>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) / std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) / std::get<1>(right->repr)).canonicalize());
                            break;
                        case power:
                            return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            break;
                    }
                    break;
                case 8: //Float <> Float
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) + std::get<2>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) - std::get<2>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) * std::get<2>(right->repr)));
                            break;
                        case division:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) / std::get<2>(right->repr)));
                            break;
                        case power:
                            return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            break;
                    }
                    break;
            }
        }
    }
    throw std::logic_error("Hmst");
}
std::unique_ptr<constant> fuse_numerical(const constant* first,const constant* second, operation op){
    //std::cout << "Fusing ";
    //std::cout << std::get<0>(first->repr) << " and " << std::get<0>(second->repr) << "\n";
    if(auto left = dynamic_cast<const constant*>(first)){
        if(auto right = dynamic_cast<const constant*>(second)){
            size_t guederswitsch = left->repr.index() * 3 + right->repr.index();
            switch(guederswitsch){
                case 0: //Int <> Int
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpz_class(std::get<0>(left->repr) + std::get<0>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpz_class(std::get<0>(left->repr) - std::get<0>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpz_class(std::get<0>(left->repr) * std::get<0>(right->repr)));
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr), std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(mpq_class(std::get<0>(left->repr), std::get<0>(right->repr))).canonicalize());
                            break;
                        case power:
                            {
                                if(!std::get<0>(right->repr).fits_sint_p() || std::abs(std::get<0>(right->repr).get_si()) > 100000000ull){
                                    throw evaluation_error("Exponent " + std::get<0>(right->repr).get_str() + " is too big");
                                }
                                mpz_class bing(0);
                                mpz_pow_ui(bing.get_mpz_t(), std::get<0>(left->repr).get_mpz_t(),std::get<0>(right->repr).get_ui());
                                if(std::get<0>(right->repr).get_si() < 0){
                                    return std::make_unique<constant>(mpq_class(1, bing));
                                }
                                return std::make_unique<constant>(bing);
                            }
                            break;
                    }
                    break;
                case 1: //Int <> Rational
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) + std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) + std::get<1>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) - std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) - std::get<1>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) * std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) * std::get<1>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<0>(left->repr) / std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<0>(left->repr) / std::get<1>(right->repr)).canonicalize());
                            break;
                        case power:
                            mpfr::mpreal _base(std::get<0>(left->repr).get_mpz_t());
                            mpfr::mpreal _exp(std::get<1>(right->repr).get_mpq_t());
                            mpfr::mpreal res = mpfr::pow(_base, _exp);
                            mpf_class ret;
                            mpfr_get_f(ret.get_mpf_t(), res.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
                            return std::make_unique<constant>(ret);
                            break;
                    }
                    break;
                case 2: //Int <> Float
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) + std::get<2>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) - std::get<2>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) * std::get<2>(right->repr)));
                            break;
                        case division:
                            return std::make_unique<constant>(mpf_class(std::get<0>(left->repr) / std::get<2>(right->repr)));
                            break;
                        case power:
                            //return std::make_unique<operation_expression>(op, first->clone(), second->clone());
                            
                            mpfr::mpreal _base(std::get<0>(left->repr).get_mpz_t());
                            mpfr::mpreal _exp(std::get<2>(right->repr).get_mpf_t());
                            mpfr::mpreal res = mpfr::pow(_base, _exp);
                            mpf_class ret;
                            mpfr_get_f(ret.get_mpf_t(), res.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
                            return std::make_unique<constant>(ret);

                            break;
                    }
                    break;
                case 3: //Rational <> Int
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) + std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) + std::get<0>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) - std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) - std::get<0>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) * std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) * std::get<0>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) / std::get<0>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) / std::get<0>(right->repr)).canonicalize());
                            break;
                        case power:
                            if(!std::get<0>(right->repr).fits_ulong_p() || std::get<0>(right->repr).get_ui() > 100000000ull){
                                throw evaluation_error("Exponent " + std::get<0>(right->repr).get_str() + " is too big");
                            }
                            {
                                std::uint64_t exp = std::get<0>(right->repr).get_ui();
                                mpq_class powered(1, 1);
                                mpz_pow_ui(powered.get_num().get_mpz_t(), std::get<1>(left->repr).get_num().get_mpz_t(), exp);
                                mpz_pow_ui(powered.get_den().get_mpz_t(), std::get<1>(left->repr).get_den().get_mpz_t(), exp);
                                return std::make_unique<constant>(powered);
                            }
                            break;
                    }
                    break;
                case 4: //Rational <> Rational
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) + std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) + std::get<1>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) - std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) - std::get<1>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) * std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) * std::get<1>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) / std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) / std::get<1>(right->repr)).canonicalize());
                            break;
                        case power:
                            mpfr::mpreal _base(std::get<1>(left->repr).get_mpq_t());
                            mpfr::mpreal _exp(std::get<1>(right->repr).get_mpq_t());
                            mpfr::mpreal res = mpfr::pow(_base, _exp);
                            mpf_class ret;
                            mpfr_get_f(ret.get_mpf_t(), res.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
                            return std::make_unique<constant>(ret);
                            break;
                    }
                    break;
                case 5: //Rational <> Float
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) + (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) + (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) - (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) - (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) * (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) * (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class(std::get<1>(left->repr) / (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float) : mpq_class(std::get<2>(right->repr))));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class(std::get<1>(left->repr) / (right->actual_if_float.size() ? mpq_from_float_literal(right->actual_if_float).canonicalize() : mpq_class(std::get<2>(right->repr)).canonicalize())));
                            break;
                        case power:
                            mpfr::mpreal _base(std::get<1>(left->repr).get_mpq_t());
                            mpfr::mpreal _exp(std::get<2>(right->repr).get_mpf_t());
                            mpfr::mpreal res = mpfr::pow(_base, _exp);
                            mpf_class ret;
                            mpfr_get_f(ret.get_mpf_t(), res.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
                            return std::make_unique<constant>(ret);
                            break;
                    }
                    break;
                case 6: //Float <> Int
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) + std::get<0>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) - std::get<0>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) * std::get<0>(right->repr)));
                            break;
                        case division:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) / std::get<0>(right->repr)));
                            break;
                        case power:
                            mpfr::mpreal _base(std::get<2>(left->repr).get_mpf_t());
                            mpfr::mpreal _exp(std::get<0>(right->repr).get_mpz_t());
                            mpfr::mpreal res = mpfr::pow(_base, _exp);
                            mpf_class ret;
                            mpfr_get_f(ret.get_mpf_t(), res.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
                            return std::make_unique<constant>(ret);
                            break;
                    }
                    break;
                case 7: //Float <> Rational
                    switch(op){
                        case addition:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) + std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) + std::get<1>(right->repr)).canonicalize());
                            break;
                        case subtraction:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) - std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) - std::get<1>(right->repr)).canonicalize());
                            break;
                        case multiplication:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) * std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) * std::get<1>(right->repr)).canonicalize());
                            break;
                        case division:
                            {
                                mpq_class ret = mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) / std::get<1>(right->repr));
                                ret.canonicalize();
                                return std::make_unique<constant>(ret);
                            }
                            //return std::make_unique<constant>(mpq_class((left->actual_if_float.size() ? mpq_from_float_literal(left->actual_if_float) : mpq_class(std::get<2>(left->repr))) / std::get<1>(right->repr)).canonicalize());
                            break;
                        case power:
                            mpfr::mpreal _base(std::get<2>(left->repr).get_mpf_t());
                            mpfr::mpreal _exp(std::get<1>(right->repr).get_mpq_t());
                            mpfr::mpreal res = mpfr::pow(_base, _exp);
                            mpf_class ret;
                            mpfr_get_f(ret.get_mpf_t(), res.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
                            return std::make_unique<constant>(ret);
                            break;
                    }
                    break;
                case 8: //Float <> Float
                    switch(op){
                        case addition:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) + std::get<2>(right->repr)));
                            break;
                        case subtraction:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) - std::get<2>(right->repr)));
                            break;
                        case multiplication:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) * std::get<2>(right->repr)));
                            break;
                        case division:
                            return std::make_unique<constant>(mpf_class(std::get<2>(left->repr) / std::get<2>(right->repr)));
                            break;
                        case power:
                            mpfr::mpreal _base(std::get<2>(left->repr).get_mpf_t());
                            mpfr::mpreal _exp(std::get<2>(right->repr).get_mpf_t());
                            mpfr::mpreal res = mpfr::pow(_base, _exp);
                            mpf_class ret;
                            mpfr_get_f(ret.get_mpf_t(), res.mpfr_srcptr(), mpfr::mpreal::get_default_rnd());
                            return std::make_unique<constant>(ret);
                            break;
                    }
                    break;
            }
        }
    }
    throw std::logic_error("Hmst");
}
bool constant_equal(const constant* l, const constant* r) {
    size_t switchdex = l->repr.index() * 3 + r->repr.index();
    switch(switchdex){
        case 0:
            return std::get<0>(l->repr) == std::get<0>(r->repr);
        case 1:
            return std::get<0>(l->repr) == std::get<1>(r->repr);
        case 2:
            return std::get<0>(l->repr) == std::get<2>(r->repr);
        case 3:
            return std::get<1>(l->repr) == std::get<0>(r->repr);
        case 4:
            return std::get<1>(l->repr) == std::get<1>(r->repr);
        case 5:
            return std::get<1>(l->repr) == std::get<2>(r->repr);
        case 6:
            return std::get<2>(l->repr) == std::get<0>(r->repr);
        case 7:
            return std::get<2>(l->repr) == std::get<1>(r->repr);
        case 8:
            return std::get<2>(l->repr) == std::get<2>(r->repr);
        default:
        throw std::logic_error("Program bug");
    }
}