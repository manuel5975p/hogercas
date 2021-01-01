#include <iostream>
#include <boost/process.hpp>
#include <vector>
#include <string>
#include <ctre.hpp>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <utility>
#include <algorithm>
#include <tuple>
#include <random>
#include <colors.hpp>
#define RANDS {"1", "2", "3", "f", "x", "(", ")", ","}
static std::vector<std::string> rands(RANDS);
struct test_case{
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
};

std::pair<char*, off_t> file_ptr(const char* filename){
    int fd = open(filename, O_RDONLY);
    if(fd == -1)return {nullptr, 0};
    off_t fsize;
    struct stat st;
    stat(filename, &st);
    fsize = st.st_size;
    char* file = (char*)mmap(0, fsize, PROT_EXEC, MAP_PRIVATE, fd, 0);
    return {file, fsize};
}
std::tuple<int, std::string, std::string> do_case(char* begin, char* end){
    namespace bp = boost::process;
    bp::ipstream out;
    bp::opstream in;
    std::string_view vw(begin, end - begin);
    bp::child c("./hogercas", "--testing", bp::std_out > out, bp::std_in < in);
    //c.detach();
    std::string str;
    //std::getline(out, str);
    //std::getline(out, str);
    //std::getline(out, str);
    unsigned line = 1;
    for(auto match : ctre::range<"\\s*\"([^\"]*)\"\\s*->\\s*\"([^\"]*)\"\\s*;">(vw)){
        //if(match.get<1>().str().find("Clear") != std::string::npos){
        //    std::cout << match.get<1>().str() << "\n";
        //}
        in << match.get<1>().str() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        if(!c.running()){
            std::cout << c.exit_code() << "\n";
            return std::make_tuple(line, match.get<2>().str(), "");
        }
        std::getline(out, str);
        
        if(match.get<2>().size() > 0){
            std::getline(out, str);
            //if(str == match.get<2>().str()){
            //    goto out;
            //}
            if(ctre::search<" *\\^ *asdfasdf">(str + "asdfasdf") || str.find('~') != std::string::npos){
                std::getline(out, str);
                if(auto match3 = ctre::search<"[a-z]*_error">(str); match3){
                    if(match3.get<0>().str() == match.get<2>().str()){
                        in << EOF << std::endl;
                        in.flush();
                        c.wait_for(std::chrono::milliseconds(10));
                        return std::make_tuple(0, std::string(), std::string());
                    }
                    else{
                        in << EOF << std::endl;
                        in.flush();
                        c.wait_for(std::chrono::milliseconds(10));
                        return std::make_tuple(line, match.get<2>().str(), match3.get<0>().str());
                    }
                }
            }
            if(auto match3 = ctre::search<"[a-z]*_error">(str); match3){
                if(match3.get<0>().str() == match.get<2>().str()){
                    in << EOF << std::endl;
                    in.flush();
                    c.wait_for(std::chrono::milliseconds(10));
                    return std::make_tuple(0, std::string(), std::string());
                }
                else{
                    in << EOF << std::endl;
                    in.flush();
                    c.wait_for(std::chrono::milliseconds(10));
                    return std::make_tuple(line, match.get<2>().str(), match3.get<0>().str());
                }
            }
            if(match.get<2>().str() != str){
                in << EOF << std::endl;
                in.flush();
                c.wait_for(std::chrono::milliseconds(10));
                return std::make_tuple(line, match.get<2>().str(), str);
            }
        }
        line++;
    }
    out:
    in << EOF << std::endl;
    in.flush();
    c.wait_for(std::chrono::milliseconds(10));
    return std::make_tuple(0, std::string(), std::string());
    
}
int main(){
    namespace bp = boost::process;
    auto filep = file_ptr("../tester/tests.tests");
    auto filec = filep.first;
    assert(filec[0] == '{');
    size_t startpos = 0;
    unsigned testnr = 1;
    std::pair<unsigned, unsigned> failsuc = std::make_pair(0u, 0u);

    while(true){
        char* hit = std::find(filec + startpos + 1, filec + filep.second,'}');
        if(hit >= filec + filep.second){
            break;
        }
        char* from = filec + startpos;
        auto s = do_case(from, hit);
        if(std::get<0>(s) != 0){
            std::cout << std::string_view(from, (hit - from) + 1) << "\nhas failed";
            std::cout << " on statement " << std::get<0>(s) << ":\n";
            if(std::get<2>(s).empty())
            std::cout << "The dumbass program outputted nothing and probably fucking died\n";
            else
            std::cout << "The dumbass program outputted " << KRED << (std::get<2>(s)) << RST << " instead of " << KGRN << std::get<1>(s) << RST << std::endl;
            failsuc.first++;
        }
        else{
            failsuc.second++;
            std::cout << KGRN << "Test " << KBLU << testnr << KGRN << " successful" << RST << std::endl;
            
            //bp::ipstream out;
            //bp::opstream in;
            //bp::child c("/usr/bin/gcovr","-r","../",".", bp::std_out > out, bp::std_in < in);
            //std::string line_from_gcov;
            //while(std::getline(out, line_from_gcov)){
            //    std::cout << line_from_gcov << "\n";
            //}
        }
        testnr++;
        startpos = hit - filec + 1;
        hit = std::find(filec + startpos, filec + filep.second,'{');
        if(hit >= filec + filep.second){
            break;
        }
        startpos = hit - filec;
    }
    if(false)
    {
        boost::process::ipstream out;
        boost::process::opstream in;
        std::random_device dev;
        std::mt19937_64 gen(dev());
        std::uniform_int_distribution<size_t> dis(0,rands.size() - 1);
        boost::process::child c("./hogercas", boost::process::std_out > out, boost::process::std_in < in);
        for(size_t i = 0;i < 1000;i++){
            size_t len = gen() % 1000;
            std::string imput;
            imput.reserve(len * 2);
            for(size_t ch = 0;ch < len;ch++)
                imput += rands[dis(gen)];
            in << imput << std::endl;
            if(!c.running()){
                failsuc.first++;
                std::cout << KRED << "Spamming test failed\n" << RST;
                goto fail;
            }
            while(out.readsome(const_cast<char*>(imput.c_str()), len));
            std::cout << imput << "\n";
            std::cout << i << std::endl;
        }
    }
    //std::cout << KGRN << "Spamming test succeeded\n" << RST;
    //failsuc.second++;
    fail:
    std::cout << KYEL << "Summary:\n" << KRED << failsuc.first << " failed   " << RST << "|   " << KGRN << failsuc.second << " succeeded\n" << RST << "\n";

    return 0;
}