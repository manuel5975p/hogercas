#include <deserializer.hpp>
#include <parser.hpp>
std::unique_ptr<expression> deserialize_expression(std::stringbuf& strbuf){
    node_id id = (node_id)strbuf.sbumpc();
    switch(id){
        case unary_expression_e:
            return deserialize_unary_expression(strbuf);
        case bracket_expression_e:
            return deserialize_bracket_expression(strbuf);
        case variable_e:
            return deserialize_variable(strbuf);
        case derivation_operator_e:
            return deserialize_operator_expression(strbuf);
        case constant_e:
            return deserialize_constant(strbuf);
        case command_e:
            return deserialize_command(strbuf);
        case function_e:
            return deserialize_function(strbuf);
        case function_argument_e:
            return deserialize_function_argument(strbuf);
        case operation_expression_e:
            return deserialize_operation_expression(strbuf);
        case tensor_expression_e:
            return deserialize_tensor_expression(strbuf);
        default:
            throw 5;
    }
}
std::unique_ptr<expression> deserialize_unary_expression(std::stringbuf& strbuf){
    unary_operation type = (unary_operation)strbuf.sbumpc();
    return std::make_unique<unary_expression>(type, deserialize_expression(strbuf));
}
std::unique_ptr<expression> deserialize_operation_expression(std::stringbuf& strbuf){
    operation type = (operation)strbuf.sbumpc();
    auto p1 = deserialize_expression(strbuf);
    auto p2 = deserialize_expression(strbuf);
    return std::make_unique<operation_expression>(type, std::move(p1), std::move(p2));
}
std::unique_ptr<expression> deserialize_bracket_expression(std::stringbuf& strbuf){
    return std::make_unique<bracket_expression>(deserialize_expression(strbuf));
}
std::unique_ptr<expression> deserialize_function(std::stringbuf& strbuf){
    size_t argc;
    strbuf.sgetn((char*)&argc, sizeof(size_t));
    std::vector<std::unique_ptr<expression>> vec;
    std::string name;
    while(true){
        char getted = strbuf.sbumpc();
        if(getted)name += getted;
        else break;
    }
    vec.reserve(argc);
    for(size_t i = 0;i < argc;i++){
        vec.push_back(deserialize_expression(strbuf));
    }
    return std::make_unique<function>(name, std::move(vec));
}
std::unique_ptr<expression> deserialize_function_argument(std::stringbuf& strbuf){
    decltype(std::declval<function_argument>().number) nr;
    strbuf.sgetn((char*)&nr, sizeof(nr));
    return std::make_unique<function_argument>(nr);
}
std::unique_ptr<expression> deserialize_command(std::stringbuf& strbuf){
    size_t argc;
    strbuf.sgetn((char*)&argc, sizeof(size_t));
    std::vector<std::unique_ptr<expression>> vec;
    uint64_t cmdt;
    strbuf.sgetn((char*)&argc, sizeof(uint64_t));
    vec.reserve(argc);
    for(size_t i = 0;i < argc;i++){
        vec.push_back(deserialize_expression(strbuf));
    }
    return std::make_unique<command>((command_type)cmdt, std::move(vec));
}
std::unique_ptr<expression> deserialize_variable(std::stringbuf& strbuf){
    std::string name;
    while(true){
        char getted = strbuf.sbumpc();
        if(getted)name += getted;
        else break;
    }
    return std::make_unique<variable>(name);
}
std::unique_ptr<expression> deserialize_constant(std::stringbuf& strbuf){
    size_t index = strbuf.sbumpc();
    
    switch(index){
        case 0:
        {
            auto c = std::make_unique<constant>("0");
            size_t i_count;
            strbuf.sgetn((char*)&i_count, sizeof(size_t));
            std::vector<char> buf(i_count);
            strbuf.sgetn(buf.data(), i_count);
            mpz_import(std::get<0>(c->repr).get_mpz_t(), i_count, 1, sizeof(char), 1, 0, buf.data());
            return c;
        }
        case 1:
        {
            auto c = std::make_unique<constant>("1/2");
            size_t i_count;
            strbuf.sgetn((char*)&i_count, sizeof(size_t));
            std::vector<char> buf(i_count);
            strbuf.sgetn(buf.data(), i_count);
            mpz_import(std::get<1>(c->repr).get_num_mpz_t(), i_count, 1, sizeof(char), 1, 0, buf.data());
            i_count = 0;
            strbuf.sgetn((char*)&i_count, sizeof(size_t));
            buf.clear();
            buf.shrink_to_fit();
            buf.resize(i_count);
            strbuf.sgetn(buf.data(), i_count);
            mpz_import(std::get<1>(c->repr).get_den_mpz_t(), i_count, 1, sizeof(char), 1, 0, buf.data());
            return c;
        }
        case 2:
            throw std::invalid_argument("Improting floats is not supproted");
    }
    return nullptr;
}
std::unique_ptr<expression> deserialize_operator_expression(std::stringbuf& strbuf){
    return nullptr;
}
std::unique_ptr<expression> deserialize_tensor_expression(std::stringbuf& strbuf){
    return nullptr;
}