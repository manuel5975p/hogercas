#ifndef NOTEBOOK_HPP
#define NOTEBOOK_HPP
#include <vector>
#include <memory>
#include <string>
#include <filesystem>
#include <unordered_map>
struct expression;
struct notebook{
    std::unordered_map<std::string, std::unique_ptr<expression>> variables;
    std::unordered_map<std::string, std::unique_ptr<expression>> functions;
    std::vector<std::unique_ptr<expression>> input_history;
    std::vector<std::unique_ptr<expression>> output_history;
    notebook(const std::filesystem::path& p);
    notebook(const std::string& p);
    notebook(const char* p);
    notebook();
    void save(const std::filesystem::path& p);
    void save(const std::string& p);
    void save(const char* p);
};
#endif