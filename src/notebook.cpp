#include <notebook.hpp>
#include <fstream>
#include <serializer_visitor.hpp>
#include <deserializer.hpp>
#include <parser.hpp>
notebook::notebook(){}
notebook::notebook(const std::filesystem::path& p){
    std::ifstream ifile(p, std::ios_base::binary);
    size_t varsize;
    size_t funcsize;
    size_t inpsize;
    size_t outsize;
    ifile.read((char*)&varsize, sizeof(size_t));
    ifile.read((char*)&funcsize, sizeof(size_t));
    ifile.read((char*)&inpsize, sizeof(size_t));
    ifile.read((char*)&outsize, sizeof(size_t));
    std::stringbuf buffer;
    while (!ifile.eof()){
        buffer.sputc(ifile.get());
    }
    for(size_t i = 0;i < varsize;i++){
        std::string varname;
        while(true){
            char x = buffer.sbumpc();
            if(x) varname += x;
            else break;
        }
        std::unique_ptr<expression> deser = deserialize_expression(buffer);
        variables.insert(std::make_pair(std::move(varname), std::move(deser)));
    }
    for(size_t i = 0;i < funcsize;i++){
        std::string funcname;
        while(true){
            char x = buffer.sbumpc();
            if(x) funcname += x;
            else break;
        }
        std::unique_ptr<expression> deser = deserialize_expression(buffer);
        functions.insert(std::make_pair(std::move(funcname), std::move(deser)));
    }
    for(size_t i = 0;i < inpsize;i++){
        std::unique_ptr<expression> deser = deserialize_expression(buffer);
        input_history.push_back(std::move(deser));
    }
    for(size_t i = 0;i < outsize;i++){
        std::unique_ptr<expression> deser = deserialize_expression(buffer);
        output_history.push_back(std::move(deser));
    }
}
notebook::notebook(const std::string& p) : notebook(std::filesystem::path(p)){

}
notebook::notebook(const char* p) : notebook(std::filesystem::path(p)){

}
void notebook::save(const std::filesystem::path& p){
    std::ofstream ofile(p, std::ios_base::binary);
    size_t size = variables.size();
    ofile.write((char*)&size, sizeof(size_t));
    size = functions.size();
    ofile.write((char*)&size, sizeof(size_t));
    size = input_history.size();
    ofile.write((char*)&size, sizeof(size_t));
    size = output_history.size();
    ofile.write((char*)&size, sizeof(size_t));
    for(auto it = variables.begin();it != variables.end();it++){
        ofile.write(it->first.c_str(), it->first.size() + 1);
        serializer_visitor srv;
        it->second->accept(srv);
        ofile.write(srv.data.str().c_str(), srv.data.str().size());
    }
    for(auto it = functions.begin();it != functions.end();it++){
        ofile.write(it->first.c_str(), it->first.size() + 1);
        serializer_visitor srv;
        it->second->accept(srv);
        ofile.write(srv.data.str().c_str(), srv.data.str().size());
    }
    for(auto it = input_history.begin();it != input_history.end();it++){
        serializer_visitor srv;
        (**it).accept(srv);
        ofile.write((char*)&size, sizeof(size_t));
    }
    for(auto it = output_history.begin();it != output_history.end();it++){
        serializer_visitor srv;
        (**it).accept(srv);
        ofile.write((char*)&size, sizeof(size_t));
    }
    ofile.close();
}
void notebook::save(const std::string& p){
    save(std::filesystem::path(p));
}
void notebook::save(const char* p){
    save(std::filesystem::path(p));
}