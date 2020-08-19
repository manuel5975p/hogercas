#include <parser.hpp>
#include <memory.hpp>
#include <iostream>
#include <unordered_map>
#include <initializer_list>
#include <chrono>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <thread>
#include <atomic>
#include <readline/readline.h>
#include <readline/history.h>
#include <traverse.hpp>
#include <replace.hpp>
#include <colors.hpp>
#include <commands.hpp>
#include <derivative_visitor.hpp>
#include <evaluator_visitor.hpp>
#include <expansion_visitor.hpp>
#include <expression_compare.hpp>
#include <flatten.hpp>
#include <ProgramOptions.hxx>
#include <serializer_visitor.hpp>
#include <deserializer.hpp>
#include <cstring>
#include <filesystem>
std::vector<std::string> vocabulary{"Clear", "Solve"};
unsigned long long nanoTime(){
    using namespace std::chrono;
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}
char* completion_generator(const char* text, int state) {
    static std::vector<std::string> matches;
    static size_t match_index = 0;
    if (state == 0) {
        matches.clear();
        match_index = 0;
        std::string textstr = std::string(text);
        for (auto word : vocabulary) {
            if (word.size() >= textstr.size() &&
                word.compare(0, textstr.size(), textstr) == 0) {
                matches.push_back(word);
            }
        }
        for(auto& wordp : current_notebook.variables){
            auto& word = wordp.first;
            if (word.size() >= textstr.size() &&
                word.compare(0, textstr.size(), textstr) == 0) {
                matches.push_back(word);
            }
        }
        for(auto& wordp : current_notebook.functions){
            auto& word = wordp.first;
            if (word.size() >= textstr.size() &&
                word.compare(0, textstr.size(), textstr) == 0) {
                matches.push_back(word);
            }
        }
    }
    if (match_index >= matches.size()) {
        return nullptr;
    }
    else {
        return strdup(matches[match_index++].c_str());
    }
}

char** completer(const char* text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, completion_generator);
}

void sig_handler(int sig){
    puts("Use Ctrl+D to exit");
}
int main(int argc, char** argv){
    if (signal(SIGINT, sig_handler) == SIG_ERR){
        //std::cout << "Shit" << std::endl;
    }
    std::unordered_map<std::string, decltype(&mpfr::exp)> funcs;
    rl_attempted_completion_function = completer;
    po::parser parser;
    std::vector<std::string> files;
    parser[""].bind(files);
    if(!parser(argc, argv)) {
        std::cerr << "errors occurred; aborting\n";
        return -1;
    }
    if(files.size() >= 1){
        if(files.size() > 1){
            std::cout << "Ignoring everything but the first file\n";
        }
        current_notebook = notebook(files[0]);
    }
    else{
        std::cout << "No notebook path specified...\n";
        std::string path = ".";
        for (const auto & entry : std::filesystem::directory_iterator(path)){
            if(entry.path().string().find(".nb") == entry.path().string().size() - 3){
                std::cout << "Opening " << entry.path().string() << "\n";
                current_notebook = notebook(entry.path().string());
                goto brake;
            }
        }
        std::cout << "Creating new notebook\n";
        brake:
        (void)0;
    }
    std::cout << "Welcome to Hogercas! Please type a command:" << std::endl;
    std::string a;
    while(true){
        try{
            std::unique_ptr<char, decltype(&std::free)> redl(readline(">> "), std::free);
            if(redl == nullptr)break;
            a = redl.get();
            add_history(redl.get());
            std::unique_ptr<expression> e = parse(a);
            if(auto v = dynamic_cast<operation_expression*>(e.get())){
                if(v->m_operation == assignment){
                    apply(v);
                }
                else{
                    //auto e2 = evaluate(e);
                    evaluator_visitor ev;
                    auto e2 = e->accept(ev);
                    e2->print();
                    std::cout << std::endl;
                    serializer_visitor srv;
                    //e->accept(srv);
                    //std::unique_ptr<expression> exp = deserialize_expression(srv.data);
                    //exp->print();std::cout << std::endl;
                    //std::vector<std::unique_ptr<expression>> flat = flatten(e);
                    //for(auto it = flat.begin();it != flat.end();it++){
                    //    (**it).print();
                    //    std::cout << ", ";
                    //}
                    //std::cout << std::endl;
                }
            }
            else if(auto v = dynamic_cast<command*>(e.get())){
                auto opt = execute(v);
                if(opt){
                    std::cout << opt.value() << "\n";
                }
            }
            else if(auto v = dynamic_cast<tensor_expression*>(e.get())){
                v->print();
                std::cout << "\n\n";
            }
            else{
                //auto e2 = evaluate(e);
                evaluator_visitor ev;
                auto e2 = e->accept(ev);
                e2->print();
                std::cout << "\n";
            }
        }
        catch(int i){
            std::cout << FBLU("Invoked a function on the expression base class. This is a program bug.") << std::endl;
            std::terminate();
        }
        catch(tokenizer_error& e){
            
            if(e.error_range.first | e.error_range.second){
                for(int i = 0;i < e.error_range.first + 3;i++){
                    std::cout << " ";
                }
                if(e.error_range.first != e.error_range.second){
                    for(int i = 0;i < e.error_range.second - e.error_range.first;i++){
                        std::cout << FRED("~");
                    }
                    std::cout << std::endl;
                }
                else{
                    std::cout << FRED("^") << std::endl;
                }
            }
            std::cout << ("tokenizer_error: ") << e.what() << std::endl;
            
        }
        catch(parser_error& e){
            
            if(e.error_range.first | e.error_range.second){
                //std::cout << a << "\n";
                
                for(int i = 0;i < e.error_range.first + 3;i++){
                    std::cout << " ";
                }
                if(e.error_range.first != e.error_range.second){
                    for(int i = 0;i < e.error_range.second - e.error_range.first;i++){
                        std::cout << FRED("\u00AF");
                    }
                    std::cout << std::endl;
                }
                else{
                    std::cout << FRED("^") << std::endl;
                }
            }
            std::cout << ("parser_error: ") << e.what() << std::endl;
        }
        catch(evaluation_error& e){
            std::cout << ("evaluation_error: ") << e.what() << std::endl;
        }
        catch(invalid_command& e){
            std::cout << ("invalid_command_error: ") << e.what() << std::endl;
        }
    }
    current_notebook.save("autosave.nb");
    std::cout << "\n";
    return 0;
}