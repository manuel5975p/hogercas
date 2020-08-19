#include <serializer_visitor.hpp>
#include <parser.hpp>

std::unique_ptr<expression> serializer_visitor::visit(const expression& arg){
    throw 213;
}
std::unique_ptr<expression> serializer_visitor::visit(const unary_expression& arg){
    data.sputc(arg.id());
    data.sputc(arg.type);
    arg.child->accept(*this);
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const operation_expression& arg){
    data.sputc(arg.id());
    data.sputc(arg.m_operation);
    arg.left->accept(*this);
    arg.right->accept(*this);
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const bracket_expression& arg){
    data.sputc(arg.id());
    arg.child->accept(*this);
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const function& arg){
    data.sputc(arg.id());
    size_t s = arg.args.size();
    data.sputn((char*)&s, 8);
    data.sputn(arg.name.c_str(), arg.name.size() + 1);
    for(auto& arg : arg.args){
        arg->accept(*this);
    }
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const function_argument& arg){
    data.sputc(arg.id());
    data.sputn((char*)&arg.number, sizeof(arg.number));
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const command& arg){
    data.sputc(arg.id());
    size_t s = arg.args.size();
    data.sputn((char*)&s, 8);
    data.sputn((char*)&arg.cmd, 8);
    for(auto& arg : arg.args){
        arg->accept(*this);
    }
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const variable& arg){
    data.sputc(arg.id());
    //size_t s = arg.data.size();
    //data.sputn((char*)&s, 8);
    data.sputn(arg.data.c_str(), arg.data.size() + 1);
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const constant& arg){
    data.sputc(arg.id());
    data.sputc(arg.repr.index());
    switch(arg.repr.index()){
        case 0:
            {
                std::vector<char> buf(mpz_sizeinbase(std::get<0>(arg.repr).get_mpz_t(), 2) / 8 + 16);
                size_t o_count;
                mpz_export (buf.data(), &o_count, 1, sizeof(char), 1, 0, std::get<0>(arg.repr).get_mpz_t());
                data.sputn((char*)&o_count, sizeof(size_t));
                data.sputn(buf.data(), o_count);
            }
            break;
        case 1:
            {
                std::vector<char> buf(mpz_sizeinbase(std::get<1>(arg.repr).get_num_mpz_t(), 2) / 8 + 16);
                size_t o_count;
                mpz_export (buf.data(), &o_count, 1, sizeof(char), 1, 0, std::get<1>(arg.repr).get_num_mpz_t());
                data.sputn((char*)&o_count, 8);
                data.sputn(buf.data(), o_count);
                buf.clear();
                buf.shrink_to_fit();
                buf.resize(mpz_sizeinbase(std::get<1>(arg.repr).get_den_mpz_t(), 2) / 8 + 16);
                o_count = 0;
                mpz_export (buf.data(), &o_count, 1, sizeof(char), 1, 0, std::get<1>(arg.repr).get_den_mpz_t());
                data.sputn((char*)&o_count, 8);
                data.sputn(buf.data(), o_count);
            }
        case 2:
            throw std::invalid_argument("Exproting floats is not supproted");
    }
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const operator_expression& arg){
    data.sputc(arg.id());
    arg.child->accept(*this);
    return nullptr;
}
std::unique_ptr<expression> serializer_visitor::visit(const tensor_expression& arg){
    //data.sputc(arg.id());
    //arg.child->accept(*this);
    return nullptr;
}