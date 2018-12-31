#ifndef UTILITY_H
#define UTILITY_H
#include <sys/stat.h> //for fstat
#include <string_view>
using Stat = struct stat;
std::string_view::iterator skip_past(std::string_view::iterator iter, std::string_view::iterator end, char delimiter = '\n');
#endif
