#include <memory.hpp>
notebook current_notebook;
std::unordered_map<std::string, std::unique_ptr<expression>> variables;
std::unordered_map<std::string, std::unique_ptr<expression>> functions;